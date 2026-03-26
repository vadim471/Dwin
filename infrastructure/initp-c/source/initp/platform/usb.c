#include <initp/platform/usb.h>
#include <initp/system/debug.h>

#include <string.h>
#include <stdio.h>

#ifdef WSL_BUILD
#include <libusb-1.0/libusb.h>
#else
#include <libusb.h>
#endif

static uint8_t sys_open_usb_context(sys_usb_context_tp object) {

    // Open context
    libusb_context* ctx;
    int r = libusb_init(&ctx);
    if (r < 0) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_open_usb_context", "Failed to initialize libusb");
        return 0;
    }
    //libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, context->debug_level);
    //libusb_set_debug(ctx, 5);

    // Open device
    libusb_device_handle* dev_handle = libusb_open_device_with_vid_pid(ctx, object->vendor_id, object->product_id);
    if (!dev_handle) {
        libusb_exit(ctx);
        return 0;
    }

    // Detach kernel driver
    if (libusb_kernel_driver_active(dev_handle, 0) == 1) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_open_usb_context", "Kernel driver active");
        if (libusb_detach_kernel_driver(dev_handle, 0) == 0) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_open_usb_context", "Kernel driver detached");
        } else {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_open_usb_context", "Couldn't detach kernel driver");
            libusb_close(dev_handle);
            libusb_exit(ctx);
            return 0;
        }
    }

    // Claim interface
    int e = libusb_claim_interface(dev_handle, object->interface_id);
    if (e < 0) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_open_usb_context", "Cannot claim interface");
        libusb_close(dev_handle);
        libusb_exit(ctx);
        return 0;
    } else {
        sys_debug_print(SYSTEM_LEVEL_INFO, "sys_open_usb_context", "Claimed interface %d", object->interface_id);
    }

    object->context = (void*)ctx;
    object->device = (void*)dev_handle;
    return 1;
}

static uint8_t sys_reopen_usb_context(sys_usb_context_tp object) {
    sys_close_usb_context(object);
    return sys_open_usb_context(object);
}

static uint8_t sys_usb_context_is_opened(sys_usb_context_tp object) {
    if (!object->context ||
        !object->device) {
        return 0;
    } else return 1;
}

void sys_init_usb_context(sys_usb_context_tp object) {
    if (!object) return;
    object->context = NULL;
    object->device = NULL;
    object->vendor_id = 0;
    object->product_id = 0;
    object->interface_id = 0;
    object->endpoint_in = 0;
    object->endpoint_out = 0;
    object->report_size = 0x40;
    object->timeout_read = 1;
    object->timeout_write = 10;
}

uint8_t sys_connect_usb_device(sys_usb_context_tp object, uint16_t vid, uint16_t pid, uint8_t ifc, uint8_t ep_in, uint8_t ep_out)
{

//    printf( "sys_connect_usb_device : CONNECT USB DEVICE VID = %s PID = %s\n", vid, pid );
    uint8_t reconnect = 0;

    if (object->vendor_id != vid) {
        object->vendor_id = vid;
        reconnect = 1;
    }
    if (object->product_id != pid) {
        object->product_id = pid;
        reconnect = 1;
    }
    if (object->interface_id != ifc) {
        object->interface_id = ifc;
        reconnect = 1;
    }
    if (object->endpoint_in != ep_in) {
        object->endpoint_in = ep_in;
    }
    if (object->endpoint_out != ep_out) {
        object->endpoint_out = ep_out;
    }

    if (reconnect) {
        return sys_reopen_usb_context(object);
    } else {
        if (!sys_usb_context_is_opened(object)) {
            return sys_open_usb_context(object);
        } else return 1;
    }
}

void sys_close_usb_context(sys_usb_context_tp object) {
    if (object->device) {
        libusb_release_interface(object->device, object->interface_id);
        libusb_close(object->device);
        object->device = NULL;
    }
    if (object->context) {
        libusb_exit((libusb_context*)object->context);
        object->context = NULL;
    }
}

size_t sys_read_usb_device(sys_usb_context_tp object, char* data, size_t size) {

    if (!sys_usb_context_is_opened(object) &&
        !sys_open_usb_context(object)) return 0;

    int transferred = 0;
    int e = libusb_bulk_transfer(object->device, object->endpoint_in, (unsigned char*)data, size, &transferred, object->timeout_read);
    if (e == LIBUSB_SUCCESS) {
        return (size_t)transferred;
    } else if (e == LIBUSB_ERROR_TIMEOUT) {
        return 0;
    } else {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_read_usb_device", "Error in read! e = %d and transferred = %d", e, transferred);
        sys_close_usb_context(object);
        return 0;
    }
}

size_t sys_write_usb_device(sys_usb_context_tp object, const char* data, size_t length) {

    if (!sys_usb_context_is_opened(object) &&
        !sys_open_usb_context(object)) return 0;

    size_t bytes = (object->report_size > SYSTEM_USB_BUFFER_SIZE) ? SYSTEM_USB_BUFFER_SIZE : object->report_size;
    if (length < bytes) bytes = length;
    if (bytes < 1) return bytes;

    memcpy(object->buffer, data, bytes);

    int transferred = 0;
    int e = libusb_bulk_transfer(object->device, object->endpoint_out, (unsigned char*)object->buffer, length, &transferred, object->timeout_write);
    if (e == LIBUSB_SUCCESS) {
        return (size_t)transferred;
    } else if (e == LIBUSB_ERROR_TIMEOUT) {
        return 0;
    } else {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_write_usb_device", "Error in write! e = %d and transferred = %d", e, transferred);
        sys_close_usb_context(object);
        return 0;
    }
}

static void sys_usb_print_devs(libusb_device **devs) {

	libusb_device *dev;
	int i = 0, j = 0;
	uint8_t path[8];

	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_usb_print_devs", "Failed to get device descriptor");
			return;
		}

		printf("%04x:%04x (bus %d, device %d)",
			desc.idVendor, desc.idProduct,
			libusb_get_bus_number(dev), libusb_get_device_address(dev));

		r = libusb_get_port_numbers(dev, path, sizeof(path));
		if (r > 0) {
			printf(" path: %d", path[0]);
			for (j = 1; j < r; j++)
				printf(".%d", path[j]);
		}
		printf("\n");
	}
}

void sys_dump_usb_devices(void) {

    libusb_device **devs;

    libusb_context* ctx;
    int r = libusb_init(&ctx);
    if (r < 0) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_dump_usb_devices", "Failed to initialize libusb");
        return;
    }

    ssize_t cnt = libusb_get_device_list(NULL, &devs);
	if (cnt < 0) {
		libusb_exit(NULL);
		return;
	}

	sys_usb_print_devs(devs);
	libusb_free_device_list(devs, 1);

	libusb_exit(NULL);
}

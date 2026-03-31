#ifndef INITPLUS_PLATFORM_USB_H_INCLUDED
#define INITPLUS_PLATFORM_USB_H_INCLUDED

/**
 * \file
 */

#include <stdint.h>
#include <stddef.h>

#define SYSTEM_USB_BUFFER_SIZE 0x45

typedef struct {
    void* context;
    void* device;
    char buffer[SYSTEM_USB_BUFFER_SIZE];
    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t interface_id;
    uint8_t endpoint_in;
    uint8_t endpoint_out;
    uint8_t report_size;
    uint8_t timeout_read;
    uint8_t timeout_write;
} sys_usb_context_t;

typedef sys_usb_context_t* sys_usb_context_tp;

void sys_init_usb_context(sys_usb_context_tp object);

uint8_t sys_connect_usb_device(sys_usb_context_tp object, uint16_t vid, uint16_t pid, uint8_t ifc, uint8_t ep_in, uint8_t ep_out);

void sys_close_usb_context(sys_usb_context_tp object);

size_t sys_read_usb_device(sys_usb_context_tp object, char* data, size_t size);

typedef size_t(*sys_read_usb_device_fp)(sys_usb_context_tp, char*, size_t);

size_t sys_write_usb_device(sys_usb_context_tp object, const char* data, size_t length);

typedef size_t(*sys_usb_device_write_fp)(sys_usb_context_tp, const char*, size_t);

void sys_dump_usb_devices(void);

#endif // INITPLUS_PLATFORM_USB_H_INCLUDED

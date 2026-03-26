#include <itp/debug.h>
#include <itp/bus.h>
#include <itp/stack_trace.h>
#include <itp/memory.h>

#include <string.h>

itp_error_code_t itp_init_bus_endpoint(itp_bus_endpoint_tp endpoint, itp_bus_tp bus) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_bus_endpoint> Endpoint is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!bus) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_bus_endpoint> Bus is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    endpoint->length = 0;
    endpoint->bus = bus;
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_init_bus(itp_bus_tp bus, uint8_t echo) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!bus) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_init_bus> Bus is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    bus->data = NULL;
    bus->size = 0;
    bus->count = 0;
    bus->echo = 0;
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_expand_bus(itp_bus_tp bus, uint8_t size) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!bus) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_expand_bus> Bus is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (bus->data) {
        itp_size_t new_size = bus->size + size;
        if (new_size > 0xFF) {
            itp_debug_print(ITP_ERRC_OUT_OF_BOUNDS, "itp_expand_bus> Bus array out of bounds");
            itp_print_stack_trace(stdout);
            return ITP_ERRC_OUT_OF_BOUNDS;
        }
        if (bus->size > 0) {
            itp_bus_endpoint_tp* data = itp_malloc((uint32_t)new_size * sizeof(itp_bus_endpoint_tp), "itp_expand_bus:data:1");
            if (!data) {
                itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_expand_bus> Failed to allocate memory for bus array");
                return ITP_ERRC_MALLOC_FAILED;
            }
            memcpy(data, bus->data, bus->size * sizeof(itp_bus_endpoint_tp));
            memset(data + bus->size, 0, size * sizeof(itp_bus_endpoint_tp));
            itp_free(bus->data);
            bus->data = data;
            bus->size = (uint8_t)new_size;
        } else {
            itp_free(bus->data);
            bus->data = itp_malloc((uint32_t)size * sizeof(itp_bus_endpoint_tp), "itp_expand_bus:data:2");
            if (!bus->data) {
                itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_expand_bus> Failed to allocate memory for bus array");
                return ITP_ERRC_MALLOC_FAILED;
            }
            memset(bus->data, 0, size * sizeof(itp_bus_endpoint_tp));
            bus->size = size;
        }
    } else {
        bus->data = itp_malloc((uint32_t)size * sizeof(itp_bus_endpoint_tp), "itp_expand_bus:data:3");
        if (!bus->data) {
            itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_expand_bus> Failed to allocate memory for bus array");
            return ITP_ERRC_MALLOC_FAILED;
        }
        bus->size = size;
        if (bus->count > 0) {
            bus->count = 0;
        }
    }
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_join_bus(itp_bus_tp bus, itp_endpoint_tp ep) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!bus) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_join_bus> Bus is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    if (!ep) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_join_bus> Endpoint is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_NULL_POINTER;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    // Добавляем в конец массива
    itp_error_code_t errc;
    if (bus->count >= bus->size) {
        errc = itp_expand_bus(bus, 1);
        if (errc) return errc;
    }
    itp_bus_endpoint_tp endpoint = (itp_bus_endpoint_tp)itp_malloc(sizeof(itp_bus_endpoint_t), "itp_join_bus:itp_bus_endpoint");
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_MALLOC_FAILED, "itp_join_bus> Failed to allocate memory for bus endpoint");
        return ITP_ERRC_MALLOC_FAILED;
    }
    errc = itp_init_bus_endpoint(endpoint, bus);
    if (errc) return errc;
    errc = itp_init_endpoint(ep, &itp_bus_read_data, &itp_bus_write_data, (void*)endpoint);
    if (errc) return errc;
    bus->data[bus->count++] = endpoint;
    return ITP_ERRC_NONE;
}

void itp_clear_bus(itp_bus_tp bus) {

    #ifdef ITP_ENABLE_NULLPTR_CHECKS
    if (!bus) {
        itp_debug_print(ITP_ERRC_NULL_POINTER, "itp_clear_bus> Bus is null");
        itp_print_stack_trace(stdout);
        return;
    }
    #endif // ITP_ENABLE_NULLPTR_CHECKS

    if (bus->data) {
        uint16_t count = (uint16_t)bus->count;
        for (uint16_t i = 0; i < count; i++)
            itp_free(bus->data[i]);
        itp_free(bus->data);
        bus->data = NULL;
    }
    bus->count = 0;
    bus->size = 0;
}

itp_size_t itp_bus_write_data(void* object, const itp_byte_t* data, itp_size_t length) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!object) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_bus_write_data> Object is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_bus_endpoint_tp ep = (itp_bus_endpoint_tp)object;
    itp_bus_tp bus = ep->bus;
    if (!bus) {
        itp_debug_print(ITP_ERRC_EP_NOT_CONFIGURED, "itp_bus_write_data> Bus is null");
        itp_print_stack_trace(stdout);
        return 0;
    }

    if (length < 1) return length;

    itp_size_t max = (itp_size_t)ITP_BUS_BUFFER_SIZE, cap;
    const itp_size_t count = (itp_size_t)bus->count;
    for (itp_size_t i = 0; i < count; ++i) {
        ep = bus->data[i];
        if ((ep == object) && !bus->echo) continue;
        cap = ITP_BUS_BUFFER_SIZE - ep->length;
        if (cap < max) max = cap;
    }

    if (length > max) length = max;
    for (itp_size_t i = 0; i < count; ++i) {
        ep = bus->data[i];
        if ((ep == object) && !bus->echo) continue;
        memcpy(ep->data + ep->length, data, length);
        //itp_debug_print(PDE_ERRC_TRACE, "itp_bus_write_data> Written %d bytes to 0x%x and object 0x%x", length, (unsigned int)ep, (unsigned int)object);
        ep->length += length;
    }
    return length;
}

itp_size_t itp_bus_read_data(void* object, itp_byte_t* data, itp_size_t size) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!object) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_bus_read_data> Object is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_bus_endpoint_tp ep = (itp_bus_endpoint_tp)object;
    itp_size_t length = ep->length;
    if (size >= ep->length) {
        length = ep->length;
        memcpy(data, ep->data, length);
        ep->length = 0;
    } else {
        length = size;
        memcpy(data, ep->data, length);
        memcpy(ep->data, ep->data + length, ep->length - length);
        ep->length -= length;
    }
    return length;
}

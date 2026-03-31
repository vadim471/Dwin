#include <itp/debug.h>
#include <itp/tunnel.h>
#include <itp/stack_trace.h>

#include <string.h>

itp_error_code_t itp_init_tunnel_endpoint(itp_tunnel_endpoint_tp endpoint, itp_tunnel_endpoint_tp remote) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!endpoint) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_tunnel_endpoint> Endpoint is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    if (!remote) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_init_tunnel_endpoint> Remote endpoint is null");
        itp_print_stack_trace(stdout);
        return ITP_ERRC_INTERNAL;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    endpoint->length = 0;
    endpoint->remote = remote;
    return ITP_ERRC_NONE;
}

itp_error_code_t itp_init_tunnel(itp_tunnel_tp tunnel, itp_endpoint_tp ep0, itp_endpoint_tp ep1) {
    itp_error_code_t error;
    error = itp_init_tunnel_endpoint(&tunnel->ep[0], &tunnel->ep[1]);
    if (error) return error;
    error = itp_init_tunnel_endpoint(&tunnel->ep[1], &tunnel->ep[0]);
    if (error) return error;
    error = itp_init_endpoint(ep0, &itp_tunnel_read_data, &itp_tunnel_write_data, &tunnel->ep[1]);
    if (error) return error;
    error = itp_init_endpoint(ep1, &itp_tunnel_read_data, &itp_tunnel_write_data, &tunnel->ep[0]);
    if (error) return error;
    return ITP_ERRC_NONE;
}

itp_size_t itp_tunnel_write_data(void* object, const itp_byte_t* data, itp_size_t length) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!object) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_tunnel_write_data> Object is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_tunnel_endpoint_tp ep = ((itp_tunnel_endpoint_tp)object)->remote;
    itp_size_t max = ITP_TUNNEL_BUFFER_SIZE - ep->length;
    if (length > max) length = max;
    memcpy(ep->data + ep->length, data, length);
    ep->length += length;
    return length;
}

itp_size_t itp_tunnel_read_data(void* object, itp_byte_t* data, itp_size_t size) {

    #ifdef ITP_ENABLE_INTERNAL_CHECKS
    if (!object) {
        itp_debug_print(ITP_ERRC_INTERNAL, "itp_tunnel_read_data> Object is null");
        itp_print_stack_trace(stdout);
        return 0;
    }
    #endif // ITP_ENABLE_INTERNAL_CHECKS

    itp_tunnel_endpoint_tp ep = (itp_tunnel_endpoint_tp)object;
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

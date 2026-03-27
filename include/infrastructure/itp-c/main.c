#include <itp/itp.h>
#include <itp/tunnel.h>
#include <itp/stack_trace.h>
#include <initp/system/time.h>
#include <initp/platform/serial_port.h>

#include <stdio.h>

itp_tunnel_t t01;
itp_endpoint_t ep01, ep10;
itp_tunnel_t t02;
itp_endpoint_t ep02, ep20;
itp_tunnel_t t13;
itp_endpoint_t ep13, ep31;
itp_tunnel_t t14;
itp_endpoint_t ep14, ep41;
itp_tunnel_t t45;
itp_endpoint_t ep45, ep54;
itp_tunnel_t t06;
itp_endpoint_t ep06, ep60;
itp_tunnel_t t67;
itp_endpoint_t ep67, ep76;

itp_endpoint_t com;
sys_serial_port_t serial;
itp_endpoint_tp remote = NULL;

static itp_size_t sys_serial_read(void* object, itp_byte_t* data, itp_size_t size) {
    return (itp_size_t)sys_read_serial_port((sys_serial_port_tp)object, (char*)data, (size_t)size);
}

static itp_size_t sys_serial_write(void* object, const itp_byte_t* data, itp_size_t length) {
    return (itp_size_t)sys_write_serial_port((sys_serial_port_tp)object, (const char*)data, (size_t)length);
}

void itp_free_user_handler(void* handler) {}

void on_error(itp_root_tp root, uint8_t address, itp_error_code_t error) {
    printf("Node %d throws error %d\r\n", address, error);
}

itp_endpoint_tp on_create_1(const char* path, uint16_t* error) {
    switch (path[0]) {
        case '3':
            *error = ITP_ERR_NONE;
            return &ep13;
        case '4':
            *error = ITP_ERR_NONE;
            return &ep14;
        default:
            *error = ITP_ERR_WRONG_PATH;
            return NULL;
    }
}

itp_endpoint_tp on_create_4(const char* path, uint16_t* error) {
    switch (path[0]) {
        case '5':
            *error = ITP_ERR_NONE;
            return &ep45;
        default:
            *error = ITP_ERR_WRONG_PATH;
            return NULL;
    }
}

itp_endpoint_tp on_create_7(const char* path, uint16_t* error) {
    if (!com.object) {
        if (sys_open_serial_port(&serial, path, 9600, 8, 1, 0)) {
            itp_init_endpoint(&com, sys_serial_read, sys_serial_write, (void*)&serial);
        }
    }
    return &com;
}

void on_connect(itp_root_tp root, uint8_t address, uint16_t error) {
    if (error) {
        printf("Failed to connect node %d, error %d - %s\r\n", address, error, itp_get_error_description(error));
    } else printf("Node with address %d connected\r\n", address);
}

uint16_t on_request_0101(itp_root_tp root, itp_frame_tp frame) {
    uint16_t value;
    itp_frame_read_value_2b(frame, &value);
    printf("Node %d received value %d from %d\r\n", root->address, value, frame->from);
    value *= 2;
    itp_frame_tp response = itp_create_frame(0x0101);
    itp_frame_write_value_2b(response, &value);
    //response->error = ITP_ERR_BAD_DATA;
    itp_push_response(root, frame, response, NULL);
    return ITP_ERR_NONE;
}

uint16_t on_request_0102(itp_root_tp root, itp_frame_tp frame) {
    printf("Node %d received 0x%04x from %d\r\n", root->address, frame->command, frame->from);
    return ITP_ERR_BAD_DATA;
}

void on_response_0101(itp_root_tp root, uint16_t error, itp_frame_tp frame) {
    printf("Received result %d from node 5\r\n", error);
    if (!error) {
        uint16_t value;
        itp_frame_read_value_2b(frame, &value);
        printf("Received value: %d\r\n", value);
    }
}

void on_response_0102(itp_root_tp root, uint16_t error, itp_frame_tp frame) {
    printf("Received result 0x%04x from node %d\r\n", error, frame->from);
}

void on_receive_data(itp_root_tp root, uint8_t address, itp_byte_t* data, itp_size_t size) {
    printf("Was received data from node %d:", address);
    for (itp_size_t i = 0; i < size; ++i) {
        printf(" 0x%02x", data[i]);
    }
    printf("\r\n");
}

void on_transmit_data(itp_root_tp root, uint16_t error) {
    printf("Data was sent with result 0x%04x\r\n", error);
}

void on_trace_route(itp_root_tp root, uint16_t error, itp_frame_tp frame) {
    printf("Trace route result: %d\r\n", error);
    if (!error) {
        uint8_t* data;
        uint16_t length;
        itp_frame_read_array_1b(frame, (void**)&data, &length);
        printf("Route: ");
        for (uint8_t i = 0; i < length; i++) {
            if (i > 0) printf(", ");
            printf("%d", data[i]);
        }
        printf(" (%d)\r\n", length);
        itp_free(data);
    }
}

void on_connect_request(itp_root_tp root, uint16_t error) {
    printf("Connect request result: %d\r\n", error);
}

void on_connect_endpoint(itp_root_tp root, uint8_t address, uint16_t error, itp_endpoint_tp endpoint) {
    if (error) {
        printf("Failed to connect endpoint %d, error %d - %s\r\n", address, error, itp_get_error_description(error));
    } else printf("Endpoint with address %d connected\r\n", address);
    if (endpoint) {
        printf("It Works!\r\n");
        remote = endpoint;
    }
}

int main(int argc, char** argv) {
    itp_time_t time;
    itp_init_stack_trace(argv[0]);
    itp_init_tunnel(&t01, &ep01, &ep10);
    itp_init_tunnel(&t02, &ep02, &ep20);
    itp_init_tunnel(&t13, &ep13, &ep31);
    itp_init_tunnel(&t14, &ep14, &ep41);
    itp_init_tunnel(&t45, &ep45, &ep54);
    itp_init_tunnel(&t06, &ep06, &ep60);
    itp_init_tunnel(&t67, &ep67, &ep76);
    itp_root_tp n0 = itp_create_root(NULL);
    itp_root_tp n1 = itp_create_root(&on_create_1);
    itp_set_parent(n1, &ep10, &on_error);
    itp_root_tp n2 = itp_create_root(NULL);
    itp_set_parent(n2, &ep20, &on_error);
    itp_root_tp n3 = itp_create_root(NULL);
    itp_set_parent(n3, &ep31, &on_error);
    itp_root_tp n4 = itp_create_root(&on_create_4);
    itp_set_parent(n4, &ep41, &on_error);
    itp_root_tp n5 = itp_create_root(NULL);
    itp_set_parent(n5, &ep54, &on_error);
    itp_root_tp n6 = itp_create_root(NULL);
    itp_set_parent(n6, &ep60, &on_error);
    itp_root_tp n7 = itp_create_root(NULL);
    itp_set_parent(n7, &ep76, &on_error);
    ep14.use_checksum = 1;
    ep41.use_checksum = 1;
    //
    for (uint32_t i = 0; i < 1; i++) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        itp_poll_root(n5, time);
        itp_poll_root(n6, time);
        itp_poll_root(n7, time);
        sys_sleep_for(3);
    }
    //
    printf("Initialization done.\r\n");
    //goto FREE_MEMORY;
    //
    uint8_t a_n0 = 0xFF;
    n0->address = a_n0;
    uint8_t a_n1 = itp_get_free_address(n0, NULL);
    itp_connect_local_node(n0, sys_clock_ms(), &ep01, a_n1, &on_connect);
    uint8_t a_n2 = itp_get_free_address(n0, NULL);
    itp_connect_local_node(n0, sys_clock_ms(), &ep02, a_n2, &on_connect);
    //
    for (uint32_t i = 0; i < 30; i++) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        //printf("Poll (2) %i done\r\n", i);
        sys_sleep_for(3);
    }
    //
    printf("Stage 1 done.\r\n");
    //goto FREE_MEMORY;
    //
    uint8_t a_n3 = itp_get_free_address(n0, NULL);
    itp_connect_remote_node(n0, sys_clock_ms(), a_n1, a_n3, "3", &on_connect);
    uint8_t a_n4 = itp_get_free_address(n0, NULL);
    itp_connect_remote_node(n0, sys_clock_ms(), a_n1, a_n4, "4", &on_connect);
    //
    for (uint32_t i = 0; i < 30; i++) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        sys_sleep_for(3);
    }
    //
    printf("Stage 2 done.\r\n");
    //goto FREE_MEMORY;
    //
    uint8_t a_n5 = itp_get_free_address(n0, NULL);
    itp_connect_remote_node(n0, sys_clock_ms(), a_n4, a_n5, "5", &on_connect);
    //
    for (uint32_t i = 0; i < 30; i++) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        itp_poll_root(n5, time);
        sys_sleep_for(3);
    }
    printf("Stage 3 done.\r\n");
    //goto FREE_MEMORY;
    //
    itp_register_handler(n5, a_n0, 0x0101, &on_request_0101);
    {
        uint16_t value = 111;
        itp_frame_tp request = itp_create_frame(0x0101);
        itp_frame_write_value_2b(request, &value);
        itp_push_request(n0, request, a_n5, &on_response_0101);
    }
    for (uint32_t i = 0; i < 30; i++) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        itp_poll_root(n5, time);
        sys_sleep_for(3);
    }
    printf("Stage 4 done.\r\n");
    //goto FREE_MEMORY;
    //
    itp_trace_route(n5, 2, &on_trace_route);
    //itp_get_children_address(n0, 1, &on_trace_route);
    for (uint32_t i = 0; i < 30; i++) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        itp_poll_root(n5, time);
        sys_sleep_for(3);
    }
    printf("Stage 5 done.\r\n");
    //goto FREE_MEMORY;
    //
    itp_register_handler(n1, a_n0, 0x0102, &on_request_0102);
    {
        itp_frame_tp request = itp_create_frame(0x0102);
        itp_push_request(n0, request, a_n1, &on_response_0102);
    }
    for (uint32_t i = 0; i < 30; i++) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        itp_poll_root(n5, time);
        sys_sleep_for(3);
    }
    printf("Stage 6 done.\r\n");
    //goto FREE_MEMORY;
    //
    itp_set_connection_listener(n0, &ep06);
    itp_request_connect(n6, time, &on_connect_request);
    for (uint32_t i = 0; i < 30; i++) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        itp_poll_root(n5, time);
        itp_poll_root(n6, time);
        sys_sleep_for(3);
    }
    printf("Stage 7 done.\r\n");
    //goto FREE_MEMORY;
    //
    itp_set_connection_listener(n6, &ep67);
    itp_request_connect(n7, time, &on_connect_request);
    for (uint32_t i = 0; i < 40; i++) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        itp_poll_root(n5, time);
        itp_poll_root(n6, time);
        itp_poll_root(n7, time);
        sys_sleep_for(3);
    }
    printf("Stage 8 done.\r\n");
    //goto FREE_MEMORY;
    //
    itp_trace_route(n1, 7, &on_trace_route);
    for (uint32_t i = 0; i < 40; i++) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        itp_poll_root(n5, time);
        itp_poll_root(n6, time);
        itp_poll_root(n7, time);
        sys_sleep_for(3);
    }
    printf("Stage 9 done.\r\n");
    //goto FREE_MEMORY;
    //
    sys_init_serial_port(&serial);
    n7->on_create = &on_create_7;
    itp_connect_remote_endpoint(n0, time, 7, "COM6", &on_connect_endpoint);
    for (uint32_t i = 0; i < 40; i++) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        itp_poll_root(n5, time);
        itp_poll_root(n6, time);
        itp_poll_root(n7, time);
        sys_sleep_for(3);
    }
    printf("Stage 10 done.\r\n");
    goto FREE_MEMORY;
    //
    for (;remote;) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        itp_poll_root(n5, time);
        itp_poll_root(n6, time);
        itp_poll_root(n7, time);
        {
            itp_byte_t data[0x100];
            itp_size_t bytes = remote->read(remote->object, data, 0x100);
            if (bytes > 0) {
                //printf("Read %d bytes\r\n", bytes);
                remote->write(remote->object, data, bytes);
            }
        }
        sys_sleep_for(3);
    }
    goto FREE_MEMORY;
    //
    FREE_MEMORY:
    //
    printf("n0 root is %d\r\n", (int)n0->root);
    printf("n1 root is %d\r\n", (int)n1->root);
    printf("n2 root is %d\r\n", (int)n2->root);
    printf("n3 root is %d\r\n", (int)n3->root);
    printf("n4 root is %d\r\n", (int)n4->root);
    printf("n5 root is %d\r\n", (int)n5->root);
    printf("n6 root is %d\r\n", (int)n6->root);
    printf("n7 root is %d\r\n", (int)n7->root);
    //
    itp_free_root(n0);
    itp_free_root(n1);
    itp_free_root(n2);
    itp_free_root(n3);
    itp_free_root(n4);
    itp_free_root(n5);
    itp_free_root(n6);
    itp_free_root(n7);
    if (remote) itp_free(remote);
    printf("Free memory done.\r\n");
    return 0;
}

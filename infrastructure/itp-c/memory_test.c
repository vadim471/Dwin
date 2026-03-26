#include <itp/itp.h>
#include <itp/tunnel.h>
#include <initp/system/time.h>

#include <stdio.h>

void init_memory(void);
void trace_memory(void);

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

void on_connect(itp_root_tp root, uint8_t address, uint16_t error) {
    if (error) {
        printf("Failed to connect node %d, error %d - %s\r\n", address, error, itp_get_error_description(error));
    } else printf("Node with address %d connected\r\n", address);
}

uint16_t on_request_0101(itp_root_tp root, itp_frame_tp frame) {
    uint8_t* array;
    uint8_t length;
    if (itp_frame_read_array_1b(frame, (void**)&array, &length)) {
        return ITP_ERR_BAD_DATA;
    }
    printf(">>> Node %d received %d size array (order %d)\r\n", root->address, length, (frame->order & 0xFF));
    itp_frame_tp response = itp_create_frame(0x0101);
    itp_frame_write_array_1b(response, array, length);
    itp_free(array);
    itp_push_response(root, frame, response, NULL);
    return ITP_ERR_NONE;
}

uint32_t total_success = 0;
uint32_t total_errors = 0;

void on_response_from_5(itp_root_tp root, uint16_t error, itp_frame_tp frame) {
    if (!error) {
        printf(">>> Received result %d (order %d) from node 5\r\n", error, (frame->order & 0xFF));
        total_success++;
        uint8_t* array;
        uint8_t length;
        itp_frame_read_array_1b(frame, (void**)&array, &length);
        printf(">>> Received %d size array\r\n", length);
        itp_free(array);
    } else {
        printf(">>> Received result %d from node 5\r\n", error);
        total_errors++;
    }
}

int main(int argc, char** argv) {
    itp_time_t time;
    init_memory();
    itp_init_tunnel(&t01, &ep01, &ep10);
    itp_init_tunnel(&t02, &ep02, &ep20);
    itp_init_tunnel(&t13, &ep13, &ep31);
    itp_init_tunnel(&t14, &ep14, &ep41);
    itp_init_tunnel(&t45, &ep45, &ep54);
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
    printf("==========================================================\r\n");
    //
    itp_register_handler(n5, a_n0, 0x0101, &on_request_0101);
    #ifndef TEST_DATA_SIZE
    #define TEST_DATA_SIZE 30
    #endif // TEST_DATA_SIZE
    uint8_t data[TEST_DATA_SIZE];
    #define SKIP_POLL 2
    for (size_t i = 0; i < TEST_FRAMES_COUNT * SKIP_POLL; i++) {
        if (!(i % SKIP_POLL)) {
            itp_frame_tp frame = itp_create_frame(0x0101);
            itp_frame_write_array_1b(frame, data, TEST_DATA_SIZE);
            if (itp_push_request(n0, frame, 5, &on_response_from_5)) {
                itp_free_frame(frame);
                total_errors++;
            }
        }
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        itp_poll_root(n5, time);
        sys_sleep_for(1);
    }
    //
    for (uint32_t i = 0; i < 100; i++) {
        time = sys_clock_ms();
        itp_poll_root(n0, time);
        itp_poll_root(n1, time);
        itp_poll_root(n2, time);
        itp_poll_root(n3, time);
        itp_poll_root(n4, time);
        itp_poll_root(n5, time);
        sys_sleep_for(1);
    }
    printf("Stage 4 done.\r\n");
    //goto FREE_MEMORY;
    //
    FREE_MEMORY:
    itp_free_root(n0);
    itp_free_root(n1);
    itp_free_root(n2);
    itp_free_root(n3);
    itp_free_root(n4);
    itp_free_root(n5);
    printf("Free memory done.\r\n");
    printf("==================\r\n");
    printf("Total success requests: %d\r\n", total_success);
    printf("Total failed requests: %d\r\n", total_errors);
    printf("==================\r\n");
    trace_memory();
    return 0;
}

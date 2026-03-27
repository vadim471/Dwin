#include <itp/frame.h>

#include <stdio.h>

int main(int argc, char** argv) {
    itp_frame_t frame;
    itp_init_frame(&frame, ITP_CMD_RESET);
    uint32_t data[0x200];
    uint16_t length = sizeof(data) / sizeof(data[0]);
    for (uint16_t i = 0; i < length; ++i) {
        data[i] = i;
    }
    itp_error_code_t errc = itp_frame_write_wide_array_4b(&frame, data, length);
    printf("Write result: %d\r\n", errc);
    itp_frame_prepare_to_read(&frame);
    uint16_t out_length = 0;
    uint32_t* out_data = 0;
    errc = itp_frame_read_wide_array_4b(&frame, (void**)&out_data, &out_length);
    printf("Read result: %d\r\n", errc);
    if (!errc) {
        printf("Head:");
        for (uint16_t i = 0; i < 10; ++i) {
            printf(" %d", out_data[i]);
        }
    }
    printf("\r\n");
    return 0;
}

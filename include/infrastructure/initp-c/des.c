#include <initp/tools/des.h>
#include <initp/tools/format.h>

#include <stdio.h>

int main(int argc, char** argv) {
    sys_des_context_t context;
    uint8_t key[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    uint8_t iv[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    sys_init_3des(&context, key, 16, iv, 8, 1, 1, SYS_DES_TYPE_CBC, SYS_DES_PADDING_NONE);
    uint8_t in[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    uint8_t out[0xFF];
    size_t size = sys_encrypt_3des(&context, in, 16, out, 0xFF);
    printf("Encrypted size: %d\r\n", size);
    sys_trace_array("main", (const char*)out, size);
    size = sys_decrypt_3des(&context, out, size, in, 16);
    printf("Decrypted size: %d\r\n", size);
    sys_trace_array("main", (const char*)in, size);
    sys_dispose_des(&context);
    return 0;
}

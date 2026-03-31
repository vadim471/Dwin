#include <initp/tools/aes.h>

#include <stdio.h>

int main(int argc, char** argv) {
    sys_aes_context_t context;
    sys_init_aes(&context, "KEY", 0, 12345, 1, 1);
    char in[0xFF], out[0xFF];
    sprintf(in, "test");
    size_t size = sys_encrypt_aes(&context, in, 0, out, 0xFF);
    printf("Encrypted size: %d\r\n", size);
    size = sys_decrypt_aes(&context, out, size, in, 0xFF);
    in[size] = 0;
    printf("Decrypted: %s\r\n", in);
    sys_dispose_aes(&context);
    return 0;
}

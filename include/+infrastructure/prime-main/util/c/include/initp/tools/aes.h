#ifndef INITPLUS_TOOLS_AES_H_INCLUDED
#define INITPLUS_TOOLS_AES_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

typedef struct {
    void* encrypt;
    void* decrypt;
} sys_aes_context_t, *sys_aes_context_tp;

uint8_t sys_init_aes(sys_aes_context_tp context, const char* key, size_t key_size, uint64_t salt, uint8_t encrypt, uint8_t decrypt);

size_t sys_encrypt_aes(sys_aes_context_tp context, const char* input_data, size_t input_size, char* output_data, size_t output_size);

size_t sys_decrypt_aes(sys_aes_context_tp context, const char* input_data, size_t input_size, char* output_data, size_t output_size);

void sys_dispose_aes(sys_aes_context_tp context);

#endif // INITPLUS_TOOLS_AES_H_INCLUDED

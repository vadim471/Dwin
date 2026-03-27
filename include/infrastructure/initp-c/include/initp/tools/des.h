#ifndef INITPLUS_TOOLS_DES_H_INCLUDED
#define INITPLUS_TOOLS_DES_H_INCLUDED

#include <stddef.h>
#include <stdint.h>

typedef struct {
    void* encrypt;
    void* decrypt;
} sys_des_context_t, *sys_des_context_tp;

enum sys_des_type {
    SYS_DES_TYPE_CBC = 0,
    SYS_DES_TYPE_ECB
};

enum sys_des_padding {
    SYS_DES_PADDING_NONE = 0
};

size_t sys_calc_3des_data_size(size_t input_size);

uint8_t sys_init_3des(sys_des_context_tp context, const uint8_t* key, size_t key_size, const uint8_t* iv_data, size_t iv_size, uint8_t encrypt, uint8_t decrypt, uint8_t type, uint8_t padding);

size_t sys_encrypt_3des(sys_des_context_tp context, const uint8_t* input_data, size_t input_size, uint8_t* output_data, size_t output_size);

size_t sys_decrypt_3des(sys_des_context_tp context, const uint8_t* input_data, size_t input_size, uint8_t* output_data, size_t output_size);

void sys_dispose_des(sys_des_context_tp context);

#endif // INITPLUS_TOOLS_DES_H_INCLUDED

#include <initp/tools/des.h>
#include <initp/system/debug.h>

#include <openssl/evp.h>
#include <openssl/des.h>

size_t sys_calc_3des_data_size(size_t input_size) {
    size_t tail = input_size % 8;
    return input_size + (size_t)((tail > 0) ? (8 - tail) : 0);
}

uint8_t sys_init_3des(sys_des_context_tp context, const uint8_t* key_data, size_t key_size, const uint8_t* iv_data, size_t iv_size, uint8_t encrypt, uint8_t decrypt, uint8_t type, uint8_t padding) {

    if (!context) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_init_3des", "Context is null");
        return 0;
    }

    if (!key_data || key_size != 16) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_init_3des", "Bad DES key");
        return 0;
    }
    if (!iv_data || iv_size != 8) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_init_3des", "Bad DES IV");
        return 0;
    }

    if (encrypt) {
        context->encrypt = (void*)EVP_CIPHER_CTX_new();
        if (!context->encrypt) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_init_3des", "Failed to create encoding context");
            return 0;
        }
        EVP_CIPHER_CTX_init((EVP_CIPHER_CTX*)context->encrypt);
    } else {
        context->encrypt = NULL;
    }

    if (decrypt) {
        context->decrypt = (void*)EVP_CIPHER_CTX_new();
        if (!context->decrypt) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_init_3des", "Failed to create decoding context");
            return 0;
        }
        EVP_CIPHER_CTX_init((EVP_CIPHER_CTX*)context->decrypt);
    } else {
        context->decrypt = NULL;
    }

    if (encrypt) {
        if (type == SYS_DES_TYPE_CBC) {
            EVP_EncryptInit((EVP_CIPHER_CTX*)context->encrypt, EVP_des_ede_cbc(), key_data, iv_data);
        } else if (type == SYS_DES_TYPE_ECB) {
            EVP_EncryptInit((EVP_CIPHER_CTX*)context->encrypt, EVP_des_ede_ecb(), key_data, iv_data);
        } else return 0;
        if (padding == SYS_DES_PADDING_NONE) {
            EVP_CIPHER_CTX_set_padding((EVP_CIPHER_CTX*)context->encrypt, 0);
        } else return 0;
    }
    if (decrypt) {
        if (type == SYS_DES_TYPE_CBC) {
            EVP_DecryptInit((EVP_CIPHER_CTX*)context->decrypt, EVP_des_ede_cbc(), key_data, iv_data);
        } else if (type == SYS_DES_TYPE_ECB) {
            EVP_DecryptInit((EVP_CIPHER_CTX*)context->decrypt, EVP_des_ede_ecb(), key_data, iv_data);
        } else return 0;
        if (padding == SYS_DES_PADDING_NONE) {
            EVP_CIPHER_CTX_set_padding((EVP_CIPHER_CTX*)context->decrypt, 0);
        } else return 0;
    }
    return 1;
}

size_t sys_encrypt_3des(sys_des_context_tp context, const uint8_t* input_data, size_t input_size, uint8_t* output_data, size_t output_size) {

    if (!context || !context->encrypt) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_encrypt_3des", "Context is null");
        return 0;
    }

    if (!input_data || !input_size) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_encrypt_3des", "Bad input data");
        return 0;
    }

    if (!output_data || !output_size) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_encrypt_3des", "Bad output buffer");
        return 0;
    }

    /* max output_data len for a n bytes of input_data is n + n % 8 bytes */
    int c_len = sys_calc_3des_data_size(input_size), f_len = 0;
    if (output_size < (size_t)c_len) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_encrypt_3des", "Insufficient output buffer, expected at less %d bytes", c_len);
    }

    /* allows reusing of encrypt context for multiple encryption cycles */
    EVP_EncryptInit_ex((EVP_CIPHER_CTX*)context->encrypt, NULL, NULL, NULL, NULL);

    /* update output_data, c_len is filled with the length of output_data generated,
     * input_size is the size of input_data in bytes */
    EVP_EncryptUpdate((EVP_CIPHER_CTX*)context->encrypt, output_data, &c_len, input_data, input_size);

    /* update output_data with the final remaining bytes */
    EVP_EncryptFinal((EVP_CIPHER_CTX*)context->encrypt, output_data + c_len, &f_len);

    return c_len + f_len;
}

size_t sys_decrypt_3des(sys_des_context_tp context, const uint8_t* input_data, size_t input_size, uint8_t* output_data, size_t output_size) {

    if (!context || !context->decrypt) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_decrypt_3des", "Context is null");
        return 0;
    }

    if (!input_data || !input_size) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_decrypt_3des", "Bad input data");
        return 0;
    }

    if (!output_data || !output_size) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_decrypt_3des", "Bad output buffer");
        return 0;
    }

    /* output_data will always be equal to or lesser than length of input_data */
    if (output_size < input_size) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_decrypt_3des", "Insufficient output buffer, expected at less %d bytes", input_size);
    }
    int p_len = input_size, f_len = 0;

    EVP_DecryptInit_ex((EVP_CIPHER_CTX*)context->decrypt, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate((EVP_CIPHER_CTX*)context->decrypt, output_data, &p_len, input_data, input_size);
    EVP_DecryptFinal((EVP_CIPHER_CTX*)context->decrypt, output_data + p_len, &f_len);

    return p_len + f_len;
}

void sys_dispose_des(sys_des_context_tp context) {
    if (!context) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_dispose_des", "Context is null");
        return;
    }
    if (context->encrypt) {
        EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)context->encrypt);
    }
    if (context->decrypt) {
        EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)context->decrypt);
    }
}

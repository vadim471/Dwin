#include <initp/tools/aes.h>
#include <initp/system/debug.h>

#include <openssl/evp.h>
#include <openssl/aes.h>

#include <string.h>

uint8_t sys_init_aes(sys_aes_context_tp context, const char* key_data, size_t key_size, uint64_t salt, uint8_t encrypt, uint8_t decrypt) {

    if (!context) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_init_aes", "Context is null");
        return 0;
    }

    if (!key_size) {
        key_size = strlen(key_data);
    }
    if (!key_size) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_init_aes", "Bad AES key");
        return 0;
    }

    if (encrypt) {
        context->encrypt = (void*)EVP_CIPHER_CTX_new();
        if (!context->encrypt) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_init_aes", "Failed to create encoding context");
            return 0;
        }
        EVP_CIPHER_CTX_init((EVP_CIPHER_CTX*)context->encrypt);
    } else {
        context->encrypt = NULL;
    }

    if (decrypt) {
        context->decrypt = (void*)EVP_CIPHER_CTX_new();
        if (!context->decrypt) {
            sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_init_aes", "Failed to create decoding context");
            return 0;
        }
        EVP_CIPHER_CTX_init((EVP_CIPHER_CTX*)context->decrypt);
    } else {
        context->decrypt = NULL;
    }

    const int nrounds = 5;
    unsigned char key[32], iv[32];

    /*
     * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
     * nrounds is the number of times the we hash the material. More rounds are more secure but
     * slower.
     */
    int i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), (unsigned char*)&salt, (unsigned char*)key_data, key_size, nrounds, key, iv);
    if (i != 32) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_init_aes", "Key size is %d bits - should be 256 bits", i);
        return 0;
    }

    if (encrypt) EVP_EncryptInit_ex((EVP_CIPHER_CTX*)context->encrypt, EVP_aes_256_cbc(), NULL, key, iv);
    if (decrypt) EVP_DecryptInit_ex((EVP_CIPHER_CTX*)context->decrypt, EVP_aes_256_cbc(), NULL, key, iv);
    return 1;
}

size_t sys_encrypt_aes(sys_aes_context_tp context, const char* input_data, size_t input_size, char* output_data, size_t output_size) {

    if (!context || !context->encrypt) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_encrypt_aes", "Context is null");
        return 0;
    }

    if (!input_data) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_encrypt_aes", "Bad input data");
        return 0;
    }

    if (!output_data || !output_size) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_encrypt_aes", "Bad output buffer");
        return 0;
    }

    if (!input_size) {
        input_size = strlen(input_data);
        if (!input_size) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_encrypt_aes", "Failed to get length of input data");
            return 0;
        }
    }

    /* max output_data len for a n bytes of input_data is n + AES_BLOCK_SIZE - 1 bytes */
    int c_len = input_size + AES_BLOCK_SIZE, f_len = 0;
    if (output_size < (size_t)c_len) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_encrypt_aes", "Insufficient output buffer, expected at less %d bytes", c_len);
    }

    /* allows reusing of encrypt context for multiple encryption cycles */
    EVP_EncryptInit_ex((EVP_CIPHER_CTX*)context->encrypt, NULL, NULL, NULL, NULL);

    /* update output_data, c_len is filled with the length of output_data generated,
     * input_size is the size of input_data in bytes */
    EVP_EncryptUpdate((EVP_CIPHER_CTX*)context->encrypt, (unsigned char*)output_data, &c_len, (const unsigned char*)input_data, input_size);

    /* update output_data with the final remaining bytes */
    EVP_EncryptFinal_ex((EVP_CIPHER_CTX*)context->encrypt, (unsigned char*)(output_data + c_len), &f_len);

    return c_len + f_len;
}

size_t sys_decrypt_aes(sys_aes_context_tp context, const char* input_data, size_t input_size, char* output_data, size_t output_size) {

    if (!context || !context->decrypt) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_decrypt_aes", "Context is null");
        return 0;
    }

    if (!input_data) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_decrypt_aes", "Bad input data");
        return 0;
    }

    if (!output_data || !output_size) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_decrypt_aes", "Bad output buffer");
        return 0;
    }

    if (!input_size) {
        input_size = strlen(input_data);
        if (!input_size) {
            sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_decrypt_aes", "Failed to get length of input data");
            return 0;
        }
    }

    /* output_data will always be equal to or lesser than length of input_data */
    if (output_size < input_size) {
        sys_debug_print(SYSTEM_LEVEL_ERROR, "sys_decrypt_aes", "Insufficient output buffer, expected at less %d bytes", input_size);
    }
    int p_len = input_size, f_len = 0;

    EVP_DecryptInit_ex((EVP_CIPHER_CTX*)context->decrypt, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate((EVP_CIPHER_CTX*)context->decrypt, (unsigned char*)output_data, &p_len, (unsigned char*)input_data, input_size);
    EVP_DecryptFinal_ex((EVP_CIPHER_CTX*)context->decrypt, (unsigned char*)output_data + p_len, &f_len);

    return p_len + f_len;
}

void sys_dispose_aes(sys_aes_context_tp context) {
    if (!context) {
        sys_debug_print(SYSTEM_LEVEL_FATAL, "sys_dispose_aes", "Context is null");
        return;
    }
    if (context->encrypt) {
        EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)context->encrypt);
    }
    if (context->decrypt) {
        EVP_CIPHER_CTX_free((EVP_CIPHER_CTX*)context->decrypt);
    }
}

#include <stdio.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

#include "openssl_sign_engine_ca.h"

static void Print_Buffer(unsigned char* buf, unsigned int len)
{
    unsigned int index = 0U;
    for (index = 0U; index < len; index++) {
        if(index < 15U) {
        }
        else if(0U == index%16U) {
            printf("\n");
        }
        else {
        }

        printf("0x%02x, ", (buf[index] & 0xFFU));

    }
    printf("\n\n");
}


int main(int argc, char** argv)
{

    if ((argc > 1) || !argv) {
        printf("should not have arguments\n");
        return (-1);
    }

    ENGINE *openssl_sign_engine = NULL;
    int ret;
    unsigned char md[5] = {0x0, 0x1, 0x2, 0x3, 0x4};
    unsigned char sig[256] = {0};
    size_t md_len;
    unsigned int key_size = 2048;
    unsigned int siglen = key_size / 8;
    BIGNUM *bne = BN_new();
    FILE *file_out = NULL, *file_in = NULL;
    RSA *rsa, *rsa_pub;

    ENGINE_load_tee_sign();

    openssl_sign_engine = ENGINE_by_id("tee_sign");
    if (openssl_sign_engine == NULL) {
        printf("get engine Error\n");
        return -1;
    }
    printf("get openssl_sign engine OK.name:%s\n",ENGINE_get_name(openssl_sign_engine));

    ENGINE_register_RSA(openssl_sign_engine);
    ENGINE_set_default(openssl_sign_engine, ENGINE_METHOD_ALL);

    md_len = sizeof(md);

    BN_set_word(bne,RSA_F4);

    rsa = RSA_new_method(openssl_sign_engine);
    RSA_generate_key_ex(rsa, key_size, bne, NULL);

    //modulus = (char*)malloc(sizeof(char*) * 64);
    //modulus = BN_bn2hex(rsa->n);
    //printf("%s\n", modulus);

    file_out = fopen("openssl_sign_engine_pub_key.pem", "wb");
    PEM_write_RSAPublicKey(file_out, rsa);

    RSA_sign(NID_sha256, md, md_len, sig, &siglen, rsa);

    //printf("%d\n", siglen);
    printf("signature done, signature: \n");
    Print_Buffer(sig, siglen);

    fclose(file_out);

    file_in = fopen("openssl_sign_engine_pub_key.pem", "rb");
    rsa_pub = PEM_read_RSAPublicKey(file_in, NULL, NULL, NULL);

    RSA_verify(NID_sha256, md, md_len, sig, siglen, rsa_pub);

    fclose(file_in);

    ret = ENGINE_finish(openssl_sign_engine);
    if (!ret) {
        printf("ENGINE_finish fail.\n");
        return -1;
    }
    ret = ENGINE_free(openssl_sign_engine);
    if (!ret) {
        printf("ENGINE_free fail.\n");
        return -1;
    }


    return 0;
}


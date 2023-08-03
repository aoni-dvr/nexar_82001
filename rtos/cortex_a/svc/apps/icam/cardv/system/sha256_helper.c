#if defined(CONFIG_BUILD_FOSS_WOLFSSL)
#include <wolfssl/openssl/sha.h> 
#endif

int sha256_buffer(unsigned char *buffer, unsigned int len, unsigned char *sha256)
{
#if defined(CONFIG_BUILD_FOSS_WOLFSSL)
#define BUFFER_SIZE (2048)
    SHA256_CTX ctx;
    int times = 0, i = 0;

    if (buffer == NULL || len == 0 || sha256 == NULL) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    times = len / BUFFER_SIZE;
    if (len % BUFFER_SIZE != 0) {
        times += 1;
    }
    SHA256_Init(&ctx);
    for (i = 0; i < times; i++) {
        SHA256_Update(&ctx, buffer + (i * BUFFER_SIZE), ((len - (i * BUFFER_SIZE)) >= BUFFER_SIZE) ? BUFFER_SIZE : (len % BUFFER_SIZE));
    }
    SHA256_Final(sha256, &ctx);
    return 0;
#else
    (void)buffer;
    (void)len;
    (void)sha256;
    return -1;
#endif
}

int sha256_file(const char *filepath, unsigned char *sha256)
{
#if defined(CONFIG_BUILD_FOSS_WOLFSSL)
#define BUFFER_SIZE (2048)
    SHA256_CTX ctx;
    int bytes = 0;
    unsigned char data[BUFFER_SIZE] = {0};
    FILE *pFile = NULL;

    if (filepath == NULL || sha256 == NULL) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    pFile = fopen(filepath, "rb");
    if (pFile == NULL) {
        debug_line("cannot read %s", filepath);
        return -1;
    }

    SHA256_Init(&ctx);
    memset(data, 0,  BUFFER_SIZE);
    while ((bytes = fread(data, 1, BUFFER_SIZE, pFile)) != 0) {
        SHA256_Update(&ctx, data, bytes);        
        memset(data, 0,  BUFFER_SIZE);
    }
    fclose(pFile);
    SHA256_Final(sha256, &ctx);
    return 0;
#else
    (void)filepath;
    (void)sha256;
    return -1;
#endif
}


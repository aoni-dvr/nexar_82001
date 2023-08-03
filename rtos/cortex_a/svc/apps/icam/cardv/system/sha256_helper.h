#ifndef __SHA256_HELPER_H__
#define __SHA256_HELPER_H__

int sha256_buffer(unsigned char *buffer, unsigned int len, unsigned char *sha256);
int sha256_file(const char *filepath, unsigned char *sha256);

#endif//__SHA256_HELPER_H__


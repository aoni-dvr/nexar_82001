#include <string.h>
#include <stdlib.h>

#include "base64.h"

static const char base64Char[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char base64_decTable[128] = { 0 };
int decTable_Init = 0;

void init_decTable()
{
    int i,j;
    memset(base64_decTable, 0 , 128);
    for(j=0,i='A';i<='Z';i++)
        base64_decTable[i]=j++;
    for(i='a';i<='z';i++)
        base64_decTable[i]=j++;
    for(i='0';i<='9';i++)
        base64_decTable[i]=j++;
    base64_decTable['+']=j++;
    base64_decTable['/']=j++;
    base64_decTable['=']=j;
}

char* base64Decode(char const* origSigned, unsigned origLength)
{
    unsigned char const* orig = (unsigned char const*)origSigned; // in case any input bytes have the MSB set
    if (orig == NULL || (origLength&0x3) > 0) return NULL;

    unsigned const numOrig32BitValues = origLength >> 2;

    unsigned const numResultBytes = origLength - numOrig32BitValues; // 3/4 of original length
    char* result = (char*)malloc(numResultBytes + 1); // trailing '\0'
    memset(result, 0, numResultBytes+1);
    if (decTable_Init == 0) {
        init_decTable();
        decTable_Init = 1;
    }

    unsigned i;
    for (i = 0; i < numOrig32BitValues ; i++) {
        result[3*i]   =  (base64_decTable[orig[4*i]] << 2) & 0xfc;      //upper 6 bits of 1st byte
        result[3*i]   |= base64_decTable[orig[4*i+1]] >> 4;             //lower 2 bits of 1st byte
        result[3*i+1] =  (base64_decTable[orig[4*i+1]] << 4) & 0xf0;    //upper 4 bits of 2nd byte
        result[3*i+1] |= (base64_decTable[orig[4*i+2]] >> 2) & 0x0f;    //lower 4 bits of 2nd byte
        result[3*i+2] =  (base64_decTable[orig[4*i+2]] << 6);           //upper 2 bits of 3rd byte
        if(orig[4*i+3] != '=')
            result[3*i+2] |= base64_decTable[orig[4*i+3]];              //lower 6 bits of 3rd byte
    }

    return result;
}

char* base64Encode(char const* origSigned, unsigned origLength)
{
    unsigned char const* orig = (unsigned char const*)origSigned; // in case any input bytes have the MSB set
    if (orig == NULL) return NULL;

    unsigned const numOrig24BitValues = origLength/3;
    unsigned char havePadding = origLength > numOrig24BitValues*3;
    unsigned char havePadding2 = origLength == numOrig24BitValues*3 + 2;
    unsigned const numResultBytes = 4*(numOrig24BitValues + havePadding);
    char* result = (char*)malloc(numResultBytes+1);//new char[numResultBytes+1]; // allow for trailing '\0'
    memset(result, 0, numResultBytes+1);
    // Map each full group of 3 input bytes into 4 output base-64 characters:
    unsigned i;
    for (i = 0; i < numOrig24BitValues; ++i) {
        result[4*i+0] = base64Char[(orig[3*i]>>2)&0x3F];
        result[4*i+1] = base64Char[(((orig[3*i]&0x3)<<4) | (orig[3*i+1]>>4))&0x3F];
        result[4*i+2] = base64Char[((orig[3*i+1]<<2) | (orig[3*i+2]>>6))&0x3F];
        result[4*i+3] = base64Char[orig[3*i+2]&0x3F];
    }

    // Now, take padding into account.  (Note: i == numOrig24BitValues)
    if (havePadding) {
        result[4*i+0] = base64Char[(orig[3*i]>>2)&0x3F];
        if (havePadding2) {
            result[4*i+1] = base64Char[(((orig[3*i]&0x3)<<4) | (orig[3*i+1]>>4))&0x3F];
            result[4*i+2] = base64Char[(orig[3*i+1]<<2)&0x3F];
        } else {
            result[4*i+1] = base64Char[((orig[3*i]&0x3)<<4)&0x3F];
            result[4*i+2] = '=';
        }
        result[4*i+3] = '=';
    }

    result[numResultBytes] = '\0';
    return result;
}

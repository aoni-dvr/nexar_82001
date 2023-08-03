#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define STR_FILE_NAME_N         "N.txt"
#define STR_FILE_NAME_X         "X.txt"
#define STR_FILE_NAME_PUBKEY    "pubkey.bin"

#define UINT8  unsigned char
#define UINT32 unsigned long int

int main(int argc, const char ** argv)
{
    FILE *p_file_N = NULL, *p_file_X = NULL, *p_file_pubkey = NULL;
    char file_path_N[128] = "", file_path_X[128] = "";
    UINT8 debug_print = 0U;

    if (argc > 1U) {
        for (UINT8 i = 1U; i < argc; i++) {
            if (strcmp(argv[i], "--debug-print") == 0U) {
                debug_print = 1U;
            } else {
                if (strcmp(file_path_N, "") == 0U) {
                    strcpy(file_path_N, argv[i]);
                } else {
                    strcpy(file_path_X, argv[i]);
                }
            }
        }
    }

    if (strcmp(file_path_N, "") == 0U) {
        strcpy(file_path_N, STR_FILE_NAME_N);
    }

    if (strcmp(file_path_X, "") == 0U) {
        strcpy(file_path_X, STR_FILE_NAME_X);
    }

    printf("genpubkey, searching for files %s and %s\n", file_path_N, file_path_X);

    p_file_N = fopen(file_path_N, "r");
    p_file_X = fopen(file_path_X, "r");

    if ((p_file_N != NULL) && (p_file_X != NULL)) {
        UINT8 N[256] = {0}, X[256] = {0}, Buffer[512] = {0};
        char ReadByte[8]  = "0x00";
        char PubKeyFn[16] = "pubkey.bin";
        UINT32 Hex, Num, Idx, BufIdx;
        char *ptr;

        printf("Searched!!\n");

        printf("Parsing %s\n", file_path_N);
        Idx = sizeof(N) - 1;
        Num = fread(Buffer, sizeof(UINT8), 512, p_file_N);
        if (Num == 512U) {
            BufIdx = 0;
            while (BufIdx < (Num - 1U)) {
                ReadByte[2] = (char) Buffer[BufIdx];
                ReadByte[3] = (char) Buffer[BufIdx + 1U];
                Hex = strtoul(ReadByte, &ptr, 16);

                N[Idx] = (UINT8) (Hex & 0xFFU);
                Idx --;
                BufIdx += 2U;
            }
        }

        printf("Parsing %s\n", file_path_X);
        Idx = sizeof(X) - 1U;
        Num = fread(Buffer, sizeof(UINT8), 512U, p_file_X);
        if (Num == 512U) {
            BufIdx = 0U;
            while (BufIdx < (Num - 1U)) {
                ReadByte[2U] = (char) Buffer[BufIdx];
                ReadByte[3U] = (char) Buffer[BufIdx + 1U];
                Hex = strtoul(ReadByte, &ptr, 16);

                X[Idx] = (UINT8) (Hex & 0xFFU);
                Idx --;
                BufIdx += 2;
            }
        }

        if (debug_print > 0U) {
            printf("N = \n");
            for (UINT32 i = 0U; i < (sizeof(N) / sizeof(UINT8)); i++) {
                printf("0x%02x ", N[i]);
                if ((i % 4) == 3U) {
                    printf("\n");
                }
            }
            printf("X = \n");
            for (UINT32 i = 0U; i < (sizeof(N) / sizeof(UINT8)); i++) {
                printf("0x%02x ", X[i]);
                if ((i % 4) == 3U) {
                    printf("\n");
                }
            }
        }

        printf("Generating pubkey.bin\n");
        p_file_pubkey = fopen(STR_FILE_NAME_PUBKEY, "w");
        if (p_file_pubkey != NULL) {
            Num = fwrite(N, sizeof(UINT8), sizeof(N), p_file_pubkey);
            if (Num != sizeof(N)) {
                printf("Write N value error\n");
            }
            Num = fwrite(X, sizeof(UINT8), sizeof(X), p_file_pubkey);
            if (Num != sizeof(X)) {
                printf("Write X value error\n");
            }

            if (fclose(p_file_pubkey) != 0) {
                printf("Cannot close %s\n", STR_FILE_NAME_PUBKEY);
            }
        } else {
            printf("Cannot create %s\n", STR_FILE_NAME_PUBKEY);
        }

        if (fclose(p_file_N) != 0) {
            printf("Cannot close %s\n", file_path_N);
        }
        if (fclose(p_file_X) != 0) {
            printf("Cannot close %s\n", file_path_X);
        }
    } else {
        if (p_file_N == NULL) {
            printf("Cannot find %s\n", file_path_N);
        }
        if (p_file_X == NULL) {
            printf("Cannot find %s\n", file_path_X);
        }
    }

    printf("Procssing Done\n");

    return 0;
}
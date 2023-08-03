#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define msg(str, args...)       do{ fprintf(stderr, str, ## args); usleep(15000);}while(0)

int main (int argc, char *argv[])
{
    unsigned char   *map_base, pattern[64];
    int             i, fd;
    unsigned long   size, p_addr;
    unsigned char   content;

    if (argc < 3) {
        msg("Usage: %s <hex:phy_addr> <hex:size>\n"
            "\t e.g. %s 3d400000 20000000\n"
            "\n", argv[0], argv[0]);
        return -1;
    }

    p_addr = strtoul(argv[1], NULL, 16);
    size   = strtoul(argv[2], NULL, 16);

    msg("phy_addr = 0x%lx, size = 0x%lx\n", p_addr, size);

    fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd == -1) {
        return (-1);
    }

    map_base = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, p_addr);

    if (map_base == MAP_FAILED) {
        perror("Fail to do mmap:\n");
        return -1;
    } else {
        msg("Successfull! mmap 0x%lx to %p.\n", p_addr, map_base);
    }


    msg("\n@ do Read memory byte by byte\n");
    for (i = 0; i < 64; ++i) {
        content = map_base[i];
        msg("%02x %s", (unsigned int)content, ((i & 0xF) == 0xf) ? "\n" : "");
    }
    msg("\t Read memory done\n");

    msg("\n@ do Write memory byte by byte\n");
    msg("\twrite start pointer= %p\n", map_base);
    for (i = 0; i < 64; ++i) {
        map_base[i] = i;
        msg("%02x %s", map_base[i], ((i & 0xF) == 0xf) ? "\n" : "");
    }
    msg("\t Write memory done\n");

    msg("\n\n@ do memset (0xaa, size= 24)\n");
    msg("\tstart pointer= %p\n", map_base);
    memset(map_base, 0xaa, 24);
    msg("\t memset done\n");


    msg("\n\n@ do memcpy (size= 64)\n");
    msg("\tstart pointer= %p\n", map_base);
    memset(pattern, 0xcc, 64);
    memcpy(map_base, pattern, 64);
    msg("\t memcpy done\n");

    close(fd);
    munmap(map_base, size);
    msg("\n@ munmap and leave... \n");
    return (0);
}

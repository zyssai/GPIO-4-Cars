#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>

#define	GPIO_REG_MAP            0xFF634000
#define GPIOX_FSEL_REG_OFFSET   0x116
#define GPIOX_OUTP_REG_OFFSET   0x117
#define GPIOX_INP_REG_OFFSET    0x118
#define PAD_PULL_UP_REG5_OFFSET 0x13F
#define BLOCK_SIZE              (4*1024)
static volatile uint32_t *gpio;

int main(int argc, char **argv) {
    int fd;

    if ((fd = open("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0) {
        printf("Unable to open /dev/gpiomem\n");
        return -1;
    }

    gpio = mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIO_REG_MAP);
    if (gpio < 0) {
        printf("Mmap failed.\n");
        return -1;
    }

    *(gpio + (PAD_PULL_UP_REG5_OFFSET)) |= (1 << 4);
    *(gpio + (PAD_PULL_UP_REG5_OFFSET)) |= (1 << 12);
    return 0;
}

#include "drivers/ata.h"
#include "arch/io.h"

#define ATA_PRIMARY_DATA         0x1F0
#define ATA_PRIMARY_ERR          0x1F1
#define ATA_PRIMARY_SECCOUNT     0x1F2
#define ATA_PRIMARY_LBA_LO       0x1F3
#define ATA_PRIMARY_LBA_MID      0x1F4
#define ATA_PRIMARY_LBA_HI       0x1F5
#define ATA_PRIMARY_DRIVE_HEAD   0x1F6
#define ATA_PRIMARY_COMM_REGSTAT 0x1F7

#define ATA_CMD_READ_PIO         0x20
#define ATA_CMD_WRITE_PIO        0x30
#define ATA_STATUS_BSY           0x80
#define ATA_STATUS_DRQ           0x08

static void ata_wait_ready(void) {
    while (inb(ATA_PRIMARY_COMM_REGSTAT) & ATA_STATUS_BSY);
}

void ata_init(void) {
    // Primary ATA channel initialization
}

int ata_read_sectors(uint32_t lba, uint8_t sector_count, void *buffer) {
    ata_wait_ready();
    outb(ATA_PRIMARY_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, sector_count);
    outb(ATA_PRIMARY_LBA_LO, (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HI, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_COMM_REGSTAT, ATA_CMD_READ_PIO);

    uint16_t *buf = (uint16_t *)buffer;
    for (int s = 0; s < sector_count; s++) {
        ata_wait_ready();
        while (!(inb(ATA_PRIMARY_COMM_REGSTAT) & ATA_STATUS_DRQ));
        for (int i = 0; i < 256; i++) {
            *buf++ = inw(ATA_PRIMARY_DATA);
        }
    }
    return 0;
}

int ata_write_sectors(uint32_t lba, uint8_t sector_count, const void *buffer) {
    ata_wait_ready();
    outb(ATA_PRIMARY_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, sector_count);
    outb(ATA_PRIMARY_LBA_LO, (uint8_t)lba);
    outb(ATA_PRIMARY_LBA_MID, (uint8_t)(lba >> 8));
    outb(ATA_PRIMARY_LBA_HI, (uint8_t)(lba >> 16));
    outb(ATA_PRIMARY_COMM_REGSTAT, ATA_CMD_WRITE_PIO);

    const uint16_t *buf = (const uint16_t *)buffer;
    for (int s = 0; s < sector_count; s++) {
        ata_wait_ready();
        while (!(inb(ATA_PRIMARY_COMM_REGSTAT) & ATA_STATUS_DRQ));
        for (int i = 0; i < 256; i++) {
            outw(ATA_PRIMARY_DATA, *buf++);
        }
    }
    return 0;
}

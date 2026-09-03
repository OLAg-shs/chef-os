#ifndef CHEF_ATA_H
#define CHEF_ATA_H

#include "types.h"

void ata_init(void);
int ata_read_sectors(uint32_t lba, uint8_t sector_count, void *buffer);
int ata_write_sectors(uint32_t lba, uint8_t sector_count, const void *buffer);

#endif

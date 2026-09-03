#include "drivers/pci.h"
#include "arch/io.h"
#include "kprintf.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

static pci_device_t s_pci_devices[64];
static size_t s_pci_device_count = 0;

static uint32_t pci_read_dword(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    uint32_t address = ((uint32_t)1 << 31)
                     | ((uint32_t)bus << 16)
                     | ((uint32_t)dev << 11)
                     | ((uint32_t)func << 8)
                     | (offset & 0xFC);
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

static uint16_t pci_read_word(uint8_t bus, uint8_t dev, uint8_t func, uint8_t offset) {
    uint32_t val = pci_read_dword(bus, dev, func, offset);
    return (uint16_t)((val >> ((offset & 2) * 8)) & 0xFFFF);
}

void pci_init(void) {
    s_pci_device_count = 0;

    for (uint16_t bus = 0; bus < 256; bus++) {
        for (uint8_t dev = 0; dev < 32; dev++) {
            uint16_t vendor = pci_read_word(bus, dev, 0, 0x00);
            if (vendor == 0xFFFF || vendor == 0x0000) continue;

            uint16_t device_id = pci_read_word(bus, dev, 0, 0x02);
            uint32_t class_rev = pci_read_dword(bus, dev, 0, 0x08);
            uint8_t class_code = (class_rev >> 24) & 0xFF;
            uint8_t subclass   = (class_rev >> 16) & 0xFF;
            uint8_t prog_if    = (class_rev >> 8) & 0xFF;
            uint32_t bar0      = pci_read_dword(bus, dev, 0, 0x10);

            if (s_pci_device_count < 64) {
                pci_device_t *d = &s_pci_devices[s_pci_device_count++];
                d->bus = bus;
                d->device = dev;
                d->function = 0;
                d->vendor_id = vendor;
                d->device_id = device_id;
                d->class_code = class_code;
                d->subclass = subclass;
                d->prog_if = prog_if;
                d->bar0 = bar0;
            }
        }
    }
}

pci_device_t *pci_find_device(uint16_t vendor_id, uint16_t device_id) {
    for (size_t i = 0; i < s_pci_device_count; i++) {
        if (s_pci_devices[i].vendor_id == vendor_id && s_pci_devices[i].device_id == device_id) {
            return &s_pci_devices[i];
        }
    }
    return NULL;
}

pci_device_t *pci_find_class(uint8_t class_code, uint8_t subclass) {
    for (size_t i = 0; i < s_pci_device_count; i++) {
        if (s_pci_devices[i].class_code == class_code && s_pci_devices[i].subclass == subclass) {
            return &s_pci_devices[i];
        }
    }
    return NULL;
}

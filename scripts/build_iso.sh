#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

ISO_ROOT="$ROOT_DIR/build/iso_root"
DIST_DIR="$ROOT_DIR/dist"
ISO_PATH="$DIST_DIR/chef-os.iso"

echo "=== Building Chef OS Bootable UEFI/BIOS ISO ==="

# 1. Clean & create iso_root
rm -rf "$ISO_ROOT"
mkdir -p "$ISO_ROOT/boot" "$ISO_ROOT/EFI/BOOT" "$DIST_DIR"

# 2. Copy Kernel ELF & Limine config
cp "$ROOT_DIR/build/chef-kernel.elf" "$ISO_ROOT/boot/chef-kernel.elf"
cp "$ROOT_DIR/boot/limine.conf" "$ISO_ROOT/boot/limine.conf"

# 3. Copy Limine Bootloader binaries
cp /usr/share/limine/BOOTX64.EFI "$ISO_ROOT/EFI/BOOT/BOOTX64.EFI"
cp /usr/share/limine/limine-bios-cd.bin "$ISO_ROOT/boot/"
cp /usr/share/limine/limine-bios.sys "$ISO_ROOT/boot/"
cp /usr/share/limine/limine-uefi-cd.bin "$ISO_ROOT/boot/"

# 4. Create EFI partition image using mtools
EFI_IMG="$ROOT_DIR/build/efi.img"
dd if=/dev/zero of="$EFI_IMG" bs=512 count=8192 status=none
mformat -i "$EFI_IMG" -F ::
mmd -i "$EFI_IMG" ::/EFI ::/EFI/BOOT
mcopy -i "$EFI_IMG" /usr/share/limine/BOOTX64.EFI ::/EFI/BOOT/BOOTX64.EFI
cp "$EFI_IMG" "$ISO_ROOT/boot/efi.img"

# 5. Generate Hybrid ISO with xorriso
xorriso -as mkisofs -R -r -J \
    -b boot/limine-bios-cd.bin \
    -no-emul-boot -boot-load-size 4 -boot-info-table \
    --efi-boot boot/limine-uefi-cd.bin \
    -efi-boot-part --efi-boot-image --protective-msdos-label \
    "$ISO_ROOT" -o "$ISO_PATH"

echo "=== Chef OS ISO Created: $ISO_PATH ==="

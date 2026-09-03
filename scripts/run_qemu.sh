#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
ISO_PATH="$ROOT_DIR/dist/chef-os.iso"

OVMF_PATH="/usr/share/edk2/x64/OVMF_CODE.4m.fd"
if [ ! -f "$OVMF_PATH" ]; then
    OVMF_PATH="/usr/share/ovmf/x64/OVMF_CODE.4m.fd"
fi

echo "=== Launching Chef OS in QEMU (UEFI GOP Mode) ==="
echo "OVMF Firmware: $OVMF_PATH"
echo "ISO Image: $ISO_PATH"
echo "Serial output redirected to stdio."
echo ""

qemu-system-x86_64 \
    -m 2G \
    -smp 2 \
    -drive if=pflash,format=raw,readonly=on,file="$OVMF_PATH" \
    -cdrom "$ISO_PATH" \
    -serial stdio \
    -vga std \
    -display none \
    -device isa-debug-exit,iobase=0xf4,iosize=0x04 \
    "$@" || true

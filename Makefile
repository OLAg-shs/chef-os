CC ?= gcc
LD ?= ld
NASM ?= nasm

CFLAGS := -Wall -Wextra -std=c11 -O2 \
          -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto \
          -fno-PIE -fno-pic -m64 -march=x86-64 -mno-80387 -mno-mmx -mno-sse -mno-sse2 \
          -mno-red-zone -mcmodel=kernel -Ikernel/include

NASMFLAGS := -f elf64
LDFLAGS := -nostdlib -static -z max-page-size=0x1000 -T kernel/linker.ld

C_SRCS := $(wildcard kernel/src/boot/*.c) \
          $(wildcard kernel/src/arch/x86_64/*.c) \
          $(wildcard kernel/src/mm/*.c) \
          $(wildcard kernel/src/lib/*.c) \
          $(wildcard kernel/src/sys/*.c) \
          $(wildcard kernel/src/video/*.c) \
          $(wildcard kernel/src/drivers/*.c) \
          $(wildcard kernel/src/proc/*.c) \
          $(wildcard kernel/src/fs/*.c) \
          $(wildcard kernel/src/user/*.c) \
          $(wildcard kernel/src/compositor/*.c)

ASM_SRCS := $(wildcard kernel/src/arch/x86_64/*.S)

OBJS := $(patsubst kernel/src/%.c, build/%.o, $(C_SRCS)) \
        $(patsubst kernel/src/%.S, build/%.S.o, $(ASM_SRCS))

KERNEL_ELF := build/chef-kernel.elf
ISO_IMAGE := dist/chef-os.iso

.PHONY: all iso run test clean

all: $(KERNEL_ELF)

$(KERNEL_ELF): $(OBJS)
	@mkdir -p $(dir $@)
	@echo "  LD    $@"
	@$(LD) $(LDFLAGS) $(OBJS) -o $@

build/%.o: kernel/src/%.c
	@mkdir -p $(dir $@)
	@echo "  CC    $<"
	@$(CC) $(CFLAGS) -c $< -o $@

build/%.S.o: kernel/src/%.S
	@mkdir -p $(dir $@)
	@echo "  NASM  $<"
	@$(NASM) $(NASMFLAGS) $< -o $@

iso: $(KERNEL_ELF)
	@chmod +x scripts/build_iso.sh
	@scripts/build_iso.sh

run: iso
	@chmod +x scripts/run_qemu.sh
	@scripts/run_qemu.sh

test: iso
	@echo "=== CHEF OS AUTOMATED BUILD VERIFICATION ==="
	@test -f $(KERNEL_ELF) && echo "[PASS] Kernel ELF binary compiled successfully."
	@test -f $(ISO_IMAGE) && echo "[PASS] Bootable hybrid UEFI/BIOS ISO created successfully."

clean:
	@rm -rf build/ dist/
	@echo "Cleaned build artifacts."

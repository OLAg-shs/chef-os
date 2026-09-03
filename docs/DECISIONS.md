# Chef OS Architecture Decision Records (ADRs)

## ADR-0001: Strict Project Separation & From-Scratch Mandate
- **Date:** 2026-09-03
- **Status:** ACCEPTED
- **Context:** Historical Chef OS V7 was a theme overlay on Arch Linux. The new project must be an independent, self-contained operating system.
- **Decision:** Separate repository (`chef-os`), fresh git history, zero shared code lineage with `Chef_Carthy`.

---

## ADR-0002: Target Architecture: x86_64 with UEFI Boot
- **Date:** 2026-09-03
- **Status:** ACCEPTED
- **Context:** Need standard 64-bit PC architecture support for virtual machines (VMware, QEMU) and modern bare-metal x86_64 systems.
- **Decision:** Target x86_64 Long Mode exclusively. Boot via modern UEFI GOP firmware interface.

---

## ADR-0003: Boot Protocol Selection: Limine v2 Boot Protocol
- **Date:** 2026-09-03
- **Status:** ACCEPTED
- **Context:** Evaluated bootloaders: Custom EFI stub, Multiboot2, Stivale2, Limine.
- **Rationale:** Limine provides an open, modern, clean Higher-Half 64-bit boot protocol with out-of-the-box support for UEFI GOP framebuffers, ACPI RSDP discovery, physical memory maps, and kernel higher-half loading without requiring complex 16-bit real mode / 32-bit protected mode trampoline assembly.
- **Consequences:** Clean 64-bit kernel entry point immediately in high-memory with hardware memory map already mapped.

---

## ADR-0004: Language & Toolchain: Freestanding C11 + NASM Assembly
- **Date:** 2026-09-03
- **Status:** ACCEPTED
- **Context:** Language choice for kernel core, drivers, and userland.
- **Decision:** Modern freestanding C (C11 with `-ffreestanding -fno-stack-protector -fno-pic -mno-red-zone -mcmodel=kernel`) combined with NASM for architecture-specific assembly routines.
- **Rationale:** Deterministic compilation, zero runtime overhead, direct memory layout control, universal toolchain availability.

---

## ADR-0005: Memory Management: Bitmap PMM + 4-Level Paging VMM + Slab Heap
- **Date:** 2026-09-03
- **Status:** ACCEPTED
- **Decision:**
  - PMM: Bitmap allocator tracking 4KB pages.
  - VMM: 4-Level Paging (PML4, PDPT, PD, PT) with HHDM physical direct mapping.
  - Heap: Dynamic slab allocator for fixed structures + freelist for variable `kmalloc`/`kfree`.

---

## ADR-0006: Syscall Architecture: System V AMD64 Syscall Convention
- **Date:** 2026-09-03
- **Status:** ACCEPTED
- **Decision:** Implement x86_64 `syscall`/`sysret` with `RAX` for syscall number and `RDI`, `RSI`, `RDX`, `R10`, `R8`, `R9` for arguments.
- **Rationale:** Native hardware-accelerated user/kernel transition without legacy `int 0x80` overhead.

---

## ADR-0007: Compositor & UI Shell: Native Double-Buffered Surface Server
- **Date:** 2026-09-03
- **Status:** ACCEPTED
- **Decision:** Build a native 2D compositing engine directly in Chef OS rather than porting bloated external desktop environments.
- **Rationale:** Ensures strict compliance with Chef Cream design tokens, eliminates reload flashes, guarantees `Super+W` vs `Ctrl+W` isolation, and enables real dynamic workspace & audio waveform integration.

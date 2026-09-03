# Chef OS Architecture Blueprint (V1.0)

## 1. System Vision & Layer Model

Chef OS is built as an independent, monolithic modular micro-core operating system designed from scratch for x86_64 platforms.

```
+=========================================================================+
|                       CHEF OS DESKTOP ENVIRONMENT                       |
|   - Left Vertical Dock (12 modules, dynamic running app indicators)     |
|   - Native Dynamic Workspaces (Main, Coding, Research, Media + runtime) |
|   - Floating Popovers (Volume, Wi-Fi, Time, Power) with Spring Easing   |
|   - Real Audio-Reactive Waveform Visualizer                             |
|   - Central Design System (Cream, Dark, Olive, Rose, Midnight)          |
+=========================================================================+
|                       COMPOSITOR & WINDOW MANAGER                       |
|   - Native Double-Buffered Surface Compositor                           |
|   - Dirty-Rectangle Damage Tracking & Alpha Blending                    |
|   - Window Lifecycle & Focus Management                                 |
|   - Application Identity (`Ctrl+W` passed through, `Super+W` window)    |
+=========================================================================+
|                        SYSTEM DAEMONS & IPC                             |
|   - PID 1 Init Engine (`system/init`)                                   |
|   - Sound Server (`system/soundd`) with PCM Circular Mixer              |
|   - Network Daemon (`system/netd`) with Interface / DNS State           |
|   - Notification Manager (`system/notifyd`)                             |
|   - Settings & Theme Synchronization Service                            |
+=========================================================================+
|                         CHEF OS USERLAND (Ring 3)                       |
|   - Native POSIX-aligned C Runtime (`lib/libchef`)                      |
|   - Interactive CLI Shell (`user/chefsh`)                               |
|   - Core Utilities (`ls`, `cat`, `ps`, `kill`, `top`, `cheffetch`)      |
+=========================================================================+
|                         CHEF OS KERNEL (Ring 0)                         |
|   - Higher-Half x86_64 Kernel (0xFFFFFFFF80000000)                      |
|   - Physical Memory Manager (Bitmap Page Allocator)                     |
|   - Virtual Memory Manager (4-Level Paging PML4 + HHDM Direct Map)      |
|   - Kernel Heap Allocator (`kmalloc` / `kfree` slab + freelist)         |
|   - Interrupts & Exceptions (IDT, GDT, TSS, ISRs, LAPIC, IOAPIC)        |
|   - Preemptive Multi-threading Scheduler & Context Switching            |
|   - Syscall ABI (`syscall` / `sysret` via System V Registers)           |
|   - Virtual Filesystem (VFS) & Initial RAMDisk (ChefRD / USTAR)         |
|   - Framebuffer Subsystem (UEFI GOP 32-bit Linear Framebuffer)          |
|   - Input Subsystem (PS/2 Keyboard & Mouse packet decoding)             |
|   - Driver Framework (PCI Enumeration, Serial COM1/2, Storage)          |
+=========================================================================+
|                         FIRMWARE & BOOTLOADER                           |
|   - UEFI 64-bit Firmware Entry via Limine Boot Protocol v2              |
|   - Hardware Memory Map & Framebuffer Hand-off                          |
+=========================================================================+
```

---

## 2. Core Subsystem Specifications

### 2.1 Boot Protocol & Memory Layout
- **Firmware Target:** UEFI 64-bit (with BIOS compatibility).
- **Boot Protocol:** Limine Boot Protocol v2 (High-Half Direct Mapping HHDM, Framebuffer GOP, Memory Map, ACPI RSDP).
- **Virtual Memory Map:**
  - `0x0000000000000000 – 0x00007FFFFFFFFFFF` (0–128 TB): Userspace address space (ELF binaries loaded at `0x400000`, user heap, user stack below `0x00007FFFFFFFF000`).
  - `0xFFFF800000000000 – 0xFFFF807FFFFFFFFF` (+0 to +128 TB): HHDM (Higher-Half Direct Map of all physical RAM).
  - `0xFFFFFFFF80000000 – 0xFFFFFFFFFFFFFFFF` (-2 GB): Kernel code, data, BSS, and kernel heap.

### 2.2 Memory Management
- **Physical Memory Manager (PMM):** High-speed bitmap allocator tracking 4KB physical page frames. Automatically initializes from UEFI memory map entries (ignoring reserved/ACPI regions and reclaiming bootloader reclaimable memory).
- **Virtual Memory Manager (VMM):** 4-level x86_64 paging (PML4 -> PDPT -> PD -> PT). Recursive mapping / HHDM-based page table manipulation. Supports 4KB standard pages and 2MB large pages.
- **Kernel Heap:** Slab allocator for fixed-size kernel structs (threads, processes, file descriptors) + freelist bucket allocator for dynamic `kmalloc`/`kfree`.

### 2.3 Interrupts & Timers
- **GDT / TSS:** 64-bit Global Descriptor Table with Kernel Code (0x08), Kernel Data (0x10), User Data (0x18), User Code (0x20), and Task State Segment (TSS at 0x28) for Interrupt Stack Table (IST) allocation.
- **IDT:** 256 interrupt descriptors with separate assembly ISR stubs preserving register state.
- **Exception Handling:** Dedicated handlers for Page Faults (#PF with CR2 decoding), General Protection Faults (#GP), and Double Faults (#DF on separate IST stack).
- **Timers:** APIC Timer calibrated against PIT/TSC for 1000Hz (1ms) preemptive scheduling ticks.

### 2.4 Syscall ABI
- **Instruction:** `syscall` / `sysretq` using IA32_STAR, IA32_LSTAR, and IA32_SFMASK MSRs.
- **Register Assignment (System V AMD64 Conforming):**
  - Syscall Number: `RAX`
  - Arguments: `RDI` (arg1), `RSI` (arg2), `RDX` (arg3), `R10` (arg4), `R8` (arg5), `R9` (arg6)
  - Return Value: `RAX` (negative values indicate `-errno`)
  - Clobbered: `RCX` (user RIP saved by CPU), `R11` (user RFLAGS saved by CPU)

### 2.5 Framebuffer & Graphics Engine
- **UEFI GOP Interface:** 32-bit linear RGBA/BGRA framebuffer with arbitrary resolutions (1920x1080 default, 1280x720 fallback).
- **Double Buffering:** Kernel and compositor maintain a dedicated backbuffer to prevent screen tearing. Only modified dirty rectangles are blitted to the hardware frontbuffer during vertical refresh.
- **2D Geometry & Typography:** Fast anti-aliased font rasterizer (8x16 bitmap console + TrueType vector engine in userland) and rounded rectangle renderer with alpha blending.

### 2.6 Native Desktop Shell & Design Tokens
- **Design Palette Tokens:**
  - Background: `#F1EBDD` (Warm Parchment Cream)
  - Surface Primary: `#F8F4EA` (Frosted Surface)
  - Surface Alt: `#E8E0CE` (Secondary Surface)
  - Text Primary: `#2B2A28` (Dark Ink)
  - Text Muted: `#6E6A5F` (Muted Ink)
  - Accent Coral: `#A6534A` (Restrained Crimson Accent)
  - Border: `#DCD3BE` (Parchment Border)
- **Primary Left Dock:** Single vertical navigation bar hosting:
  1. Chef OS Crest Glyph
  2. Workspace Pills (Dynamic `[1: Main]`, `[2: Coding]`, `[3: Research]`, `[4: Media]`, `[+]`)
  3. Pinned & Dynamic Running Application Tiles with active state indicator
  4. Audio-Reactive Waveform Visualizer (tied to PCM playback stream)
  5. System Control Tiles (Volume Popover, Wi-Fi Popover, Clock Popover, Power Menu)

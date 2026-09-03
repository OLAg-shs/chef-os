# Chef OS Architecture Specification

## 1. System Vision & Architecture Hierarchy

```
+-------------------------------------------------------------------------+
|                         CHEF OS GRAPHICAL USERLAND                      |
|  +-------------------+  +-------------------+  +---------------------+  |
|  | Left Vertical Dock|  |  App Launcher     |  | System Popovers     |  |
|  | Dynamic Workspaces|  |  Real App Icons   |  | (Audio, Net, Power) |  |
|  +-------------------+  +-------------------+  +---------------------+  |
|  +-------------------------------------------------------------------+  |
|  |      Central Design System (Cream, Dark, Olive, Rose, Midnight)    |  |
|  +-------------------------------------------------------------------+  |
+-------------------------------------------------------------------------+
|                     WINDOW & COMPOSITION SERVICES                       |
|  - Surface Manager & Double-Buffered Compositor                         |
|  - Real Window Lifecycle, Focus, Stacking & Dynamic App Registration    |
|  - Window Close (`Super+W`) vs Application Tab Close (`Ctrl+W`)          |
+-------------------------------------------------------------------------+
|                     SYSTEM SERVICES & IPC SUBSYSTEMS                    |
|  - PID 1 Init & Service Lifecycle Engine                                |
|  - Audio Service (PCM Mixer, Volume IPC, Real Playback Stream Telemetry)|
|  - Network Service (Interface Discovery, IP/DNS Configuration)          |
|  - Notification Daemon & Power Management Service                       |
+-------------------------------------------------------------------------+
|                         CHEF OS KERNEL & ABI                            |
|  - 64-bit Kernel Core (x86_64 Long Mode, IDT, Syscall ABI)             |
|  - Memory Subsystem (Physical Bitmap/Buddy Allocator, 4-Level Paging)   |
|  - Preemptive Thread Scheduler & Synchronization Primitives             |
|  - Virtual Filesystem (VFS) & Storage Drivers (AHCI / NVMe / RAMDisk)   |
|  - Graphics Display Abstraction (UEFI GOP Framebuffer)                  |
+-------------------------------------------------------------------------+
|                     BOOTLOADER & HARDWARE LAYER                         |
|  - UEFI x86_64 Stage-1 Bootloader -> Kernel Entry Protocol              |
|  - Memory Map Hand-off & ACPI / PCI Discovery                           |
+-------------------------------------------------------------------------+
```

---

## 2. Subsystem Roadmaps

1. **Bootloader (`boot/`):** UEFI Application querying GOP display mode and memory map, loading ELF kernel binary into high memory.
2. **Kernel Core (`kernel/`):** Monolithic modular micro-core with clean C/Assembly architecture, GDT/IDT exception handling, APIC timer, PMM/VMM memory management, preemptive scheduler, and POSIX-aligned system call table.
3. **Compositor & Graphics (`graphics/`, `gui/`):** Double-buffered compositor directly managing display surfaces, rendering anti-aliased geometry and fonts with the central design tokens.
4. **Shell & Workspace Engine (`gui/`):** Native left vertical dock, dynamic workspace pill manager, spring-physics animated system popovers, and audio-reactive waveform visualizer.

# Chef OS Requirements Traceability Matrix

| Req ID | Category | Requirement Description | Target Gate | Status |
| :--- | :--- | :--- | :---: | :---: |
| **REQ-CORE-01** | Project Isolation | Independent repository, git history, and remote separate from Chef_Carthy | G1 | IN PROGRESS |
| **REQ-BOOT-01** | Boot | UEFI-compliant 64-bit bootloader loading kernel image | G4 | OPEN |
| **REQ-KERN-01** | Kernel | 64-bit kernel with PMM (bitmap/buddy), VMM (4-level paging), IDT, and scheduler | G5 | OPEN |
| **REQ-SYSC-01** | ABI | Stable system call interface and user/kernel privilege transition | G6 | OPEN |
| **REQ-DRIV-01** | Drivers | Framebuffer (GOP), PS/2 & USB HID (keyboard, pointer), timer (APIC/PIT), storage (AHCI/NVMe) | G7 | OPEN |
| **REQ-FS-01**   | Storage | Partition-aware filesystem driver with read/write and integrity checks | G8 | OPEN |
| **REQ-INIT-01** | System | PID 1 init with dependency-ordered service management and IPC | G9 | OPEN |
| **REQ-USER-01** | Userland | Native CLI shell and core system tools | G10 | OPEN |
| **REQ-DISP-01** | Graphics | Double-buffered compositing window system exposing window/application state | G14 | OPEN |
| **REQ-DESK-01** | Shell | Signature single LEFT vertical dock with real app icons & dynamic running indicators | G17 | OPEN |
| **REQ-WS-01**   | Workspaces | Dynamic workspace creation, renaming, removal, and window assignment | G18 | OPEN |
| **REQ-KEY-01**  | Input | `Ctrl+W` delivered to app; `Super+W` reserved for OS window close | G18 | OPEN |
| **REQ-DS-01**   | Design | Central Chef Cream design tokens (`#F1EBDD`, `#F8F4EA`, `#2B2A28`, `#A6534A`, `#DCD3BE`) | G16 | OPEN |
| **REQ-POPV-01** | Shell | Compact, readable popovers for volume, network, time, notifications, and power | G19 | OPEN |
| **REQ-THM-01**  | Themes | Multi-theme support (Cream, Dark, Olive, Rose, Midnight) with full contrast validation | G20 | OPEN |
| **REQ-AUD-01**  | Audio | Real audio playback reactivity for dock waveform visualizer | G21 | OPEN |
| **REQ-COLD-01** | Acceptance | 33-step verified cold-boot acceptance test in VM environment | G24 | OPEN |

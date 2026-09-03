# Chef OS

**Chef OS** is an independent, from-scratch operating system designed with a custom boot path, kernel, userland, display architecture, and a cohesive, Mac-inspired desktop environment.

---

## Design Philosophy

- **Warm, Elegant Aesthetic:** Signature Chef Cream palette (`#F1EBDD` background, `#F8F4EA` surfaces, `#2B2A28` ink typography, `#A6534A` subtle accents).
- **Primary Left Vertical Dock:** A clean, vertical navigation anchor with real application icons, dynamic running indicators, and system controls.
- **Dynamic Workspaces:** Native, dynamic workspace management (Main, Coding, Research, Media + runtime additions).
- **Application Respect:** Window-level shortcuts (`Super+W`) isolated from application tab controls (`Ctrl+W`).
- **Cohesive Services:** Native IPC services for settings, audio/visualizer, network, themes, notifications, and power management.

---

## Project Structure

```
chef-os/
├── boot/         # Bootloader & EFI/firmware interface
├── kernel/       # Core OS kernel, memory management, scheduler, syscalls
├── drivers/      # Device drivers (storage, input, display, net, audio)
├── system/       # Init, service manager, daemon infrastructure
├── user/         # Core userland utilities, shell, standard libraries
├── graphics/     # Framebuffer abstraction, 2D renderer, composition
├── gui/          # Native Chef OS window system & shell services
├── apps/         # Core system applications (Terminal, Files, Settings, Editor)
├── assets/       # Wallpapers, design tokens, iconography, fonts
├── toolchain/    # Cross-compilation toolchain & build environment
├── scripts/      # Build, test, and image creation utilities
├── tests/        # Unit, integration, and VM acceptance tests
└── docs/         # Architectural blueprints, requirements, logs, decisions
```

---

## Documentation & Governance

- [Master Build Directive](docs/MASTER_BUILD_DIRECTIVE.md) — The controlling project constitution.
- [Architecture Blueprint](docs/ARCHITECTURE.md) — Subsystem specifications and ABI contracts.
- [Build Progress](docs/BUILD_PROGRESS.md) — Milestone tracking and gate status.
- [Architecture Decisions (ADRs)](docs/DECISIONS.md) — Rationale and alternatives considered.
- [Requirements Traceability](docs/REQUIREMENTS.md) — Comprehensive requirement matrix.
- [V7 Reference Lessons](docs/V7_REFERENCE_NOTES.md) — Reusable ideas and lessons from historical V7.

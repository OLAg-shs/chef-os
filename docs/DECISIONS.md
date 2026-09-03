# Chef OS Architecture Decision Records (ADRs)

## ADR-0001: Strict Project Separation & From-Scratch Mandate
- **Date:** 2026-09-03
- **Status:** ACCEPTED
- **Context:** The historical Chef OS V7 project was built as a customized Arch Linux / Hyprland / Quickshell environment within repository `Chef_Carthy`. To achieve genuine operating system independence, a from-scratch architecture is required.
- **Decision:**
  1. Freeze `Chef_Carthy` as a permanent historical archive and reference library (`v7-archive`).
  2. Create an entirely new, isolated repository `chef-os` with fresh Git history and no shared lineage or submodules.
  3. All subsystems (bootloader, kernel, memory management, drivers, userland, compositor, shell, design system) will be designed and owned by Chef OS.
- **Consequences:**
  - High initial development investment, but complete ownership of ABI, lifecycle, and user experience.
  - Zero coupling to upstream Linux desktop package breaking changes.

---

## ADR-0002: Reference Utilization of V7 Assets and Algorithms
- **Date:** 2026-09-03
- **Status:** ACCEPTED
- **Context:** V7 established strong visual and interaction design principles (dynamic workspace state management, audio waveform visualization, warm cream aesthetic, popover geometry).
- **Decision:** Reusable concepts, formulas, and visual assets will be referenced from the frozen archive with explicit justification, but rewritten into native Chef OS architecture rather than imported as dependencies.

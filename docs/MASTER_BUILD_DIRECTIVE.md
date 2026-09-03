# CHEF OS — FROM-SCRATCH MASTER BUILD DIRECTIVE V1

## 0. DOCUMENT STATUS

This is the controlling master directive for the new Chef OS project. It is written for an autonomous Gemini/Antigravity CLI coding agent. Treat this document as the project constitution.

The agent must execute it sequentially, maintain persistent state, verify every milestone, and never silently change architecture or scope.

---

# 1. MISSION

Build a genuinely independent operating system named **Chef OS** from scratch.

The target is not an Arch Linux installation with a custom theme. The target is a coherent operating system project with its own boot path, kernel, userland, system services, graphics/session stack, GUI shell, design system, applications/integration, build system, installer/image, recovery strategy, and documentation.

The final experience must feel like one product: warm parchment/cream surfaces, dark ink typography, restrained red/olive accents, elegant rounded surfaces, real application icons, a single left vertical dock, dynamic workspaces, compact natural popovers, unified themes, smooth restrained motion, and a polished Mac-inspired interaction model.

V7 is a historical reference project, not the architecture of the new OS.

---

# 2. ABSOLUTE PROJECT BOUNDARY

There are TWO projects.

## OLD PROJECT — FROZEN REFERENCE

Repository: `Chef_Carthy`

Remote: `https://github.com/OLAg-shs/Chef_Carthy.git`

Purpose:
- historical archive
- V7 reference implementation
- source of lessons
- source of reusable ideas/assets when deliberately justified

Never:
- delete it
- reset it
- replace it with the new OS
- push new OS code to it
- change its identity into the new project

## NEW PROJECT — CHEF OS

Proposed repository: `chef-os`

Proposed local path: `/home/chef_carthy/Projects/chef-os`

Proposed remote: `https://github.com/OLAg-shs/chef-os.git`

The new project MUST have:
- independent directory
- independent Git repository
- independent Git history
- independent remote
- independent build system
- independent architecture

Before the first push, print and record the proposed repository name, local path, remote, and explicit confirmation that it differs from `Chef_Carthy`.

---

# 3. NON-NEGOTIABLE AGENT RULES

1. Read this entire document before implementation.
2. Read the relevant phase again immediately before executing that phase.
3. After every meaningful change, verify it.
4. Never claim a change happened unless a command/output proves it.
5. Never invent a file, command, package, API, hardware capability, repository state, or test result.
6. Inspect before modifying.
7. Never overwrite an existing project component blindly.
8. Never destroy working V7 material.
9. Never copy V7 architecture into the new OS merely because it exists.
10. Every architectural reuse must have a written reason.
11. Keep a persistent progress record.
12. Keep a decision record for architecture choices.
13. Keep a requirement traceability matrix.
14. Keep a build log.
15. Keep a known-issues log.
16. Keep a rollback point before risky operations.
17. Prefer small verified milestones over giant unverified changes.
18. Never make unrelated cleanup changes during a phase.
19. Do not skip a failed verification; stop and repair it.
20. Do not proceed to the next phase if a required gate is red.
21. Do not ask the user to manually perform work that can safely be performed by the agent, unless credentials, destructive confirmation, physical hardware action, or an unavoidable external approval is required.
22. If an external dependency is unavailable, document it and choose a justified fallback; do not silently redesign the project.
23. The agent must be able to resume after interruption using the state files.
24. Before each session, read `BUILD_PROGRESS.md`, `DECISIONS.md`, `KNOWN_ISSUES.md`, and this master directive.
25. At the end of each session, update all applicable state files.
26. Never expose secrets in source, logs, Git history, screenshots, or documentation.
27. Never embed GitHub access tokens in remotes, shell history, scripts, or files.
28. If a secret is discovered in existing project material, flag it and recommend rotation; do not reuse it.

---

# 4. REQUIRED PERSISTENT STATE

The new repository must contain at least:

`docs/MASTER_BUILD_DIRECTIVE.md`
`docs/BUILD_PROGRESS.md`
`docs/DECISIONS.md`
`docs/REQUIREMENTS.md`
`docs/KNOWN_ISSUES.md`
`docs/BUILD_LOG.md`
`docs/ARCHITECTURE.md`
`docs/TEST_PLAN.md`
`docs/RECOVERY.md`
`docs/V7_REFERENCE_NOTES.md`

State rules:
- update progress after each completed task
- record exact date/time
- record Git commit
- record verification result
- record remaining work
- record blockers

The progress file must always identify exactly one current phase/task.

---

# 5. EXECUTION LOOP

For EVERY task use this loop:

### A. READ
Read this master directive and the relevant section.

### B. INSPECT
Inspect the current repository, hardware/VM environment, files, toolchain, and previous state.

### C. PLAN
Write a short plan into the build log before changing files.

### D. SNAPSHOT
Create a Git checkpoint before risky modifications. If Git is unavailable, create a filesystem backup.

### E. IMPLEMENT
Make only the changes required for the current task.

### F. VERIFY
Run targeted tests and inspect actual outputs.

### G. RECORD
Update progress, requirements, decisions, issues, and build log.

### H. COMMIT
Commit only when the milestone is coherent and verified.

### I. REREAD
Reread this directive before moving forward.

---

# 6. DEFINITION OF FROM SCRATCH

For this project, “from scratch” means the final system is designed and owned as a Chef OS system rather than being a themed existing desktop distribution.

Target-owned subsystems:
- boot process/bootloader integration
- kernel
- hardware abstraction and low-level initialization
- memory/process primitives
- system call interface
- device/input interfaces
- filesystem/userland interface
- init/service model
- core command-line userland
- package/build model
- user/session model
- graphics initialization
- compositor/window system or equivalent display server architecture
- Chef OS graphical shell
- settings/control center
- notifications
- application launching
- workspace management
- theme engine
- wallpaper management
- power/network/audio integration
- recovery/update model

Development may use ordinary host tools and cross-compilers. That does NOT make the final OS an Arch/Ubuntu/etc. derivative.

The agent must document every third-party component used in the final system and why it is permitted.

If a component is temporarily used as scaffolding, label it `TEMPORARY_SCAFFOLD` and create a replacement/removal milestone.

---

# 7. V7 PRESERVATION GATE — MUST HAPPEN FIRST

Before new OS implementation:

1. Finish the read-only inventory of the V7 VM/host.
2. Compare VM files against `Chef_Carthy`.
3. Preserve missing V7 artifacts in the old repository.
4. Include relevant:
   - Quickshell files
   - Waybar configs/styles
   - Hyprland configs
   - SwayNC configs
   - Rofi configs
   - Chef scripts
   - workspace/theme state
   - wallpapers
   - custom assets
   - V5/V6/V7 specifications
   - build/progress/history records
   - visual proof artifacts where practical
5. Verify archive integrity.
6. Commit the archive to `Chef_Carthy`.
7. Tag it `v7-archived-reference`.
8. Push only after verifying the remote is exactly `Chef_Carthy`.
9. Record the final commit and tag.
10. Freeze the old project.

The V7 audit has already identified important files that were not in Git, including `~/.config/quickshell/shell.qml`, multiple Waybar files, Hyprland configuration, Chef scripts, workspace/theme state, wallpapers, specifications, and visual proof screenshots. Treat the inventory as evidence to verify, not as permission to guess missing files.

Do not start the new repository until the archive gate is green.

---

# 8. V7 LESSONS GATE

After preservation, create `docs/V7_REFERENCE_NOTES.md`.

Classify V7 findings into:

A. WORKED
B. FAILED
C. REUSABLE IDEA
D. REUSABLE ASSET
E. MUST REDESIGN
F. MUST NOT REPEAT

Important known lesson categories:
- V7 used Arch/Hyprland and therefore is not the new architecture.
- UI state must have readable contrast in every popover/theme.
- Dynamic workspace behavior must be real, not static decoration.
- Running applications should be derived from actual application/window state.
- Real application icons should come from authoritative icon/application metadata rather than random placeholders.
- Global hover/focus/active states must follow the active theme.
- The old UI had multiple components whose behavior became fragile when implemented as disconnected scripts/configurations.
- The new OS should design the system around explicit shared services and a central design system.

Every reused V7 artifact must list:
- source
- license/provenance if known
- reason for reuse
- modifications
- destination
- whether it is temporary or permanent

---

# 9. NEW REPOSITORY CREATION GATE

Only after V7 is frozen:

1. Create `/home/chef_carthy/Projects/chef-os`.
2. Verify it is not inside the old repository.
3. Verify no `.git` from the old project is inherited.
4. Initialize a fresh Git repository.
5. Create the initial documentation/state structure.
6. Verify `git remote -v` is empty before adding anything.
7. Record proposed remote.
8. Create/connect the NEW GitHub repository only after showing/recording:
   - name
   - path
   - remote
   - old repo name
   - explicit separation confirmation
9. Never add the old remote.
10. Never use the old repository as a Git submodule.

Initial commit: `chef-os: establish independent project foundation`.

---

# 10. ARCHITECTURE DISCOVERY — DO NOT GUESS

Before writing the kernel or GUI, perform an architecture study.

Determine and document:
- target CPU architecture
- boot firmware target (UEFI first unless hardware constraints require otherwise)
- boot protocol
- kernel language/toolchain
- kernel layout
- physical memory manager
- virtual memory manager
- scheduler model
- interrupt model
- syscall ABI
- userspace ABI
- executable format
- filesystem format
- storage driver strategy
- PCI/device discovery
- timer source
- keyboard/mouse input model
- framebuffer/GOP graphics path
- eventual GPU acceleration strategy
- network stack strategy
- audio strategy
- USB strategy
- process/service model
- IPC model
- permission/security model
- package/build model
- graphics/compositor model
- GUI toolkit strategy

Architecture choices must be recorded in `DECISIONS.md` with alternatives considered and rejection reasons.

Do not choose Hyprland, Waybar, SwayNC, Rofi, systemd, or another existing desktop stack simply because V7 used it.

---

# 11. BUILD SYSTEM FOUNDATION

Create a reproducible cross-compilation environment.

Requirements:
- deterministic source layout
- pinned toolchain versions
- host dependency checks
- one-command build entry point
- one-command clean build
- one-command test
- one-command image generation
- build artifacts separated from source
- checksums for release images
- build logs

Suggested high-level structure:

`toolchain/`
`kernel/`
`boot/`
`lib/`
`user/`
`system/`
`drivers/`
`graphics/`
`gui/`
`apps/`
`assets/`
`tests/`
`scripts/`
`docs/`
`build/`
`dist/`

Adjust only after documenting why.

The build system must support incremental development but also a clean reproducible build.

---

# 12. BOOTLOADER MILESTONE

Build the first bootable Chef OS image.

Minimum milestone:
- firmware loads Chef OS bootloader
- bootloader identifies the kernel
- kernel is loaded into memory
- kernel entry executes
- kernel writes a visible boot/debug message
- failure path prints a useful error

Tests:
- clean build
- boot in VM
- reboot repeatedly
- invalid kernel test produces controlled failure

Do not continue to GUI work until this is repeatably bootable.

---

# 13. KERNEL FOUNDATION

Implement incrementally:

13.1 CPU/architecture initialization
13.2 exception handling
13.3 interrupt descriptor/controller setup
13.4 timer
13.5 physical memory allocator
13.6 virtual memory/page tables
13.7 kernel heap if needed
13.8 basic synchronization primitives
13.9 process/thread model
13.10 scheduler
13.11 system call entry
13.12 user/kernel transition
13.13 basic IPC
13.14 clock/timekeeping
13.15 panic/debug facilities

Each subsystem gets:
- source
- unit/integration tests where possible
- boot test
- documentation
- failure behavior

The kernel must be able to boot to a controlled kernel shell/debug console before a graphical shell exists.

---

# 14. DEVICE/HARDWARE FOUNDATION

Implement in dependency order:
- firmware/platform discovery
- PCI enumeration
- storage
- keyboard
- pointer
- display framebuffer
- serial/debug console
- network device
- audio device
- USB where required

Hardware abstraction must isolate device-specific code.

Do not hard-code one VM device without documenting it.

Primary development target may be VMware/virtual hardware, but the architecture must not falsely claim universal hardware support.

---

# 15. FILESYSTEM AND STORAGE

Define:
- disk layout
- boot partition
- system partition
- writable state partition
- user data
- recovery area

Implement:
- filesystem reader
- filesystem writer
- directory operations
- file permissions
- timestamps
- mounting
- filesystem integrity checks

Provide a recovery mechanism before production release.

---

# 16. INIT AND SERVICE MANAGER

Create Chef OS's service lifecycle model.

Required concepts:
- PID 1/init
- service definitions
- dependency ordering
- startup
- shutdown
- restart
- failure isolation
- logs
- status
- user services

Do not automatically adopt systemd merely because it is familiar.

The service manager must expose an internal API usable by network, audio, GUI, notifications, and desktop services.

---

# 17. USERLAND FOUNDATION

Build the minimum coherent userland:
- shell
- process tools
- file tools
- directory tools
- text tools
- process/status tools
- system information
- logging
- environment management
- configuration management

Define a stable userspace ABI.

The shell must be usable before GUI completion.

---

# 18. SECURITY MODEL

Design security before broad application support.

Implement/document:
- user IDs
- groups/capabilities
- file permissions
- process isolation
- privilege boundaries
- privileged service interfaces
- secure boot considerations
- secret handling
- update authenticity
- crash/log privacy

Never store passwords, API keys, GitHub tokens, or credentials in the repository.

---

# 19. NETWORKING

Provide a coherent network service:
- interface discovery
- link state
- IP configuration
- DNS
- routing
- connectivity checks
- user-visible status

The GUI must consume a stable network API rather than scraping command output.

---

# 20. AUDIO

Provide a stable audio service:
- output device discovery
- input device discovery
- volume
- mute
- default device
- application stream state where supported
- playback metadata where supported

Expose a clean IPC/API layer to the GUI.

The audio visualizer must consume real playback data and degrade gracefully when no music is playing.

---

# 21. GRAPHICS FOUNDATION

Bring up:
- framebuffer/GOP first
- display abstraction
- input events
- surface model
- composition model
- window lifecycle
- focus
- pointer
- keyboard events

Then implement the accelerated graphics path when the architecture and driver support are mature.

Do not build the final UI as a collection of screenshots or fake panels.

---

# 22. CHEF OS WINDOW/COMPOSITOR MODEL

The graphical system must support:
- multiple windows
- focus
- minimize/restore if designed
- maximize/fullscreen
- move/resize
- stacking or tiling according to the Chef OS design
- workspace assignment
- application identity
- window identity
- close
- keyboard shortcuts
- pointer interaction

The compositor/window manager must expose real state to the Chef shell.

---

# 23. CENTRAL CHEF DESIGN SYSTEM

Create one source of truth for:
- colors
- typography
- spacing
- corner radii
- borders
- shadows
- icon sizing
- animation timing
- easing
- hover
- focus
- active
- pressed
- disabled
- selected
- error
- success
- warning

Default palette:

Background: `#F1EBDD`
Surface: `#F8F4EA`
Surface Alt: `#E8E0CE`
Text: `#2B2A28`
Text Muted: `#6E6A5F`
Accent: `#A6534A`
Border: `#DCD3BE`

These are the default Chef Cream tokens. Do not scatter raw color values throughout applications.

Every component must consume design tokens.

---

# 24. TYPOGRAPHY

Use a modern, readable sans-serif family with a reliable open fallback such as Inter/Noto Sans depending on licensing and availability.

Requirements:
- consistent hierarchy
- readable small text
- adequate contrast
- predictable metrics
- monospace font only where technically appropriate

Never assume a proprietary font exists.

---

# 25. WALLPAPER SYSTEM

Default wallpaper target:
- 1920x1080 PNG
- background approximately `#F1EBDD`
- Chef artwork centered toward the right side
- substantial negative space on the left

Wallpaper service must support:
- selection
- preview
- applying
- persistence
- safe fallback
- per-theme relationship if desired

Do not hard-code one wallpaper path into every component.

---

# 26. PRIMARY DESKTOP SHELL

The main navigation is a **single elegant LEFT vertical dock**.

Do NOT use a primary horizontal top bar.

The dock must:
- be visually integrated into the desktop
- have consistent spacing
- support hover/focus/active states
- show real application icons
- show running applications dynamically
- provide launcher/search access
- provide system controls
- remain readable in every theme

The shell must not look like a generic Linux rice.

---

# 27. REAL APPLICATION ICONS

Use authoritative application metadata.

Preferred resolution order:
1. application desktop metadata
2. installed icon theme
3. application-provided icon
4. Chef OS fallback icon

Never use random placeholder squares merely to make the dock look populated.

Icons must have consistent optical sizing.

---

# 28. APPLICATION LAUNCHER

Implement a native Chef OS launcher.

Requirements:
- searchable
- keyboard navigable
- categorized or intelligently grouped
- real application icons
- recent/favorite applications if supported
- fast open
- smooth appearance/disappearance
- current theme
- correct contrast

The launcher must use the system application registry rather than hard-coded application names.

---

# 29. DYNAMIC RUNNING APPLICATIONS

The dock must reflect actual running application/window state.

Requirements:
- application appears when running
- disappears when no longer running unless pinned
- active application clearly identified
- multiple windows handled correctly
- clicking icon focuses application
- right-click/context interaction may expose windows/actions

Do not fake this with static configuration.

---

# 30. DYNAMIC WORKSPACES

Chef OS must support real dynamic workspaces.

Initial named workspaces:
1. Main
2. Coding
3. Research
4. Media

Additional workspaces can be created as needed.

Requirements:
- create
- remove
- rename
- switch
- assign windows
- move windows
- active indicator
- occupied indicator
- keyboard navigation
- pointer navigation
- persistence where appropriate

Suggested shortcuts:
- `Super + 1` → workspace 1
- `Super + 2` → workspace 2
- `Super + 3` → workspace 3
- etc.
- `Super + Shift + number` → move focused window

The system must gracefully support numbers beyond the initial four.

---

# 31. WINDOW/CLOSE SEMANTICS

Do NOT globally hijack `Ctrl+W`.

`Ctrl+W` must be delivered to the focused application so browsers, editors, terminals, file managers, and other applications can decide what their own tab-close behavior means.

Chef OS window-level close shortcut:

`Super + W`

This closes the focused window/application surface through the compositor/window system.

The implementation must distinguish application-level keyboard handling from OS-level window management.

---

# 32. SYSTEM POPOVERS

Clicking system controls must open compact, polished popovers rather than giant raw utility windows.

Required popovers:
- volume/audio
- Wi-Fi/network
- date/time
- notifications
- battery/power
- quick settings where applicable

Rules:
- compact
- coherent
- themed
- readable
- sufficient contrast
- keyboard accessible
- pointer accessible
- animated subtly
- dismissible by click-away/Escape

Hard failure:
Any popover where text disappears into the background, icons become unreadable, or state cannot be understood.

---

# 33. NOTIFICATION SYSTEM

Provide a native notification service and UI.

Support:
- app identity
- title/body
- priority
- timestamp
- dismissal
- notification history if designed
- grouping
- quiet/do-not-disturb mode

Notifications must use the central theme engine.

---

# 34. SETTINGS / CONTROL CENTER

Build a native settings/control center.

Core areas:
- appearance
- theme
- colors
- wallpaper
- display
- sound
- network
- keyboard shortcuts
- workspaces
- notifications
- power
- accessibility
- system information
- updates/recovery

Settings must modify real system state through documented services.

---

# 35. THEME ENGINE

Built-in themes:
- Chef Cream
- Chef Dark
- Chef Olive
- Chef Rose
- Chef Midnight

Also support custom colors.

Changing theme must update:
- dock
- launcher
- windows
- popovers
- notifications
- settings
- workspace indicators
- hover states
- focus states
- active states
- icons where appropriate
- typography where appropriate
- wallpaper relationship

No component is allowed to remain visually stranded in an old theme.

---

# 36. THEME VALIDATION

For every theme perform a matrix test:

Components × states:
- normal
- hover
- focus
- active
- pressed
- disabled
- selected
- error
- warning
- success

Check:
- text contrast
- icon contrast
- border visibility
- active-state clarity
- popover readability
- notification readability

Record failures in `KNOWN_ISSUES.md` and fix before release.

---

# 37. ANIMATION SYSTEM

Animations should feel natural and restrained.

Define shared durations/easing rather than arbitrary values.

Use motion for:
- dock appearance/state
- popover open/close
- workspace transition
- launcher appearance
- notification arrival
- focus changes where useful
- settings transitions

Do NOT use flashy animation for decoration.

Provide reduced-motion behavior.

---

# 38. AUDIO-REACTIVE VISUALIZER

When music is playing, show a tasteful waveform/equalizer visualization in the main navigation/dock region.

Rules:
- reacts to real playback/audio data
- subtle
- never blocks interaction
- disappears or becomes calm when idle
- follows theme
- low CPU use
- reduced-motion mode disables or simplifies it

Do not fake continuous animation when no audio is playing.

---

# 39. APPLICATION INTEGRATION

Define a standard application model:
- application ID
- display name
- icon
- executable/entry
- categories
- capabilities
- windows
- lifecycle

Applications must register with Chef OS instead of requiring shell-specific hacks.

Build a minimal initial app set only after the platform is stable:
- terminal
- file manager
- settings
- system monitor
- text editor
- image viewer/browser where feasible

Do not expand application count before the core OS is reliable.

---

# 40. ACCESSIBILITY

Minimum support:
- keyboard-only navigation
- visible focus
- readable contrast
- scalable text where architecture permits
- reduced motion
- clear status indicators
- screen reader/accessibility API planning
- color not being the only state signal

Accessibility is part of the architecture, not a last-minute patch.

---

# 41. POWER MANAGEMENT

Implement/document:
- battery state
- charging
- suspend
- resume
- screen power
- idle policy
- shutdown
- reboot

All power actions must have safe confirmation semantics where destructive.

---

# 42. UPDATE AND RECOVERY MODEL

Define before release:
- versioning
- package/image update mechanism
- atomicity or rollback strategy
- recovery environment
- failed-update behavior
- configuration backup
- integrity verification

A system update must never leave the machine unbootable without a recovery path.

---

# 43. TESTING STRATEGY

Testing must exist at every layer.

### Unit tests
Memory helpers, parsers, data structures, theme token logic, workspace logic, protocol parsing.

### Integration tests
Boot chain, filesystem, services, IPC, input, network, audio, graphics, shell.

### VM tests
Cold boot, reboot, shutdown, workspace operations, launcher, settings, popovers, themes.

### Regression tests
Every fixed bug gets a regression test where feasible.

### Visual tests
Use screenshots/video only as evidence; never as the implementation itself.

---

# 44. COLD-BOOT ACCEPTANCE TEST

Before declaring Chef OS usable:

1. Power off VM completely.
2. Start VM from cold state.
3. Bootloader appears.
4. Kernel starts.
5. Init starts.
6. Services start.
7. Graphics session starts.
8. Login/session reaches Chef desktop.
9. Wallpaper loads.
10. Left dock loads.
11. Launcher opens.
12. Real applications launch.
13. Running applications appear dynamically.
14. Workspaces switch.
15. Additional workspace can be created.
16. Window moves between workspaces.
17. `Ctrl+W` reaches application.
18. `Super+W` closes focused window.
19. Volume popover works.
20. Wi-Fi popover works.
21. Date/time popover works.
22. Notification UI works.
23. Battery/power UI works where hardware exposes it.
24. Settings works.
25. Theme changes work.
26. Custom colors work.
27. Wallpaper selector works.
28. Hover/focus/active states work.
29. Audio visualizer works with actual playback.
30. Reduced motion works.
31. Shutdown works.
32. Reboot works.
33. Recovery path is documented/tested.

---

# 45. PERFORMANCE REQUIREMENTS

Measure rather than guess.

Track:
- boot time
- memory use
- idle CPU
- compositor frame stability
- shell responsiveness
- launcher latency
- workspace switch latency
- popover open latency
- application launch latency
- audio visualizer CPU cost

Avoid premature micro-optimization, but never ignore measurable regressions.

---

# 46. CODE QUALITY

Rules:
- clear names
- small modules
- explicit ownership
- documented interfaces
- no giant monolithic GUI file
- no duplicated theme logic
- no duplicated IPC parsing
- no unexplained magic constants
- no dead experimental code in production paths
- no debug shortcuts left enabled

Every public subsystem gets documentation.

---

# 47. ERROR HANDLING

Every service and UI component must define failure behavior.

Examples:
- no network → show offline state
- no audio device → show unavailable state
- missing wallpaper → fallback wallpaper
- application crashes → shell remains alive
- notification service fails → desktop remains usable
- theme file corrupt → safe default theme
- workspace service fails → preserve active window usability
- graphics acceleration unavailable → documented fallback

Never show blank/invisible UI as an error state.

---

# 48. LOGGING

Logs must distinguish:
- kernel
- boot
- init
- services
- graphics
- shell
- applications

Provide useful severity levels.

Avoid logging secrets.

Provide a user-accessible diagnostic bundle mechanism eventually.

---

# 49. DOCUMENTATION

Maintain:
- architecture overview
- build guide
- boot guide
- kernel guide
- driver guide
- userspace ABI
- IPC protocol
- graphics architecture
- shell architecture
- theme API
- application API
- packaging/update guide
- recovery guide
- troubleshooting guide

Documentation must describe the current implementation, not aspirations presented as facts.

---

# 50. PHASE GATES

The project is divided into these gates:

G0 — V7 preserved and frozen
G1 — new repository isolated
G2 — architecture frozen
G3 — reproducible build environment
G4 — bootloader booting
G5 — kernel foundation
G6 — user/kernel ABI
G7 — hardware/device layer
G8 — filesystem/storage
G9 — init/service manager
G10 — userland
G11 — security model
G12 — networking/audio/input
G13 — graphics foundation
G14 — compositor/window system
G15 — Chef shell foundation
G16 — design system/wallpaper
G17 — dock/launcher/apps
G18 — dynamic workspaces/window behavior
G19 — popovers/notifications/settings
G20 — themes/customization
G21 — motion/audio visualizer/accessibility
G22 — update/recovery
G23 — integration/performance/security hardening
G24 — cold boot acceptance
G25 — release and repository milestone

Never skip a gate.

---

# 51. REQUIRED GATE REPORT

At every gate report:

`GATE: Gx`
`STATUS: GREEN/RED/BLOCKED`
`OBJECTIVE:`
`CHANGES:`
`FILES:`
`COMMANDS:`
`VERIFICATION:`
`TEST RESULTS:`
`KNOWN ISSUES:`
`GIT COMMIT:`
`NEXT TASK:`

If RED or BLOCKED, do not advance.

---

# 52. GIT DISCIPLINE

Commit milestones, not random edits.

Commit naming convention:

`chef-os: <milestone>`

Examples:
- `chef-os: establish independent project foundation`
- `chef-os: boot kernel milestone`
- `chef-os: implement physical memory manager`
- `chef-os: add init service model`
- `chef-os: bring up framebuffer shell`
- `chef-os: implement dynamic workspaces`

Before pushing:
1. inspect `git remote -v`
2. verify remote is `chef-os`
3. verify current branch
4. inspect diff
5. inspect status
6. run required tests
7. confirm no secrets

Never push blindly.

---

# 53. SECRET SAFETY

The previous V7 audit showed a GitHub access token embedded in a command/remote. Treat that credential as compromised if it was real.

Actions:
- do not reuse it
- do not copy it into the new project
- do not place credentials in documentation
- remove credentials from remotes
- recommend rotation/revocation
- use secure GitHub authentication mechanisms

Never print tokens in logs.

---

# 54. VISUAL DESIGN ACCEPTANCE

The final desktop must communicate:

**Chef OS** — not “Linux with a theme.”

Visual principles:
- warm
- elegant
- calm
- premium
- restrained
- coherent
- Mac-inspired without cloning proprietary UI
- strong typography
- generous negative space
- subtle depth
- natural motion
- excellent readability

The left dock is the primary navigation anchor.

There must not be a generic Linux top-bar-first visual hierarchy.

---

# 55. INTERACTION ACCEPTANCE

Every interactive object needs a clear state:
- normal
- hover
- focus
- active
- pressed
- selected
- disabled

Users must always be able to tell:
- where they are
- what workspace is active
- which application is focused
- what control they are hovering
- whether audio/network/power state is active

---

# 56. NO FAKE FUNCTIONALITY RULE

Never implement a fake control that looks functional but does nothing.

Examples:
- fake Wi-Fi list
- fake volume slider
- fake workspace pills
- fake running-app indicators
- fake theme selector
- fake settings switches
- fake audio visualizer

If a feature is not implemented, label it unavailable/development-only rather than pretending.

---

# 57. NO BIG-BANG BUILD RULE

Do not write the entire operating system in one giant change.

Build in vertical, bootable milestones.

Each milestone must leave the project in a known state.

Prefer:

boot → kernel → shell/debug → userspace → services → graphics → compositor → shell → UI features

rather than:

write everything → attempt boot → debug thousands of unknown failures.

---

# 58. VM DEVELOPMENT RULES

The VM is a test target, not the definition of the OS.

The agent must:
- record VM configuration
- record virtual hardware
- avoid destructive host changes
- snapshot before risky disk operations where possible
- test cold boots
- retain reproducible image artifacts

Never confuse “works in this VM” with “architecture complete.”

---

# 59. RELEASE CHANNELS

Use at least:
- development
- milestone
- release candidate
- stable

A milestone image must identify:
- Git commit
- build date
- toolchain version
- architecture
- image checksum

---

# 60. DEFINITION OF DONE

Chef OS is not done because the desktop looks good.

Chef OS is done only when:
- it boots from its own image
- its kernel and userspace architecture are documented
- core services are stable
- storage works
- users can interact with the system
- graphics work reliably
- the Chef shell is native to the project
- workspaces are real
- applications are real
- system controls are real
- themes are real
- settings are real
- recovery exists
- updates are safe
- tests pass
- cold boot passes
- repository history is clean
- no secrets are present
- documentation matches implementation

---

# 61. INITIAL EXECUTION ORDER — EXACT

The agent must begin in this exact order:

### STEP 1
Read this master directive completely.

### STEP 2
Read the current V7 preservation state and build log.

### STEP 3
Finish the V7 read-only inventory.

### STEP 4
Verify every listed V7 artifact exists before archiving it.

### STEP 5
Archive V7 into `Chef_Carthy` without destroying anything.

### STEP 6
Verify the old repository contains the intended V7 archive.

### STEP 7
Commit and tag the V7 archive.

### STEP 8
Verify `Chef_Carthy` remote before pushing.

### STEP 9
Freeze the old project.

### STEP 10
Create the new `/home/chef_carthy/Projects/chef-os` directory.

### STEP 11
Initialize fresh Git history.

### STEP 12
Create the persistent state/documentation files.

### STEP 13
Perform architecture discovery.

### STEP 14
Write and freeze `ARCHITECTURE.md` for the first implementation milestone.

### STEP 15
Build the reproducible cross-toolchain.

### STEP 16
Create the bootloader milestone.

### STEP 17
Boot the kernel in the VM.

### STEP 18
Proceed through the numbered gates sequentially.

At every step: inspect → plan → snapshot → implement → verify → record → commit → reread.

---

# 62. WHAT THE AGENT MUST NEVER DO

Never:
- turn the project back into Arch/Hyprland
- install a desktop and call it Chef OS
- copy the V7 architecture wholesale
- overwrite `Chef_Carthy`
- push to `Chef_Carthy`
- delete V7 history
- use static fake workspace UI
- use static fake running-app UI
- globally hijack Ctrl+W
- create unreadable popovers
- hard-code every application
- scatter theme colors through the codebase
- skip tests because the screen “looks right”
- claim success without verification
- leave temporary hacks undocumented
- commit credentials
- make architecture decisions without recording them
- silently change requirements

---

# 63. HOW TO HANDLE UNCERTAINTY

When uncertain:

1. Stop.
2. Inspect local evidence.
3. Inspect project documentation.
4. Check toolchain/version information.
5. Research authoritative technical documentation if web access is available.
6. Compare alternatives.
7. Record the decision.
8. Implement only after the decision is justified.

Never fill an unknown with an invented assumption.

If two requirements conflict, identify the conflict explicitly and preserve the higher-level product requirement.

---

# 64. HOW TO HANDLE FAILURES

When a command fails:

1. Capture exact error.
2. Do not hide it.
3. Determine whether the failure is environmental, architectural, implementation, or tooling-related.
4. Fix the smallest root cause.
5. Re-run the original failing test.
6. Add a regression test where possible.
7. Update `KNOWN_ISSUES.md` if relevant.
8. Update `BUILD_LOG.md`.
9. Only then continue.

Never “fix” a failure by deleting the failing subsystem without documenting the architectural consequence.

---

# 65. FINAL PROJECT MEMORY

The agent must preserve these facts across sessions:

- Chef OS V7 is archived and frozen.
- `Chef_Carthy` is never the new OS repository.
- `chef-os` is the new independent repository.
- New OS architecture is from scratch.
- V7 is reference material only.
- The product experience is premium, warm, Mac-inspired, and cohesive.
- The primary navigation is a left vertical dock.
- Workspaces are dynamic.
- Running applications are dynamic.
- Applications use real icons.
- `Ctrl+W` belongs to applications.
- `Super+W` is Chef OS window close.
- Popovers must be compact and readable.
- Themes affect the entire system.
- Custom colors are supported.
- Wallpaper selection is supported.
- Audio visualization is real and restrained.
- Accessibility and reduced motion are required.
- Every major milestone is tested and recorded.

---

# 66. FINAL INSTRUCTION TO THE AGENT

You are not being asked to improvise a Linux desktop.

You are building an operating system project.

Think like:
- an OS engineer
- a kernel engineer
- a systems architect
- a graphics engineer
- a desktop-shell engineer
- a UI/UX engineer
- a release engineer
- a security engineer
- a test engineer

But execute like a disciplined build agent:

**inspect first → reason → plan → implement one bounded step → verify → record → commit → reread this document → continue.**

The goal is not to make a screenshot that looks like Chef OS.

The goal is to make a system that **is Chef OS**.

When a phase is complete, prove it.
When something fails, record it.
When something is uncertain, investigate it.
When something is reused, justify it.
When something changes, verify it.
When a session ends, save the state.

Never lose the project history.
Never lose the reasoning.
Never lose the ability to recover.
Never confuse the old V7 reference with the new operating system.

**Build Chef OS from the foundation upward. One verified milestone at a time.**

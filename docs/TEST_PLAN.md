# Chef OS Test Plan & Acceptance Matrix

## Test Levels

### 1. Unit Tests (`tests/unit/`)
- Memory allocator boundary conditions and fragmentation tests.
- String, path, and protocol serialization parsers.
- Design system color palette and token math validation.
- Workspace data model state transitions.

### 2. Integration Tests (`tests/integration/`)
- Kernel boot to userland transition.
- Syscall ABI conformance.
- Service startup, shutdown, and crash recovery.
- Compositor surface allocation, dirty rect updating, and input routing.

### 3. VM & Cold-Boot Acceptance (`tests/vm/`)
- Cold power-on boot validation in VMware and QEMU.
- Full 33-step userland interaction acceptance suite.

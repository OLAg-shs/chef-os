# Chef OS Known Issues & Risk Log

| Issue ID | Subsystem | Description | Severity | Mitigation / Status |
| :--- | :--- | :--- | :---: | :--- |
| **ISS-001** | Project Setup | Initial toolchain setup requires reproducible host dependencies | Medium | Document and pin host toolchain versions in `toolchain/` |
| **ISS-002** | Graphics | Framebuffer GOP rendering requires fast software blitting before hardware acceleration | Medium | Implement optimized double-buffered dirty-rectangle software compositor |

# Kernel Module Cross-Compilation (ARM64)

This project demonstrates how to write and **cross-compile** a simple Linux kernel module for an ARM64 target system (such as QEMU or embedded boards).

---

## 🔑 Key Concepts

### Native Compilation
- **Definition:** Compiling software **on the same architecture** and system where it will run.  
- **Example:** Building a kernel module on an x86-64 Ubuntu machine, and also running it there.  
- **Pros:** Simple, no toolchains needed.  
- **Cons:** Not possible if your target system is resource-constrained (e.g., embedded boards).  

### Cross Compilation
- **Definition:** Compiling software on a **different architecture** than the one it will run on, using a **cross-compiler toolchain**.  
- **Example:** Building a kernel module on an x86-64 host for an **ARM64 target** using `aarch64-linux-gnu-gcc`.  
- **Pros:** Faster builds, can target small/embedded systems.  
- **Cons:** Requires correct cross-toolchain, matching kernel headers.  

---
### 🐧 Cross Compilation Overview

![Cross Compilation Flow](doc/cross_compiling_diagram.svg)



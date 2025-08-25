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

## 1. Download and Extract Linux Kernel 5.10

```bash
wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.10.tar.xz
tar xf linux-5.10.tar.xz
cd linux-5.10
```

## 2. Configure and Build Kernel for QEMU (ARM64)

Set cross-compilation environment variables:
```bash
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
```

Build the kernel:
```bash
make defconfig
make menuconfig     # (optional for enabling extra options)
make -j$(nproc) Image dtbs
```
The compiled kernel image will be located at:
```bash
arch/arm64/boot/Image
```
### 3. Prepare a Minimal BusyBox RootFS

Create rootfs directory structure:
```bash
mkdir qemu-rootfs
cd qemu-rootfs
mkdir -p {bin,sbin,etc,proc,sys,usr/{bin,sbin}}
```

Download and build BusyBox:
```bash
wget https://busybox.net/downloads/busybox-1.36.0.tar.bz2
tar xf busybox-1.36.0.tar.bz2
cd busybox-1.36.0

make defconfig
make CROSS_COMPILE=aarch64-linux-gnu- install

```

Copy BusyBox install files into the rootfs:
```bash
cp -r _install/* ../qemu-rootfs/
cd ..
```

### 4. Create init Script

Inside qemu-rootfs, create a file named init:
```bash
cd qemu-rootfs
cat > init << 'EOF'
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
echo "Welcome to Minimal BusyBox RootFS"
exec /bin/sh
EOF
chmod +x init
cd ..
```

### 5. Create RootFS Image

Pack the filesystem into an initramfs:
```bash
cd qemu-rootfs
find . | cpio -o -H newc > ../rootfs.cpio
cd ..
gzip rootfs.cpio
```
Now we have: rootfs.cpio.gz

### 6. Run QEMU with Kernel + RootFS
```bash
qemu-system-aarch64 \
    -M virt -cpu cortex-a53 -nographic \
    -kernel linux-5.10/arch/arm64/boot/Image \
    -initrd rootfs.cpio.gz \
    -append "console=ttyAMA0 rdinit=/init"
```
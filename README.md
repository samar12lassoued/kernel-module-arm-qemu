# Kernel Module Cross-Compilation (ARM64)

This project demonstrates how to write and **cross-compile** a simple Linux kernel module for an ARM64 target system (such as QEMU or embedded boards).

---

## 🔑 Key Concepts

### Native Compilation
- **Definition:** consists of compiling software **on the same architecture** and system where it will run.  
- **Example:** Compiling a c program  on an x86-64 Ubuntu machine, and also running it there.  
- **Cons:** Not possible if our target system is resource-constrained (e.g., embedded boards).  

### Cross Compilation
- **Definition:** consists of compiling software on a **different architecture** than the one it will run on, using a **cross-compiler toolchain**.  
- **Example:** Building a kernel module on an x86-64 host for an **ARM64 target** using `gcc-aarch64-linux-gnu`.  

  ***Why we need cross-compilation?***

  We need Cross compilation since we can't compile directly on the target itself due to limited ressources and the compilation process itself needs large ressources.
  
---
### 🐧 Cross Compilation Overview

![Cross Compilation Flow](doc/cross_compiling_steps-Page.svg)

## 1. Install Required Packages
```bash
sudo apt install -y \
    qemu-system-arm \
    gcc-aarch64-linux-gnu \
    build-essential \
    libncurses-dev \
    bison \
    flex \
    libssl-dev \
    libelf-dev \
    cpio \
    kmod
```

## 2. Download and Extract Linux Kernel 6.6

```bash
wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-6.6.tar.xz
tar xf linux-6.6.tar.xz
cd linux-6.6
```

## 3. Configure and Build Kernel for QEMU (ARM64)

Set cross-compilation environment variables:
```bash
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
```


```bash
# Configure kernel
make defconfig

# Enable necessary options (optional but recommended)
make menuconfig

# Build the kernel:
make -j$(nproc) Image 
make -j$(nproc) modules
```
The compiled kernel image will be located at:
```bash
arch/arm64/boot/Image
```


### 4. Download and Build BusyBox
For our embedded environment, we'll build a minimal rootfs using BusyBox, which packages numerous essential Unix utilities into a single compact binary. This approach dramatically simplifies our build by creating a statically linked system, eliminating the need for dynamic linking or shared library dependencies.
```bash
cd ..
wget https://busybox.net/downloads/busybox-1.36.0.tar.bz2
tar xf busybox-1.36.0.tar.bz2
cd busybox-1.36.0

# Configure BusyBox
make defconfig

# Optional: Enable static linking
make menuconfig
# Navigate to:
# - Settings -> Build static binary (no shared libs) [enable]
# - Save and exit

# Build BusyBox for ARM64
make CROSS_COMPILE=aarch64-linux-gnu- install

# Verify the build
file _install/bin/busybox
# Should show: ELF 64-bit LSB executable, ARM aarch64
```
### 4. Prepare a Minimal BusyBox RootFS
Create Root Filesystem directory structure:

```bash
cd ..
mkdir -p rootfs/{bin,dev,etc,proc,sys,tmp,usr/bin}

# Copy BusyBox
cp -r busybox-1.36.0/_install/* rootfs/

# Create essential device nodes
sudo mknod rootfs/dev/console c 5 1
sudo mknod rootfs/dev/null c 1 3
```


### 5. Create init Script
For the kernel to pass control to the userspace, it attempts to execute an init process as the first process. Therefore we'll create our own shell script to run as the first process. Create rootfs/init script:

```bash
cd rootfs
cat > init << 'EOF'
#!/bin/sh

# Mount essential filesystems
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev

# Set up environment
export PATH=/bin:/sbin:/usr/bin:/usr/sbin
export SHELL=/bin/sh

echo "Minimal Linux environment ready"
exec /bin/sh
EOF

# Setup the script to be executable
chmod +x init
cd ..
```

### 7. Module Compilation and Integration 

Cross-compile the module
```bash
make KDIR=linux-6.6 cross-compile
```
***Verify ARM64 module***
```bash
file hello_module.ko
```
Add to rootfs and rebuild initramfs
```bash
cp hello_module.ko rootfs/
cd rootfs
find . | cpio -o -H newc | gzip > ../initramfs.cpio.gz
cd ..
```
Now we have: initramfs.cpio.gz



### 8. Boot QEMU with Kernel and Initramfs
```bash
qemu-system-aarch64 \
    -M virt \
    -cpu cortex-a53 \
    -smp 2 \
    -m 1G \
    -kernel linux-6.6/arch/arm64/boot/Image \
    -initrd initramfs.cpio.gz \
    -append "console=ttyAMA0 earlycon=pl011,0x9000000 init=/init" \
    -nographic \
    -no-reboot
```

### 9.  Inside QEMU: Test the Module
```bash
# Once you get the shell prompt in QEMU: Load and test module

insmod hello_module.ko
dmesg | tail -3

rmmod hello_module
dmesg | tail -3
```
Verify Everything Works :
You should see:

    1. Kernel boots successfully

    2. Shell prompt appears

    3. Module can be loaded/unloaded

    4. "Hello, ARM64 World!" and "Goodbye, ARM64 World!" messages in dmesg

#### 💡 Final Note: Overcoming the Fear of Embedded Linux
See Embedded Linux is not that scary after all ! so what we have achieved today : 

1. Cross-compiled a Linux kernel for ARM64
2. Built a minimal root filesystem with BusyBox
3. Created a working initramfs
4. Wrote and cross-compiled a kernel module
5. Booted a virtual ARM64 machine with QEMU
6. Loaded and tested our custom kernel module

🌟 Remember
All you have to do is start learning and implementing things you thought were hard or complex. 
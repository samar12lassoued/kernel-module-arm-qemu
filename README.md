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

![Cross Compilation Flow](doc/cross_compiling_steps.svg)

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

## 2. Download and Extract Linux Kernel 5.10

```bash
wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.10.tar.xz
tar xf linux-5.10.tar.xz
cd linux-5.10
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
# Navigate to:
# - Device Drivers -> Character devices -> Serial drivers -> ARM AMBA PL011 serial port support (enable)
# - General setup -> Initial RAM filesystem and RAM disk support (enable)
# - Save and exit

# Build the kernel:
make -j$(nproc) Image dtbs
```
The compiled kernel image will be located at:
```bash
arch/arm64/boot/Image
```


### 4.Download and Build BusyBox
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

Inside rootfs, create a file named init:
```bash
cd rootfs
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

### 6. Create Create Initramfs

Pack the filesystem into an initramfs:
```bash
cd rootfs
find . | cpio -o -H newc | gzip > ../initramfs.cpio.gz
cd ..
```
Now we have: initramfs.cpio.gz

### 7. Cross-Compile the Module
# Use the kernel source we built
```bash
make KDIR=../linux-5.10 cross-compile
```

### 8. Test the Module in QEMU
```bash
# Copy module to rootfs
cp hello_module.ko rootfs/

# Recreate initramfs with module
cd rootfs
find . | cpio -o -H newc | gzip > ../initramfs.cpio.gz
cd ..
```

### 9. Boot QEMU with Kernel and Initramfs
```bash
qemu-system-aarch64 \
    -M virt \
    -cpu cortex-a53 \
    -smp 2 \
    -m 1G \
    -kernel linux-5.10/arch/arm64/boot/Image \
    -initrd initramfs.cpio.gz \
    -append "console=ttyAMA0 earlycon=pl011,0x9000000 init=/init" \
    -nographic \
    -no-reboot
```

### 6.  Inside QEMU: Test the Module
```bash
# Once you get the shell prompt in QEMU:
ls -la /hello_module.ko
insmod /hello_module.ko
dmesg | tail -5
rmmod hello_module
dmesg | tail -5
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
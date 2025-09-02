KDIR ?= compile_kernel/linux-5.10
CROSS_COMPILE ?= aarch64-linux-gnu-
ARCH ?= arm64

obj-m += hello.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

cross-compile:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
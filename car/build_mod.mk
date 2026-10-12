CROSS_PATH := /home/book/100ask_imx6ull-sdk/ToolChain/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin/arm-buildroot-linux-gnueabihf-

ARCH := arm

KERNELDIR := /home/book/100ask_imx6ull-sdk/Linux-4.9.88

CURRENT_PATH := $(shell pwd)

MODULE_INSTALL_PATH := /home/book/Desktop/car/modules

MAKE := /usr/bin/make
#GCC_ENV := true

.PHONY: all install kernel_modules clean clean_modules

all: install

install:kernel_modules
	@echo "Installing kernel modules to $(MODULE_INSTALL_PATH)"
	mkdir -p -m 755 $(MODULE_INSTALL_PATH)
	cp -f *.ko $(MODULE_INSTALL_PATH)/
ifeq ($(FIRMWARE_CLEAN),1)
	$(MAKE) clean
endif

kernel_modules:
	@echo "Building kernel modules..."
	$(MAKE) -C $(KERNELDIR)  M=$(CURRENT_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	@echo "Cleaing build artifacts..."
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
clean_modules:
	rm -f $(CURRENT_PATH)/*.ko
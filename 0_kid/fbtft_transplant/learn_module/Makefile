ifneq ($(KERNELRELEASE),)

# Core module
obj-m           += hello.o
obj-m           += server.o
obj-m           += client.o
obj-m           += test_param.o


else
	
KDIR := /home/wupf/work/nfsroot/awlinux-wupf/kernel/linux-sunxi-3.4

all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
clean:
	rm -rf *.ko *.o .*.cmd  *.order *.mod.c *.symvers .tmp_versions 
	
endif


ifneq ($(KERNELRELEASE),)

# Core module
obj-m       += fbtft.o
fbtft-objs  := fbtft-core.o
fbtft-objs  += fbtft-sysfs.o
fbtft-objs  += fbtft-bus.o
fbtft-objs  += fbtft-io.o
fbtft-objs  += fbtft-iteadio.o

# drivers
obj-m += fb_bd663474.o
obj-m += fb_hx8340bn.o
obj-m += fb_hx8347d.o
obj-m += fb_hx8353d.o
obj-m += fb_ili9320.o
obj-m += fb_ili9325.o
obj-m += fb_ili9340.o
obj-m += fb_ili9341.o
obj-m += fb_pcd8544.o
obj-m += fb_s6d02a1.o
obj-m += fb_s6d1121.o
obj-m += fb_ssd1289.o
obj-m += fb_ssd1306.o
obj-m += fb_ssd1331.o
obj-m += fb_ssd1351.o
obj-m += fb_st7735r.o
obj-m += fb_tinylcd.o
obj-m += fb_upd161704.o
obj-m += fb_watterott.o
obj-m += flexfb.o

# Device modules
obj-m += fbtft_device.o


else
	
KDIR := /home/wupf/work/nfsroot/awlinux-wupf/kernel/linux-sunxi-3.4

all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
clean:
	rm -rf *.ko *.o .*.cmd  *.order *.mod.c *.symvers .tmp_versions 
	
endif


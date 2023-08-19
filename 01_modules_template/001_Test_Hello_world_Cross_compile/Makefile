obj-m := 01_Hello_Raspberry_Cross_Compiled.o
01_Hello_Raspberry_Cross_Compiled-objs := 01_Hello_world.o

ARCH=arm
CROSS_COMPILE=arm-linux-gnueabihf-
KERN_DIR_TARGET=/home/varun/Raspberry_PI/kernel/linux
KERN_DIR_HOST=/lib/modules/$(shell uname -r)/build/


all:
	#make -C $(KERN_DIR_HOST) M=$(PWD) modules
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR_TARGET) M=$(PWD) modules
	scp *.ko pi@192.168.0.110:~/

clean:
	#make -C $(KERN_DIR_HOST) M=$(PWD) clean
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR_TARGET) M=$(PWD) clean

target:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR_TARGET) M=$(PWD) modules

clean_target:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR_TARGET) M=$(PWD) clean



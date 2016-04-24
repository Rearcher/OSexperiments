obj-m := helloworld.o
KVERSION = $(shell uname -r)
default:
	make -C /lib/modules/$(KVERSION)/build M=$(shell pwd) modules
clean:
	rm -rf *.o .*.cmd *.ko *.mod.c .tmp_versions *.order *.symvers

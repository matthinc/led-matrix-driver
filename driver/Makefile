obj-m += led_matrix.o

KDIR = /lib/modules/$(shell uname -r)/build

all: test_userspace
	rmmod led_matrix; \
	make -C $(KDIR) SUBDIRS=$(PWD) M=$(shell pwd) modules; \
	insmod led_matrix.ko


clean:
	make -C $(KDIR)  M=$(shell pwd) clean

test_userspace test_userspace.c:
	gcc -o test_userspace test_userspace.c

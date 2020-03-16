
EXTRA_CFLAGS += -I$(src)/../include
KDIR = /lib/modules/`uname -r`/build
obj-m := pmu_demo.o
M := make -C ${KDIR}  M=`pwd`


all:
	${M} modules

install:
	${M} modules_install

clean:
	${M} clean


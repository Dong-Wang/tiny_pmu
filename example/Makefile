
EXTRA_CFLAGS += -I${src}/../include
KDIR = /lib/modules/`uname -r`/build
M := make -C ${KDIR}  M=`pwd`

obj-m := pmu_demo.o \
		top_down_demo.o

all:
	${M} modules

install:
	${M} modules_install

clean:
	${M} clean



# usage:
# user space application: make / make clean
# kernel module: make kmod / make kmod_clean

TARGET = pmu_info

# user applicaiton
U := gcc -g -Wall

all:${TARGET}

${TARGET}:pmu_info.c
	${U} $^ -o $@

clean:
	@rm -f ${TARGET}

# kernel module
KDIR = /lib/modules/`uname -r`/build
M := make -C ${KDIR}  M=`pwd`

obj-m := ${TARGET}.o
${TARGET}-y := pmu_info_kmod.o

kmod:
	${M} modules

kmod_clean:
	${M} clean

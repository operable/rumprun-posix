#!/bin/sh

set -x

NAME=$1

shift

LINK=$*

OBJDIR="obj-rr"

mkdir -p ${OBJDIR}

CC=${CC-cc}

RUMPLIBS="-Lrumpdyn/lib -Wl,--no-as-needed -lrumpkern_time -lrumpvfs -lrumpfs_kernfs -lrumpdev -lrumpnet_local -lrumpnet_netinet -lrumpnet_netinet6 -lrumpnet_net -lrumpnet -lrump -lrumpuser -lrumpkern_time"

${CC} -Wl,-r -nostdlib ${LINK} -o ${OBJDIR}/tmp0_${NAME}.o
objcopy --redefine-syms=env.map ${OBJDIR}/tmp0_${NAME}.o
${CC} -Wl,-r ${OBJDIR}/tmp0_${NAME}.o -nostdlib rump/lib/libc.a -o ${OBJDIR}/tmp1_${NAME}.o 2>/dev/null
objcopy --redefine-syms=extra.map ${OBJDIR}/tmp1_${NAME}.o
objcopy --redefine-syms=rump.map ${OBJDIR}/tmp1_${NAME}.o
objcopy --redefine-syms=emul.map ${OBJDIR}/tmp1_${NAME}.o
objcopy --redefine-sym environ=_netbsd_environ ${OBJDIR}/tmp1_${NAME}.o
objcopy --redefine-sym __progname=_netbsd__progname ${OBJDIR}/tmp1_${NAME}.o
objcopy --redefine-sym exit=emul_exit ${OBJDIR}/tmp1_${NAME}.o
${CC} -Wl,-r -nostdlib -Wl,-dc ${OBJDIR}/tmp1_${NAME}.o readwrite.o -o ${OBJDIR}/tmp2_${NAME}.o 2>/dev/null
objcopy -w --localize-symbol='*' ${OBJDIR}/tmp2_${NAME}.o
objcopy -w --globalize-symbol='_netbsd_*' ${OBJDIR}/tmp2_${NAME}.o
objcopy --globalize-symbol=rumprun_functions ${OBJDIR}/tmp2_${NAME}.o
objcopy -w --globalize-symbol='main_*' ${OBJDIR}/tmp2_${NAME}.o
${CC} ${OBJDIR}/tmp2_${NAME}.o rumprun.o emul.o rumpinit.o nullenv.o ${RUMPLIBS} -o ${NAME} 


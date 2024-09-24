#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-
DEPENDENCY=/home/rishap/rishap/Coursera/LinuxSystemProgramming/assignment-1-rishap777/dependency

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}
OUTDIR=$(realpath $OUTDIR)
echo $OUTDIR

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux/arch/${ARCH}/boot/Image ]; then
    cd linux
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- mrproper
    
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- defconfig
    
    make -j4 ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- all
    
    make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- modules
 
 	make ARCH=arm64 CROSS_COMPILE=aarch64-none-linux-gnu- dtbs
  
fi

echo "Adding the Image in outdir"

cp ${OUTDIR}/linux/arch/${ARCH}/boot/Image $OUTDIR

echo "Creating the staging directory for the root filesystem"
cd ..
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
cd "$OUTDIR"
pwd
mkdir rootfs
cd rootfs
mkdir -p bin dev etc lib lib64 proc sbin sys tmp usr/bin usr/sbin var var/run home

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make distclean
    make defconfig
else
    cd busybox
fi

# TODO: Make and install busybox

make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} 

make CONFIG_PREFIX=${OUTDIR}/rootfs ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install


cd "$OUTDIR/rootfs"
echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
echo "Add dependency" 
cd "$OUTDIR"
cp ${DEPENDENCY}/ld-linux-aarch64.so.1 ${OUTDIR}/rootfs/lib
cp ${DEPENDENCY}/libc.so.6 ${DEPENDENCY}/libm.so.6 ${DEPENDENCY}/libresolv.so.2 ${OUTDIR}/rootfs/lib64

# TODO: Make device nodes
cd "$OUTDIR/rootfs"
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 666 dev/console c 5 1

# TODO: Clean and build the writer utility
cd "$FINDER_APP_DIR"
aarch64-none-linux-gnu-gcc writer.c -o writer
cp writer "$OUTDIR"/rootfs/home

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cp finder.sh conf/username.txt conf/assignment.txt finder-test.sh autorun-qemu.sh "$OUTDIR"/rootfs/home
cd "$OUTDIR"/rootfs/home
mkdir conf
cp "$FINDER_APP_DIR"/conf/username.txt "$FINDER_APP_DIR"/conf/assignment.txt "$OUTDIR"/rootfs/home/conf

# TODO: Chown the root directory
echo "Creating InitRamFs"
cd "$OUTDIR"/rootfs
# TODO: Create initramfs.cpio.gz
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
cd "$OUTDIR"
gzip -f initramfs.cpio


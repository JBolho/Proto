#!/bin/bash

# Variables

BUILDDIR=$(pwd)
TMPDIR="labyrinth/tmp"
OUTDIR="labyrinth/out"
TCDIR="labyrinth/toolchain/bin"
PREFIX=$( cd $BUILDDIR/$TCDIR/ ; ( ls | grep cpp | sed -e s/cpp// ) )
AKDIR="labyrinth/anykernel2"
AIKDIR="labyrinth/aik"
JOBS=$(nproc --all)
TIMESTAMP=$(date +%Y%m%d%H%M)
CONFIG=labyrinth_defconfig
BUILD=""

# Functions

CLEAN()
{
	echo "Cleaning up...                      "
	make distclean 2> /dev/null > /dev/null
	rm -rf $BUILDDIR/$TMPDIR/
	rm -rf $BUILDDIR/$AKDIR/Image.gz $BUILDDIR/$AKDIR/UPDATE-AnyKernel2.zip
	rm -rf $BUILDDIR/$AIKDIR/split_img/KERNEL.img-zImage
	rm -rf $BUILDDIR/$AIKDIR/image-new.img
	rm -rf $BUILDDIR/$AIKDIR/ramdisk-new.cpio.empty
	exit
}
BUILD()
{
	echo "Building zImage"
	export CROSS_COMPILE="$BUILDDIR/$TCDIR/$PREFIX"
	mkdir $BUILDDIR/$OUTDIR/$TIMESTAMP
	mkdir "$BUILDDIR/$OUTDIR/$TIMESTAMP/info"
	touch $BUILDDIR/$OUTDIR/$TIMESTAMP/info/build.log
	touch $BUILDDIR/$OUTDIR/$TIMESTAMP/info/error.log
	make O=$BUILDDIR/$TMPDIR/ $CONFIG >> $BUILDDIR/$OUTDIR/$TIMESTAMP/info/build.log 2>> $BUILDDIR/$OUTDIR/$TIMESTAMP/info/error.log
	make O=$BUILDDIR/$TMPDIR/ -j$JOBS >> $BUILDDIR/$OUTDIR/$TIMESTAMP/info/build.log 2>> $BUILDDIR/$OUTDIR/$TIMESTAMP/info/error.log
}
CREATE_ZIP()
{
	echo "Creating Flashable zip"
	cp $BUILDDIR/$TMPDIR/arch/arm64/boot/Image.gz $BUILDDIR/$AKDIR/Image.gz
	( cd $BUILDDIR/$AKDIR/ ; zip -q -r9 UPDATE-AnyKernel2.zip * -x README UPDATE-AnyKernel2.zip )
	cp $BUILDDIR/$AKDIR/UPDATE-AnyKernel2.zip $BUILDDIR/$OUTDIR/$TIMESTAMP/labyrinth.zip
}
CREATE_IMG()
{
	echo "Creating Kernel Image"
	cp $BUILDDIR/$TMPDIR/arch/arm64/boot/Image.gz $BUILDDIR/$AIKDIR/split_img/KERNEL.img-zImage
	( cd $BUILDDIR/$AIKDIR/ ; (./repackimg.sh >/dev/null) )
	cp $BUILDDIR/$AIKDIR/image-new.img $BUILDDIR/$OUTDIR/$TIMESTAMP/labyrinth.img
}
REL()
{
	if [ ! -f "$BUILDDIR/$TMPDIR/arch/arm64/boot/Image.gz" ]; then
		echo "Failed! Check $BUILDDIR/$OUTDIR/$TIMESTAMP/error.log"
		return 1
	fi
		case "$BUILD" in
		zip)
			CREATE_ZIP;;		
		img)
			CREATE_IMG;;
		*)
			CREATE_ZIP
			CREATE_IMG;;
	esac
}
DUMP_INFO()
{
	git log > "$BUILDDIR/$OUTDIR/$TIMESTAMP/info/git_log"
	git status > "$BUILDDIR/$OUTDIR/$TIMESTAMP/info/git_status"
	make kernelversion > "$BUILDDIR/$OUTDIR/$TIMESTAMP/info/kernel_version"
}
PROGRESS()
{
	file="$BUILDDIR/$OUTDIR/$TIMESTAMP/info/build.log"
	correct="make[1]"		
	END="195286"
	PRCT="0"
	sleep 1
	while :
	do
		CURR=$( du -b $BUILDDIR/$OUTDIR/$TIMESTAMP/info/build.log 2> /dev/null | cut -f1 )
		PRCT=$(( $CURR * 100 / $END ))
		echo -ne "$PRCT% done\r"
		rip=$( cat $file | tail -n1 | cut -c 1-7 )
		if [[ $rip == $correct ]] || [[ $rip == $rip3 ]]; then
			break
		fi
		rip3=$rip2
		rip2=$rip1
		rip1=$rip
		sleep 1
	done
}

# Actual script

while getopts "b:d:nc" o; do
	case "${o}" in
		b)
			BUILD="$OPTARG";;
		n)
			CLEAN="0";;
		c)
			CLEAN="1";;
		d)
			CONFIG="$OPTARG";;
	esac
done
trap CLEAN SIGINT
if [[ $CLEAN = "0" ]]; then
	BUILD & PROGRESS
	DUMP_INFO
	REL
elif [[ $CLEAN = "1" ]]; then
	CLEAN
else
	BUILD & PROGRESS
	DUMP_INFO
	REL
	CLEAN
fi

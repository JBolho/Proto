#!/bin/bash
# Variables
BUILDDIR=$(pwd)
OUTDIR="labyrinth/out"
RELDIR="labyrinth/release"
TOOLCHAIN="labyrinth/toolchain/bin/aarch64-linux-android-"
AKDIR="labyrinth/anykernel2"
JOBS=$(nproc --all)
LOGS="labyrinth/logs"
TIMESTAMP=$(date +%Y%m%d%H%M)
CONFIG=labyrinth_defconfig
# Functions
CLEAN()
{
echo "Cleaning up..."
make distclean 2> /dev/null > /dev/null
rm -rf $BUILDDIR/$OUTDIR/
rm -rf $BUILDDIR/$AKDIR/Image.gz $BUILDDIR/$AKDIR/UPDATE-AnyKernel2.zip
}
BUILD()
{
echo "Building zImage" 
export CROSS_COMPILE="$BUILDDIR/$TOOLCHAIN"
make O=$BUILDDIR/$OUTDIR/ $CONFIG >> $BUILDDIR/$LOGS/$TIMESTAMP-build.log 2>> $BUILDDIR/$LOGS/$TIMESTAMP-error.log
make O=$BUILDDIR/$OUTDIR/ -j$JOBS >> $BUILDDIR/$LOGS/$TIMESTAMP-build.log 2>> $BUILDDIR/$LOGS/$TIMESTAMP-error.log
}
REL()
{
if [ ! -f "$BUILDDIR/$OUTDIR/arch/arm64/boot/Image.gz" ]; then
	echo "Failed! Check $BUILDDIR/$LOGS/$TIMESTAMP-error.log"
	return 1
fi
if [[ $build = "zImage" ]]; then
	cp $BUILDDIR/$OUTDIR/arch/arm64/boot/Image.gz $BUILDDIR/$RELDIR/$TIMESTAMP-Image.gz
else
	echo "Creating flashable zip"
	cp $BUILDDIR/$OUTDIR/arch/arm64/boot/Image.gz $BUILDDIR/$AKDIR/Image.gz
	( cd $BUILDDIR/$AKDIR/ ; zip -q -r9 UPDATE-AnyKernel2.zip * -x README UPDATE-AnyKernel2.zip )
	cp $BUILDDIR/$AKDIR/UPDATE-AnyKernel2.zip $BUILDDIR/$RELDIR/$TIMESTAMP-labyrinth.zip
fi
}
while getopts "b:dc" o; do
	case "${o}" in
		b)
			build="$OPTARG";;
		d)
			clean="0";;
		c)
			clean="1";;
	esac
done
if [[ $clean = "0" ]]; then
	BUILD
	REL
elif [[ $clean = "1" ]]; then
	CLEAN
else
	BUILD
	REL
	CLEAN
fi

# AnyKernel2 Ramdisk Mod Script
# osm0sis @ xda-developers

## AnyKernel setup
# begin properties
properties() { '
kernel.string=Labyrinth by Dil3mm4 NeonDragon1909 @ xda-developers
do.devicecheck=0
do.modules=0
do.cleanup=1
do.cleanuponabort=0
device.name1=hi6250
'; } # end properties

# shell variables
block=/dev/block/bootdevice/by-name/kernel;
is_slot_device=0;
ramdisk_compression=auto;


## AnyKernel methods (DO NOT CHANGE)
# import patching functions/variables - see for reference
. /tmp/anykernel/tools/ak2-core.sh;


## AnyKernel file attributes


## AnyKernel install
split_boot;

flash_boot;

## end install


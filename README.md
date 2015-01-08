From readme:

Kernel Build  
  - Uncompress using following command at the android directory
  - When you compile the kernel source code, you have to add google original prebuilt source(toolchain) into the android directory.
  - Run following scripts to build kernel
    a) cd kernel
    c) make ARCH=arm CROSS_COMPILE=../prebuilts/gcc/linux-x86/arm/arm-eabi-4.7/bin/arm-eabi- f70n_global_com_defconfig zImage
  - After build, you can find the build image(zImage) at arch/arm/boot
  
  NOTE: This is official source kernel from LG dev for D315n (LG F70) device

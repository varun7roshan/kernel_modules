######################################################################################
- "tldr" gives summary of man page, and also quick commands, better than "man"
sudo apt-get install tldr
- Example: tldr ps;

######################################################################################
- "Linux Tracing Toolkit next generation" - LTTng
- A superb tool for "tracing" and "pofiling" the linux kernel
- Allows to trace user-space applications and kernel code paths, like performance
- The IRQ hits vs time can be seen with a nice GUI

######################################################################################
- "procmap" utility
- Visualizing the complete memory map of the "Kernel Virtual Address Space", as well as any
given processes user VAS is what "procmap" utility is designed to do.
- Note : This has kernel component as well, works on x86_64, AArch32 and AArch64 CPUs
- can be downloaded from : https://github.com/kaiwan/procmap

######################################################################################
- "Simple Embedded ARM Linux System FOSS project"
- SEALS (Simple Embedded ARM Linux System) - is very simple "Skeleton" linux base system
running on an emulated ARM machine.
- It compiles kernel and also creates a simple RFS for us to use
- we can then use it with QEMU - qemu-system-arm to emulate and run on ARM platform
- The "Versatile Express CA-9" is the default board emulated
- https://github.com/kaiwan/seals/
- https://github.com/kaiwan/seals/wiki

######################################################################################
- "The LDV - Linux Driver Verification" project
- This is test suite to verify static and dynamic analysis on the kernel, and driver code
- http://linuxtesting.org/
######################################################################################
														[Kernel Build Process]
######################################################################################
- Any Linux System needs to have minimum 3 components:
1. Bootloader
2. Kernel / OS
3. RFS

######################################################################################
- To check the running kernel version:
uname -r
uname -a

5.15.0-113-generic

major#.minor#[.patchlevel][-EXTRAVERSION]
>> The "[]" square brackets for patchlevel and EXTRAVERSION indicate that its optinal

- So from above, 5(Major) . 15(minor)  .  0(patchlevel)   -113-generic

- When we configure the kernel, inorder for us to track - we can change the -EXTRAVERSION
- which can also be called as "localversion" here, where an organization can track releases

######################################################################################
- Kernel Development Workflow

- 

######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################


######################################################################################



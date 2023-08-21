###########################################################################################
Raspberry Pi 3B V1.2
SoC : BCM2837

- The underlying architecture of the BCM2837 is identical to the BCM2836/BCM2835. 
>> We have datasheet of BCM2835

- The only significant difference is the replacement of the ARMv7 quad core cluster 
with a quad-core ARM Cortex A53 (ARMv8) cluster.

- The ARM cores run at 1.2GHz, making the device about 50% faster than the Raspberry Pi 2. 
-The VideoCore IV runs at 400MHz.

- Please refer to the following BCM2836 document for details on the ARM peripherals 
specification, which also applies to the BCM2837.

BCM2836 ARM-local peripherals

Cortex-A53 MPCore Processor Technical Reference Manual

###########################################################################################
Raspberry Pi Zero, Raspberry Pi Zero W
SoC : BCM2835
- It contains a single-core ARM1176JZF-S processor
- GPU Documentation : VideoCoreIV-AG100-R

pi@pi:~$ lscpu
Architecture:        armv6l
Byte Order:          Little Endian
CPU(s):              1
On-line CPU(s) list: 0
Thread(s) per core:  1
Core(s) per socket:  1
Socket(s):           1
Vendor ID:           ARM
Model:               7
Model name:          ARM1176
Stepping:            r0p7
CPU max MHz:         1000.0000
CPU min MHz:         700.0000
BogoMIPS:            997.08
Flags:               half thumb fastmult vfp edsp java tls

###########################################################################################
- will be using "Raspberry Pi Zero W" for the this bare metal
- SoC : BCM2835
- Its Peripheral Physical Memory Address starts from : 0x20000000
>> This is as per datasheet 
- Peripherals (at physical address 0x20000000 on) are mapped into the kernel virtual address
space starting at address 0xF2000000. Thus a peripheral advertised here at bus address
0x7Ennnnnn is available in the ARM kenel at virtual address 0xF2nnnnnn


- to double confirm, check the "bcm2835.dtsi" 
soc {
		ranges = <0x7e000000 0x20000000     0x02000000>;
						 <Bus-Address Physical-Addr size(may be)>
		dma-ranges = <0x40000000 0x00000000 0x20000000>;
	};

------------------------------------------------------------------------------
- To cross-verify if we have read the right numbers check "bcm2837.dtsi"
- This BCM2837 i have used in Pi-3B for led verification and used 0x3F000000 as base
peripheral physical address to offset into /dev/mem
soc {
		ranges = <0x7e000000 0x3f000000 0x1000000>,
						 <Bus-Address Physical   size-may be>
			 <0x40000000 0x40000000 0x00001000>;
		dma-ranges = <0xc0000000 0x00000000 0x3f000000>;
}
- Also from datasheet :
Peripherals (at physical address 0x3F000000 on) are mapped into the kernel virtual address
space starting at address 0xF2000000. Thus a peripheral advertised here at bus address
0x7Ennnnnn is available in the ARM kenel at virtual address 0xF2nnnnnn.

###########################################################################################
Going through the datasheet:

- We can access all the peripherals from ARM

- There are a number of peripherals which are intended to be controlled by the GPU. 
- These are omitted from this datasheet. 
- Accessing these peripherals from the ARM is not recommended.
>> GPU Datasheet : VideoCoreIV-AG100-R

- In addition to the ARM’s MMU, BCM2835 includes a second coarse-grained MMU for
mapping ARM physical addresses onto system bus addresses.
>> So the when we write to the Physical Address peripheral register like 0x20000000
it actuall gets converted to 0x7e000000 Bus Address and writes to the peripheral register

- So we have 2 MMUs:
1. ARM MMU : used for Virtual Address to Physical Address translation
2. VC MMU  : used to further translate Physical Address to Bus Addess
>> This VC MMU seems to be hardcoded

###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################

###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################

###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


###########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################
I2C Tutorial
Introduction

- I2C is a serial protocol for a two-wire interface to connect low-speed devices like 
microcontrollers, EEPROMs, A/D and D/A converters, I/O interfaces, and other similar 
peripherals in embedded systems. 
- It was invented by Philips and now it is used by almost all major IC manufacturers. 
- Each I2C slave device needs an address – they must still be obtained from NXP

- The I2C bus is popular because it is simple to use, there can be more than one master, 
only upper bus speed is defined and only two wires with pull-up resistors are needed to 
connect an almost unlimited number of I2C devices. 
- I2C can use even slower microcontrollers with general-purpose I/O pins since they only 
need to generate correct Start and Stop conditions in addition to functions for reading 
and writing a byte.

- Each slave device has a unique address. Transfer from and to the master device is 
serial and it is split into 8-bit packets. 
- All these simple requirements make it very simple to implement the I2C interface even 
with cheap microcontrollers that have no special I2C hardware controller. 
- You only need 2 free I/O pins and few simple i2C routines to send and receive commands.

- I2C bus specification describes four operating speed categories for bidirectional 
data transfer:

Standard-mode (Sm)	
a bit rate up to 100 kbit/s

Fast-mode (Fm)	
a bit rate up to 400 kbit/s

Fast-mode Plus (Fm+)	
a bit rate up to 1 Mbit/s

High-speed mode (Hs-mode)	
a bit rate up to 3.4 Mbit/s

Ultra-fast mode (UFm)	
a bit rate up to 5 Mbit/s
-------------------------------------------------------------------------------------------
I²C Interface
I2C uses only two wires:

SCL (serial clock)
SDA (serial data)

- Both need to be pulled up with a resistor to +Vdd. There are also I2C level shifters 
that can be used to connect to two I2C buses with different voltages.
-------------------------------------------------------------------------------------------
I²C Address

- Basic I2C communication is using transfers of 8 bits or bytes. 
- Each I2C slave device has a 7-bit address that needs to be unique on the bus. 
- Some devices have fixed I2C addresses while others have few address lines which 
determine lower bits of the I2C address. 
- This makes it very easy to have all I2C devices on the bus with a unique I2C address. 
- There are also devices that have a 10-bit address as allowed by the specification.

- The 7-bit address represents bits 7 to 1 while bit 0 is used to signal reading from or 
writing to the device. 
- If bit 0 (in the address byte) is set to 1 then the master device will read from the 
slave I2C device.

- The master device needs no address since it generates the clock (via SCL) and addresses 
individual I2C slave devices.
##########################################################################################
I²C Protocol

- In a normal state, both lines (SCL and SDA) are high. 
- The communication is initiated by the master device. 
- It generates the Start condition (S) followed by the address of the slave device (B1). 
- If bit 0 of the address byte was set to 0 the master device will write to the slave 
device (B2). 
- Otherwise, the next byte will be read from the slave device. 
- Once all bytes are read or written (Bn) the master device generates Stop condition (P). 
- This signals to other devices on the bus that the communication has ended and another 
device may use the bus.

- Most I2C devices support repeated start conditions. This means that before the 
communication ends with a stop condition, the master device can repeat the start 
condition with the address byte and change the mode from writing to reading.
-------------------------------------------------------------------------------------------
Conclusion

- The I2C bus is used by many integrated circuits and is simple to implement. 
- Any microcontroller can communicate with I2C devices even if it has no special I2C 
interface. 
- I2C specifications are flexible – I2C buses can communicate with slow devices and can 
also use high-speed modes to transfer large amounts of data. 
- Because of many advantages, the I2C bus will remain one of the most popular serial 
interfaces to connect integrated circuits on the board. 

##########################################################################################
Cont ( I2C Tutorial )…

- The I2C Protocol bus uses two wires: serial data (SDA) and serial clock (SCL). 
- All I2C master and slave devices are connected with only those two wires. 
- Each device can be a transmitter, a receiver, or both. 
- Some devices are masters – they generate bus clocks and initiate communication on the 
bus, other devices are slaves and respond to the commands on the bus. 
- In order to communicate with a specific device, each slave device must have an address 
that is unique on the bus. 
- I2C master devices (usual microcontrollers) don’t need an address since no other 
(slave) device sends commands to the master.
-------------------------------------------------------------------------------------------
I2C terminology

Transmitter – This is the device that transmits data to the bus

Receiver – This is the device that receives data from the bus

Master – This is the device that generates clock, starts communication, 
sends I2C commands, and stops communication

Slave – This is the device that listens to the bus and is addressed by the master

Multi-master – I2C can have more than one master and each can send commands

Arbitration – A process to determine which of the masters on the bus can use it 
when more masters need to use the bus

Synchronization – A process to synchronize clocks of two or more devices
-------------------------------------------------------------------------------------------
I2C Protocol

Bus Signals
- Both signals (SCL and SDA) are bidirectional. They are connected via resistors to a 
positive power supply voltage. 
- This means that when the bus is free, both lines are high. 
- All devices on the bus must have an open-collector or open-drain pins. 
- Activating the line means pulling it down (wired AND). 
- The number of devices on a single bus is almost unlimited – the only requirement is 
that the bus capacitance does not exceed 400 pF. 
- Because logical 1 level depends on the supply voltage, there is no standard bus voltage.
-------------------------------------------------------------------------------------------
Serial Data Transfer

- For each clock pulse, one bit of data is transferred. 
- The SDA signal can only change when the SCL signal is low – when the clock is high the 
data should be stable.
-------------------------------------------------------------------------------------------
Start and Stop Condition

- Each I2C command initiated by the master device starts with a START condition and ends 
with a STOP condition. 
- For both conditions, SCL has to be high. 
- A high to low transition of SDA is considered as START and a low to high transition as 
STOP.
-------------------------------------------------------------------------------------------
- After the Start condition, the bus is considered as busy and can be used by another 
master only after a Stop condition is detected. 
- After the Start condition, the master can generate a repeated Start. 
- This is equivalent to a normal Start and is usually followed by the slave I2C address.
-------------------------------------------------------------------------------------------
- Microcontrollers that have dedicated I2C hardware can easily detect bus changes and 
behave also as I2C slave devices. 
- However, if the I2C communication is implemented in software, the bus signals must be 
sampled at least two times per clock cycle in order to detect necessary changes.
-------------------------------------------------------------------------------------------
I2C Data Transfer

- Data on the I2C Protocol bus is transferred in 8-bit packets (bytes). 
- There is no limitation on the number of bytes, however, each byte must be followed by an 
Acknowledge bit. 
- This bit signals whether the device is ready to proceed with the next byte. 
- For all data bits including the Acknowledge bit, the master must generate clock pulses. 
- If the slave device does not acknowledges transfer this means that there is no more data 
or the device is not ready for the transfer yet. 
- The master device must either generate the Stop or Repeated Start condition.
-------------------------------------------------------------------------------------------
Synchronization

- Each master must generate its own clock signal and the data can change only when the 
clock is low. 
- For successful bus arbitration, a synchronized clock is needed. 
- Once a master pulls the clock low it stays low until all masters put the clock into a 
high state. 
- Similarly, the clock is in a high state until the first master pulls it low. 
- This way by observing the SCL signal, master devices can synchronize their clocks.
-------------------------------------------------------------------------------------------
Clock Stretching

- As we told you earlier, the master device determines the clock speed. 
- However, there are situations where an I2C slave is not able to co-operate with the 
clock speed given by the master and needs to slow down a little. 
- This is done by a mechanism called Clock Stretching.

- An I2C slave is allowed to hold down the clock if it needs to reduce the bus speed. 
- The master, on the other hand, is required to read back the clock signal after releasing 
it to the high state and wait until the line has actually gone high.
-------------------------------------------------------------------------------------------
Arbitration

- For normal data transfer on the I2C bus, only one master can be active. 
- If for some reason two masters initiate the I2C command at the same time, 
the arbitration procedure determines which master wins and can continue with the command. 
- Arbitration is performed on the SDA signal while the SCL signal is high. 
- Each master checks if the SDA signal on the bus corresponds to the generated SDA signal. 
- If the SDA signal on the bus is low but it should be high, then this master has lost 
arbitration. 
- Master I2C device that has lost arbitration can generate SCL pulses until the byte ends 
and must then release the bus and go into slave mode. 
- The arbitration procedure can continue until all the data is transferred. 
- This means that in a multi-master system each I2C master must monitor the I2C bus for 
collisions and act accordingly.
-------------------------------------------------------------------------------------------
Clock Synchronization and Handshaking

- Slave devices that need some time to process the received byte or are not ready yet to 
send the next byte, can pull the clock low to signal to the master that it should wait. 
- Once the clock is released the master can proceed with the next byte.
-------------------------------------------------------------------------------------------
Communication With 7-bit I2C Addresses

- Each slave device on the bus should have a unique 7-bit address. 
- The communication starts with the Start condition, followed by the 7-bit slave address 
and the data direction bit. 
- If this bit is 0 then the master will write to the slave device. 
- Otherwise, if the data direction bit is 1, the master will read from the slave device. 
- After the slave address and the data direction is sent, the master can continue with 
reading or writing. 
- The communication is ended with the Stop condition which also signals that the I2C bus 
is free. 
- If the master needs to communicate with other slaves it can generate a repeated start 
with another slave address without the generation Stop condition. 
- "All the bytes are transferred with the MSB bit shifted first"

- If the master only writes to the slave device then the data transfer direction is not 
changed.
- If the master only needs to read from the slave device then it simply sends the I2C 
address with the R/W bit set to read. 
- After this, the master device starts reading the data.
-------------------------------------------------------------------------------------------
- Sometimes the master needs to write some data and then read from the slave device. 
- In such cases it must first write to the slave device, change the data transfer 
direction and then read the device. 
- This means sending the I2C address with the R/W bit set to write and then sending some 
additional data like the register address. 
- After the writing is finished the master device generates repeated start condition and 
sends the I2C address with the R/W bit set to read. 
- After this, the data transfer direction is changed and the master device starts 
reading the data.

-------------------------------------------------------------------------------------------
7-bit I2C Addressing

- A slave address may contain a fixed and a programmable part. 
- Some slave devices have few bits of the I2C address dependent on the level of address 
pins. 
- This way it is possible to have on the same I2C bus more than one I2C device with the 
same fixed part of the I2C address.
-------------------------------------------------------------------------------------------
- The allocation of I2C addresses is administered by the I2C bus committee which takes 
care of the allocations. 
- Two groups of 8 I2C addresses are reserved for future uses and one address is used for 
10-bit I2C addressing.
-------------------------------------------------------------------------------------------
I2C Protocol

- The general call address is used to address all devices on the slave bus. 
- If any slave device doesn’t need to respond to such a call or a general call is not 
supported by the slave device, the call must be ignored. 
- If the device supports general calls and wants to receive the data it must acknowledge 
the address and read the data as a slave receiver.
-------------------------------------------------------------------------------------------
Start Byte

- If the microcontroller has I2C hardware and the microcontroller acts as a slave then the 
software needs to do nothing to check the bus state. 
- The I2C hardware will detect the Start condition, receive the I2C address, and 
interrupt the software if necessary. 
- However, if the I2C interface is implemented by the software, the microcontroller has to 
sample the SDA line at least twice per clock pulse in order to detect changes. 
- To simplify the detection of I2C commands on the bus in such cases, a special I2C 
address called Start byte is used. 
- Such a start byte (0000 0001) is followed by an acknowledge pulse (for interface 
compatibility reasons). 
- This combination holds the SDA line low for 7 clock pulses and allows simple detection 
of active I2C buses with lower sampling frequency.
-------------------------------------------------------------------------------------------
Extension of the I2C Specifications

- The standard mode of the I2C Protocol bus uses transfer rates up to 100 kbit/s and 
7-bit addressing. 
- Such an I2C interface is used by many hundred I2C Protocol-compatible devices from many 
manufacturers since its introduction in the 80s. 
- However, with the advance of technology, needs for higher transfer rates and larger 
address space emerged. 
- There are cases where a large amount of data needs to be transferred. 
- Many complex embedded boards contain a large number of different I2C Protocol devices. 
- In some cases, it is very hard to avoid address collisions since 7 bits for I2C 
addresses allow only 127 different addresses where only 112 can actually be used. 
- Some I2C devices on the board, despite address pins, have the same address. 
This resulted in few upgrades to the standard-mode I2C specifications:
-------------------------------------------------------------------------------------------
Fast Mode – supports transfer rates up to 400 kbit/s
High-speed mode (Hs-mode) – supports transfer rates up to 3.4 Mbit/s
10-bit addressing – supports up to 1024 I2C addresses

- There can be any combination of the devices on the bus regardless of the supported speed 
and addressing. 
- Fast mode devices are downward-compatible and can work with slower I2C controllers. 
- However, most modern I2C controllers support all speeds and addressing modes.

- The high-speed mode uses signals called SCLH and SDAH to emphasize the higher speed. 
- These signals are usually separated from standard SDA and SCL lines.
- High-speed mode introduces also few differences (or improvements) in the specifications:

Improved data and clock line output drivers
- Schmitt trigger and spike suppression circuits on data and clock inputs
- Clock synchronization and arbitration is not used
- Clock signal has 1 to 2 high/low ratio
- 10-bit I2C Addressing

- 10-bit addressing can be used together with 7-bit addressing since a special 7-bit 
address (1111 0XX) is used to signal a 10-bit I2C address. 
- When a master wants to address a slave device using 10-bit addressing, it generates a 
start condition, then it sends 5 bits signaling 10-bit addressing (1111 0), 
followed by the first two bits of the I2C address and then the standard read/write bit.
-------------------------------------------------------------------------------------------
- If the master will write data to the slave device it must send the remaining 8 bits of 
slave address as the second byte.

- If the master will read data from the slave device it must send the complete 10-bit 
address (two bytes) as for writing, then a repeated start is sent followed by the first 
address byte with read/write bit set to high to signal reading. 
- After this procedure, the data can be read from the slave device.
-------------------------------------------------------------------------------------------
- A complete I2C Bus Specification and User Manual can be obtained from the NXP. 
##########################################################################################
I2C Linux Device Driver

Prerequisites
- I2C Introduction – Part 1 (Basics)
- I2C Introduction – Part 2 (Advanced Topics)

Hardware Required
- Raspberry Pi
- SSD1306 OLED I2C Display

Bring up Raspberry PI
- Install Raspberry Pi OS (32-bit) with desktop in the SD card.
- Then install the kernel header using sudo apt install raspberrypi-kernel-headers
- For your information, In our Raspberry PI 4 board, kernel 5.4.51-v7l+ is installed.

Please enable the I2C in the Raspberry Pi.
-------------------------------------------------------------------------------------------
I2C

- I assume that you know about the I2C and how the master and slave are communicating 
normally. If you don’t know about I2C, please go through the I2C’s part 1 and part 2 
tutorials before beginning. In this tutorial, we will focus on how the Linux kernel 
is handling the I2C.
-------------------------------------------------------------------------------------------
I2C Subsystem

- The kernel divided the I2C subsystem by 
1. Buses and 
2. Devices

- The Buses are again divided into 
1. Algorithms and 
2. Adapters 

- The devices are again divided into 
1. Drivers and 
2. Clients 

- The below image will give you some understandings.
-------------------------------------------------------------------------------------------
I2C SUBSYSTEM I2C Linux Device Driver

Algorithm:
- An Algorithm driver contains a general code that can be used for a whole class of 
I2C adapters.
-------------------------------------------------------------------------------------------
Adapters:
- An Adapter effectively represents a "bus – Channel Number"
>> If SoC has 2 I2C channels - we can refer either one 0 or 1 - this becomes Adapter num
- it is used to tie up a particular I2C with an algorithm and bus number
- Each specific adapter driver either depends on one algorithm driver or includes 
its own implementation.
-------------------------------------------------------------------------------------------
Clients:
- A Client represents a chip (slave) on the I2C
-------------------------------------------------------------------------------------------
Drivers:
- This is the driver that we are writing for the client
-------------------------------------------------------------------------------------------
- Usually, Driver and Client are more closely integrated than Algorithm and Adapter.

- So, you will need a driver for your I2C bus, and drivers for your I2C devices 
(usually one driver for each device).

>> So we have 2 types of Drivers in I2C Sub-system:
1. Client / Slave Drivers - which we write for each I2C Device
2. Bus Drivers / Adapter Drivers - Which sets register for the particular I2C channel
>> Since we have 2 I2C channels on Raspberry Pi - we will have 2 instances of Adapter
Driver "?"

3. Algorithm Drivers - which the Bus Drivers can use generic code to set register of
I2C channels
-------------------------------------------------------------------------------------------
I2C Driver in Linux Kernel

- Steps that involve while writing the I2C device driver are given below.

1. Get the I2C adapter - either the I2C Channel Number (Bus Number)

2. Create the oled_i2c_board_info structure and create a device using that.
>> This Board info will have : NAME, ADDR - and will be used in creating a New I2C Slave
device - sitting on a Adapter/Bus_Number

3. Create the i2c_device_id for your slave device and register that
>> This Device ID - is used for Matching the "Device" with "Driver" - so that probe()
may be invoked

4. Create the i2c_driver structure and add that to the I2C subsystem.
>> This "i2c_driver" will have a "Device ID" - which will be used for matching the
registered "Device" using Device_ID and with this "Driver" using Device_ID

5. Now the driver is ready. So you can transfer the data between master and slave.
>> probe() will be invoked for the matching DEVICE_IDs - for "Device" and "Driver"

6. Once you are done, then remove the device.
-------------------------------------------------------------------------------------------
Get the I2C adapter
- In raspberry Pi 4, the i2c-1 bus is available already. 
- You can check that using the command ls -al /sys/bus/i2c/devices/
- So we will use the below API to get the adapter structure of this I2C bus.

struct i2c_adapter *i2c_get_adapter(int nr);

nr – I2C bus number. In our case (Raspberry Pi 4), it should be 1.

It returns the struct i2c_adapter

>> Here we are getting the "Adapter" meaing a refrence to the "I2C Bus Number"
>> So that we can use this Refrence to "Adapter" - to register "New I2C Salve Device"
sitting on this "Bus Number"/"Adapter"
>> This is how we tell which "Slave Device" is sitting on which "Adapter"/"I2C Bus Number"
-------------------------------------------------------------------------------------------

Create the board info
- Once you get the adapter structure, 
- then create the board info and
>> This board info will have "NAME" and Address - so that - this "NAME" will be used
in searching - for the matched "device" and "driver" - to get the ADDRESS of the Slave
device - so that "probe()" will be invoked passing the "struct i2c_client" with ADDR
- using that board info, create the device.

Create Board info
- Just create the i2c_board_info  structure and assign required members of that.

struct i2c_board_info 
{
  char type[I2C_NAME_SIZE];
  unsigned short flags;
  unsigned short addr;
  void * platform_data;
  struct dev_archdata * archdata;
  struct device_node * of_node;
  struct fwnode_handle * fwnode;
  int irq;
};

type[I2C_NAME_SIZE] – chip type, to initialize i2c_client.name
flags – to initialize i2c_client.flags
addr  – stored in i2c_client.addr
platform_data – stored in i2c_client.dev.platform_data
archdata – copied into i2c_client.dev.archdata
of_node – pointer to OpenFirmware device node
fwnode – device node supplied by the platform firmware
irq – stored in i2c_client.irq

- You can use I2C_BOARD_INFO macro to initialize the essential fields of 
struct i2c_board_info.

I2C_BOARD_INFO ( dev_type, dev_addr);

dev_type – identifies the device type
dev_addr – the device’s address on the bus
-------------------------------------------------------------------------------------------
Create Device
- Now board info structure is ready. Let’s instantiate the device from that I2C bus.
>> At this point we have got : a refrence to "Adapter" - now we need to inform which
I2C Slave Device is sitting on this Adapter/Bus_Number - and when we inform which slave
device is sitting - we need to pass the "NAME" of the Slave Device - which will be used 
in Matching <Device ID> <Driver> <I2C Client Device> - to get the ADDRESS of this
Slave device - before invoking probe()

struct i2c_client * i2c_new_device ( struct i2c_adapter * adap, 
																		 struct i2c_board_info const * info);

*adap – Adapter structure that we got from i2c_get_adapter()
*info – Board info structure that we have created

- This will return the i2c_client structure. We can use this client for our future 
transfers.

Note: If you are using the newer kernel (5.2 =<), then you must use the 
"i2c_new_client_device()" API instead of "i2c_new_device()"
-------------------------------------------------------------------------------------------
- Now we will see the example for this section. 
- So that you will get some idea that how we are using this in our code.
-------------------------------------------------------------------------------------------
Example
#define I2C_BUS_AVAILABLE   (          1 )              // I2C Bus available in our Raspberry Pi
#define SLAVE_DEVICE_NAME   ( "ETX_OLED" )              // Device and Driver Name
#define SSD1306_SLAVE_ADDR  (       0x3C )              // SSD1306 OLED Slave Address

static struct i2c_adapter *etx_i2c_adapter     = NULL;  // I2C Adapter Structure
static struct i2c_client  *etx_i2c_client_oled = NULL;  // I2C Cient Structure (In our case it is OLED)

/*
** I2C Board Info strucutre
*/
static struct i2c_board_info oled_i2c_board_info = 
{
	I2C_BOARD_INFO(SLAVE_DEVICE_NAME, SSD1306_SLAVE_ADDR)
};

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
    ....
    
    etx_i2c_adapter     = i2c_get_adapter(I2C_BUS_AVAILABLE);
    etx_i2c_client_oled = i2c_new_device(etx_i2c_adapter, &oled_i2c_board_info);
    
    ....
    return 0;
}
-------------------------------------------------------------------------------------------
Create the device id and register
- Now we have to create the i2c driver for our slave. 
- In order to do that, you have to create the device id and i2c_driver. 
- Then add that driver to the I2C subsystem.
-------------------------------------------------------------------------------------------
Create the device id
- Just create the structure i2c_device_id and initialize the necessary members.
>> This Device ID - is mainly used to Match the "Device" with "Driver"
>> This "ID-Matching" - is using "NAME" only

struct i2c_device_id 
{
  char name[I2C_NAME_SIZE];
  kernel_ulong_t driver_data;
};
name –  Slave name
driver_data – Data private to the driver (This data will be passed to the respective driver)

MODULE_DEVICE_TABLE(i2c, my_id_table)
>> This will inform the "MODULE DEVICE TABLE" - about the "Device - NAME" sitting on the
i2c bus
>> And when a Driver of this ID "NAME" is found on I2C - it is matched and drivers
probe() will be invoked
>> Before invoking - it will get the SAVE ADDRESS, which is already registered with
I2C Core - using the same "NAME"

- After this, call MODULE_DEVICE_TABLE(i2c, my_id_table) in order to expose the driver 
along with its I2C device table IDs to userspace.
-------------------------------------------------------------------------------------------
Create the i2c_driver

struct i2c_driver 
{
  unsigned int class;
  int (* attach_adapter) (struct i2c_adapter *);
  int (* probe) (struct i2c_client *, const struct i2c_device_id *);
  int (* remove) (struct i2c_client *);
  void (* shutdown) (struct i2c_client *);
  void (* alert) (struct i2c_client *, unsigned int data);
  int (* command) (struct i2c_client *client, unsigned int cmd, void *arg);
  struct device_driver driver;
  const struct i2c_device_id * id_table;
  int (* detect) (struct i2c_client *, struct i2c_board_info *);
  const unsigned short * address_list;
  struct list_head clients;
};

class – What kind of i2c device we instantiate (for detect)
attach_adapter – Callback for bus addition (deprecated)
probe – Callback for device binding
remove – Callback for device unbinding
shutdown – Callback for device shutdown
alert – Alert callback, for example for the SMBus alert protocol
command – Callback for bus-wide signaling (optional)
driver – Device driver model driver
id_table – List of I2C devices supported by this driver
detect – Callback for device detection
address_list – The I2C addresses to probe (for detect)
clients – List of detected clients we created (for i2c-core use only)
-------------------------------------------------------------------------------------------
Add the I2C driver to the I2C subsystem

- Now we have the i2c_driver structure. 
- So we can add this structure to the I2C subsystem using the below API.

i2c_add_driver(struct i2c_driver *i2c_drive);

i2c_drive – The i2c_driver structure that we have created.

- During the call to i2c_add_driver to register the I2C driver, all the I2C devices 
will be traversed. 

- "Once matched, the probe function of the driver will be executed"
-------------------------------------------------------------------------------------------
You can remove the driver using 

i2c_del_driver(struct i2c_driver *i2c_drive).
-------------------------------------------------------------------------------------------
Let’s put this together and the code snippet is shown below.

Example
/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int etx_oled_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    ...
    
    return 0;
}

/*
** This function getting called when the slave has been removed
** Note : This will be called only once when we unload the driver.
*/
static int etx_oled_remove(struct i2c_client *client)
{   
    ...
    return 0;
}

/*
** Structure that has slave device id
*/
static const struct i2c_device_id etx_oled_id[] = {
        { SLAVE_DEVICE_NAME, 0 }, /*name, driver_data*/
        { }
};
MODULE_DEVICE_TABLE(i2c, etx_oled_id);

/*
** I2C driver Structure that has to be added to linux
*/
static struct i2c_driver etx_oled_driver = 
{
        .driver = {
            .name   = SLAVE_DEVICE_NAME,
            .owner  = THIS_MODULE,
        },
        .probe          = etx_oled_probe,
        .remove         = etx_oled_remove,
        .id_table       = etx_oled_id,
};

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
    ...
    i2c_add_driver(&etx_oled_driver);
    ...
    return 0;
}
-------------------------------------------------------------------------------------------
Transfer data

- Till this point, everything is on our plate. So, we can start the communication 
between master and slave. I meant data transfer.
-------------------------------------------------------------------------------------------
Send data

i2c_master_send
- This API issue a single I2C message in the master transmit mode.

int i2c_master_send ( const struct i2c_client * client, const char * buf, int count);

client – Handle to the slave device
buf – Data that will be written to the slave
count – How many bytes to write, must be less than 64k since msg length is u16

It returns negative errno, or else the number of bytes written.
-------------------------------------------------------------------------------------------
i2c_smbus_write_byte

- This API is used to send one byte to the slave.

s32 i2c_smbus_write_byte ( const struct i2c_client * client, u8 value);

client – Handle to the slave device
value – Byte to be sent

It returning negative errno else zero on success.
-------------------------------------------------------------------------------------------
i2c_smbus_write_byte_data

s32 i2c_smbus_write_byte_data ( const struct i2c_client * client, u8 command, u8 value);

client – Handle to the slave device
command – Byte interpreted by slave
value – Byte being written

It returning negative errno else zero on success.
-------------------------------------------------------------------------------------------
i2c_smbus_write_word_data

s32 i2c_smbus_write_word_data ( const struct i2c_client * client, u8 command, u16 value);

client – Handle to the slave device
command – Byte interpreted by slave
value – 16-bit “word” being written

It returning negative errno else zero on success.
-------------------------------------------------------------------------------------------
i2c_smbus_write_block_data

s32 i2c_smbus_write_block_data ( const struct i2c_client * client, u8 command, u8 length, const u8 * values);

client – Handle to the slave device
command – Byte interpreted by slave
length – Size of the data block; SMBus allows at most 32 bytes
values – Byte array which will be written

It returns negative errno else zero on success.
-------------------------------------------------------------------------------------------
Read data

i2c_master_recv
- This API issue a single I2C message in master receive mode.

int i2c_master_recv ( const struct i2c_client * client, const char * buf, int count);

client – Handle to the slave device
buf – Data that will be read from the slave
count – How many bytes to read, must be less than 64k since msg length is u16

It returns negative errno, or else the number of bytes reads.
-------------------------------------------------------------------------------------------
i2c_smbus_read_byte

s32 i2c_smbus_read_byte ( const struct i2c_client * client);

client – Handle to the slave device

It is returning negative errno else the byte received from the device.
-------------------------------------------------------------------------------------------
i2c_smbus_read_byte_data

s32 i2c_smbus_read_byte_data ( const struct i2c_client * client, u8 command);

client – Handle to the slave device
command – Byte interpreted by slave

It is returning negative errno else a data byte received from the device.
-------------------------------------------------------------------------------------------
i2c_smbus_read_word_data

s32 i2c_smbus_read_word_data ( const struct i2c_client * client, u8 command);

client – Handle to the slave device
command – Byte interpreted by slave

This returns negative errno else a 16-bit unsigned “word” received from the device.
-------------------------------------------------------------------------------------------
i2c_smbus_read_block_data

s32 i2c_smbus_read_block_data ( const struct i2c_client * client, u8 command, u8 * values);

client – Handle to the slave device
command – Byte interpreted by slave
values – Byte array into which data will be read; big enough to hold the data returned 
by the slave. SMBus allows at most 32 bytes.

This returns negative errno else the number of data bytes in the slave’s response.

- Note that using this function requires that the client’s adapter support the 
I2C_FUNC_SMBUS_READ_BLOCK_DATA functionality. 
- Not all adapter drivers support this; its emulation through I2C messaging relies on a 
specific mechanism (I2C_M_RECV_LEN) which may not be implemented.
-------------------------------------------------------------------------------------------
i2c_transfer

- If you want to send multiple I2C messages then you can use the below-given API.

int i2c_transfer ( struct i2c_adapter * adap, struct i2c_msg * msgs, int num);

adap – Handle to the I2C bus
msgs – One or more messages to execute before STOP is issued to terminate the operation; 
each message begins with a START.
num – Number of messages to be executed.

It returns negative errno, else the number of messages executed.
-------------------------------------------------------------------------------------------

												"How I2C bus driver works"

- I2C client driver initiates transfer using a function like i2c_transfer, 
i2c_master_send etc.
- It comes to the "master_xfer()" function in the "bus driver" (drivers/i2c/busses/* */)

- The "bus driver" splits the entire transaction into 
START, STOP, ADDRESS, READ with ACK, READ with NACK, etc. 

- These conditions have to be created on the real i2c bus. 

- The bus driver writes to the I2C hardware adaptor to generate these conditions on the 
I2C bus one by one, "sleeping on a wait queue in between" (basically giving the CPU to 
some other task to do some useful job rather than polling until hardware finishes).

- Once the hardware has finished a transaction on the bus (for eg a START condition), 
an interrupt will be generated and the ISR will wake up the sleeping master_xfer.

- Once master_xfer wakes up, he will go and advise the hardware adaptor to send the 
second condition (for eg ADDRESS of the chip).

- This continues till the whole transaction is over and return back to the client driver.

- The point to note here is "sleep done by the thread" in between each condition. 

- ***** This is why I2C transactions cannot be used in ISRs *******************************

- For client driver, it is just a simple function like i2c_transfer, i2c_master_send.
- But it is implemented in the bus driver as explained above.
-------------------------------------------------------------------------------------------
SSD1306 OLED

- SSD1306 is a single-chip CMOS OLED/PLED driver with a controller for an 
organic / polymer light-emitting diode dot-matrix graphic display system. 
It consists of 128 segments and 64commons.

- The SSD1306 embeds with contrast control, display RAM, and oscillator, which reduces the 
number of external components and power consumption. 
- It has 256-step brightness control. 
- Data/Commands are sent from general MCU through the hardware selectable 6800/8000 
series compatible Parallel Interface, I2C interface, or Serial Peripheral Interface. 
- It is suitable for many compact portable applications, such as mobile phone sub-display, 
MP3 player and calculator, etc.

- The operating voltage of the SSD1306 controller is from 1.65V to 3.3V while the OLED 
panel requires 7V to 15V supply voltage. 
- All these different power requirements are sufficed using internal charge pump circuitry
- This makes it possible to connect it to any 5V logic microcontroller easily without 
using any logic level converter.
-------------------------------------------------------------------------------------------
SSD1306 OLED Memory Map
Regardless of the size of the OLED module, the SSD1306 driver has a built-in 1KB Graphic Display Data RAM (GDDRAM) for the screen which holds the bit pattern to be displayed. This 1K memory area is organized in 8 pages (from 0 to 7). Each page contains 128 columns/segments (block 0 to 127). And each column can store 8 bits of data (from 0 to 7). That surely tells us we have

8 pages x 128 segments x 8 bits of data = 8192 bits = 1024 bytes = 1KB memory

Here are the complete specifications:

Display Technology	OLED (Organic LED)
MCU Interface	I2C / SPI
Screen Size	0.96 Inch Across
Resolution	128×64 pixels
Operating Voltage	3.3V – 5V
Operating Current	20mA max
Viewing Angle	160°
Characters Per Row	21
Number of Character Rows	7
Data in SSD1306 OLED
There are two types of data that we can send to the SSD1306 OLED.

Command
Data to be written into the GDDRAM
Whenever you send some data, you have to send the control byte first, then send the data byte after that. That control byte used to tell the data that you are sending is command or data.

Control Byte
Bit 7	Bit 6	Bit 5	Bit 4	Bit 3	Bit 2	Bit 1	Bit 0
Co bit *	D/C *	0	0	0	0	0	0
The first 6 bits should be 0.

D/C – If this bit is 1, then the next byte will be a command. If this bit is 0, then the next byte will be data.

Co –  If this bit is 0, then the following bytes contain data bytes only.

Example
If you want to send a command, make the control byte as 0x00 and attach the command to the next byte. Let’s say I want to send the command 0xAE (Turn OFF the display), Follow the below steps.

Send the Start condition
Send the Slave address with R/W bit
Send the Control Byte (0x00)
Send the Command byte (0xAE)
Send the Stop condition
If you want to write some 0xFF to the display, then follow the below steps.

Send the Start condition
Send the Slave address with R/W bit
Send the Control Byte (0x40)
Send the Command byte (0xFF)
Send the Stop condition
For more information, please refer to the datasheet of the SSED1306.

Example Programming
In this example, we are going to use the SSD1306 OLED display as the slave device. This is not the tutorial of SSD1306 whereas this is the tutorial of I2C in the Linux device driver. So we don’t show any circus in the SSD1306 OLED display. We will just fill something in the display and clear that. That’s all.

Note: Don’t care about the commands that we send to initialize the OLED display. We will explain that in separate tutorials.

The concept of this example is, we will fill 0xFF in the full display when we load the driver and clears it while unloading. This makes the process simple right. Let’s do it.

Please enable the I2C in the Raspberry Pi.

If you see the Pinout of the Raspberry Pi 4,

GPIO 2 – SDA
GPIO 3 – SCL
Connection Diagram
ssd1306-interface-with-raspberry-pi

Driver Source Code
If you want to interface any other I2C slave, then you don’t care about the functions that start with SSD1306_. You implement your own functions for that slave.

[Get the source code from the GitHub]

/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple I2C driver explanation (SSD_1306 OLED Display Interface)
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
* *******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>

#define I2C_BUS_AVAILABLE   (          1 )              // I2C Bus available in our Raspberry Pi
#define SLAVE_DEVICE_NAME   ( "ETX_OLED" )              // Device and Driver Name
#define SSD1306_SLAVE_ADDR  (       0x3C )              // SSD1306 OLED Slave Address

static struct i2c_adapter *etx_i2c_adapter     = NULL;  // I2C Adapter Structure
static struct i2c_client  *etx_i2c_client_oled = NULL;  // I2C Cient Structure (In our case it is OLED)

/*
** This function writes the data into the I2C client
**
**  Arguments:
**      buff -> buffer to be sent
**      len  -> Length of the data
**   
*/
static int I2C_Write(unsigned char *buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit, 
    ** ACK/NACK and Stop condtions will be handled internally.
    */ 
    int ret = i2c_master_send(etx_i2c_client_oled, buf, len);
    
    return ret;
}

/*
** This function reads one byte of the data from the I2C client
**
**  Arguments:
**      out_buff -> buffer wherer the data to be copied
**      len      -> Length of the data to be read
** 
*/
static int I2C_Read(unsigned char *out_buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit, 
    ** ACK/NACK and Stop condtions will be handled internally.
    */ 
    int ret = i2c_master_recv(etx_i2c_client_oled, out_buf, len);
    
    return ret;
}

/*
** This function is specific to the SSD_1306 OLED.
** This function sends the command/data to the OLED.
**
**  Arguments:
**      is_cmd -> true = command, flase = data
**      data   -> data to be written
** 
*/
static void SSD1306_Write(bool is_cmd, unsigned char data)
{
    unsigned char buf[2] = {0};
    int ret;
    
    /*
    ** First byte is always control byte. Data is followed after that.
    **
    ** There are two types of data in SSD_1306 OLED.
    ** 1. Command
    ** 2. Data
    **
    ** Control byte decides that the next byte is, command or data.
    **
    ** -------------------------------------------------------                        
    ** |              Control byte's | 6th bit  |   7th bit  |
    ** |-----------------------------|----------|------------|    
    ** |   Command                   |   0      |     0      |
    ** |-----------------------------|----------|------------|
    ** |   data                      |   1      |     0      |
    ** |-----------------------------|----------|------------|
    ** 
    ** Please refer the datasheet for more information. 
    **    
    */ 
    if( is_cmd == true )
    {
        buf[0] = 0x00;
    }
    else
    {
        buf[0] = 0x40;
    }
    
    buf[1] = data;
    
    ret = I2C_Write(buf, 2);
}


/*
** This function sends the commands that need to used to Initialize the OLED.
**
**  Arguments:
**      none
** 
*/
static int SSD1306_DisplayInit(void)
{
    msleep(100);               // delay

    /*
    ** Commands to initialize the SSD_1306 OLED Display
    */
    SSD1306_Write(true, 0xAE); // Entire Display OFF
    SSD1306_Write(true, 0xD5); // Set Display Clock Divide Ratio and Oscillator Frequency
    SSD1306_Write(true, 0x80); // Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended
    SSD1306_Write(true, 0xA8); // Set Multiplex Ratio
    SSD1306_Write(true, 0x3F); // 64 COM lines
    SSD1306_Write(true, 0xD3); // Set display offset
    SSD1306_Write(true, 0x00); // 0 offset
    SSD1306_Write(true, 0x40); // Set first line as the start line of the display
    SSD1306_Write(true, 0x8D); // Charge pump
    SSD1306_Write(true, 0x14); // Enable charge dump during display on
    SSD1306_Write(true, 0x20); // Set memory addressing mode
    SSD1306_Write(true, 0x00); // Horizontal addressing mode
    SSD1306_Write(true, 0xA1); // Set segment remap with column address 127 mapped to segment 0
    SSD1306_Write(true, 0xC8); // Set com output scan direction, scan from com63 to com 0
    SSD1306_Write(true, 0xDA); // Set com pins hardware configuration
    SSD1306_Write(true, 0x12); // Alternative com pin configuration, disable com left/right remap
    SSD1306_Write(true, 0x81); // Set contrast control
    SSD1306_Write(true, 0x80); // Set Contrast to 128
    SSD1306_Write(true, 0xD9); // Set pre-charge period
    SSD1306_Write(true, 0xF1); // Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK
    SSD1306_Write(true, 0xDB); // Set Vcomh deselect level
    SSD1306_Write(true, 0x20); // Vcomh deselect level ~ 0.77 Vcc
    SSD1306_Write(true, 0xA4); // Entire display ON, resume to RAM content display
    SSD1306_Write(true, 0xA6); // Set Display in Normal Mode, 1 = ON, 0 = OFF
    SSD1306_Write(true, 0x2E); // Deactivate scroll
    SSD1306_Write(true, 0xAF); // Display ON in normal mode
    
    return 0;
}

/*
** This function Fills the complete OLED with this data byte.
**
**  Arguments:
**      data  -> Data to be filled in the OLED
** 
*/
static void SSD1306_Fill(unsigned char data)
{
    unsigned int total  = 128 * 8;  // 8 pages x 128 segments x 8 bits of data
    unsigned int i      = 0;
    
    //Fill the Display
    for(i = 0; i < total; i++)
    {
        SSD1306_Write(false, data);
    }
}

/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int etx_oled_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    SSD1306_DisplayInit();
    
    //fill the OLED with this data
    SSD1306_Fill(0xFF);

    pr_info("OLED Probed!!!\n");
    
    return 0;
}

/*
** This function getting called when the slave has been removed
** Note : This will be called only once when we unload the driver.
*/
static int etx_oled_remove(struct i2c_client *client)
{   
    //fill the OLED with this data
    SSD1306_Fill(0x00);
    
    pr_info("OLED Removed!!!\n");
    return 0;
}

/*
** Structure that has slave device id
*/
static const struct i2c_device_id etx_oled_id[] = {
        { SLAVE_DEVICE_NAME, 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, etx_oled_id);

/*
** I2C driver Structure that has to be added to linux
*/
static struct i2c_driver etx_oled_driver = {
        .driver = {
            .name   = SLAVE_DEVICE_NAME,
            .owner  = THIS_MODULE,
        },
        .probe          = etx_oled_probe,
        .remove         = etx_oled_remove,
        .id_table       = etx_oled_id,
};

/*
** I2C Board Info strucutre
*/
static struct i2c_board_info oled_i2c_board_info = {
        I2C_BOARD_INFO(SLAVE_DEVICE_NAME, SSD1306_SLAVE_ADDR)
    };

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
    int ret = -1;

    etx_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);
    
    if( etx_i2c_adapter != NULL )
    {
        etx_i2c_client_oled = i2c_new_device(etx_i2c_adapter, &oled_i2c_board_info);
        
        if( etx_i2c_client_oled != NULL )
        {
            i2c_add_driver(&etx_oled_driver);
            ret = 0;
        }
        
        i2c_put_adapter(etx_i2c_adapter);
    }
    
    pr_info("Driver Added!!!\n");
    return ret;
}

/*
** Module Exit function
*/
static void __exit etx_driver_exit(void)
{
    i2c_unregister_device(etx_i2c_client_oled);
    i2c_del_driver(&etx_oled_driver);
    pr_info("Driver Removed!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <[email protected]>");
MODULE_DESCRIPTION("Simple I2C driver explanation (SSD_1306 OLED Display Interface)");
MODULE_VERSION("1.34");
Makefile
obj-m += driver.o
 
KDIR = /lib/modules/$(shell uname -r)/build
 
 
all:
    make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
    make -C $(KDIR)  M=$(shell pwd) clean
Testing the Device Driver
Build the driver by using Makefile (sudo make)
Load the driver using sudo insmod driver.ko
See the Display is filled.
Unload the driver using sudo rmmod driver
See the Display has been cleared

##########################################################################################
																I2C "Bus Driver" in Linux

Prerequisites
I2C Introduction – Part 1 (Basics)
I2C Introduction – Part 2 (Advanced Topics)
I2C Client Driver in Linux kernel

Hardware Required
Raspberry Pi
SSD1306 OLED I2C Display

Bring up Raspberry PI
Install Raspberry Pi OS (32-bit) with desktop in the SD card.
Then install the kernel header using sudo apt install raspberrypi-kernel-headers
For your information, In our Raspberry PI 4 board, kernel 5.4.51-v7l+ is installed.

Please enable the I2C in the Raspberry Pi.
-------------------------------------------------------------------------------------------
Introduction

- In our last tutorial, we have seen how to write the I2C device driver in the Linux 
kernel for the Slave client. 
- Let’s recap what we have done there.

1. Get the adapter that is available using i2c_get_adapter()
2. Add the device using i2c_new_device()
3. Add the driver to the subsystem using i2c_add_driver()
4. Once you have done these steps, then just transfer the data using any transfer API.
5. Finally, unregister the device using i2c_unregister_device() and delete the driver from the subsystem using i2c_del_driver().
-------------------------------------------------------------------------------------------

- Have you ever thought that it is very simple, unlike the I2C Bare-metal coding "?" 
- Yes, you are correct. It is simple. But have you ever wondered that who is sending the 
START, STOP, ADDRESS, READ with ACK, READ with NACK, etc "?" 
- I might give you the hint in the previous tutorial. Yes, you are correct. 
- The I2C bus driver will do such operations.

- So in this tutorial, we will discuss the I2C bus drivers in the Linux kernel.
-------------------------------------------------------------------------------------------
APIs used for the I2C bus driver

- There are two structures that you need to use in order to write the i2c bus driver in 
the Linux kernel.

1. Algorithm Structure
2. Adapter Structure
-------------------------------------------------------------------------------------------
Algorithm Structure

- This structure represents the I2C transfer method.

struct i2c_algorithm 
{
  int (* master_xfer) (struct i2c_adapter *adap, struct i2c_msg *msgs,int num);
  int (* smbus_xfer) (struct i2c_adapter *adap, u16 addr,unsigned short flags, char read_write,u8 command, int size, union i2c_smbus_data *data);
  u32 (* functionality) (struct i2c_adapter *);
#if IS_ENABLED(CONFIG_I2C_SLAVE)
  int (* reg_slave) (struct i2c_client *client);
  int (* unreg_slave) (struct i2c_client *client);
#endif
};

master_xfer: Issue a set of i2c transactions to the given I2C adapter defined by the 
msgs array, with num messages available to transfer via the adapter specified by adap. 
This function will be called whenever we call I2C read-write APIs from the client driver.

smbus_xfer: Issue SMBus transactions to the given I2C adapter. 
If this is not present, then the bus layer will try and convert the SMBus calls into 
I2C transfers instead. 
This function will be called whenever we call SMBus read-write APIs from the client driver.

functionality: Return the flags that this algorithm/adapter pair supports from the 
I2C_FUNC_* flags.

reg_slave: Register given client to I2C slave mode of this adapter
unreg_slave: Unregister given client from I2C slave mode of this adapter

- The return codes from the master_xfer field should indicate the type of error code that 
occurred during the transfer, as documented in the kernel Documentation file 
Documentation/i2c/fault-codes.
-------------------------------------------------------------------------------------------
Adapter Structure

- This structure is used to identify a physical i2c bus along with the access 
algorithms necessary to access it.

struct i2c_adapter 
{
    struct module *owner;
    unsigned int class;
    const struct i2c_algorithm *algo;
    void *algo_data;
    struct rt_mutex bus_lock;
    int timeout;
    int retries;
    struct device dev;
    int nr;
    char name[48];
    struct completion dev_released;
    struct mutex userspace_clients_lock;
    struct list_head userspace_clients;
};

*owner       — Owner of the module(usually set this to THIS_MODULE).
class        — the type of I2C class devices that this driver supports. 
Usually, this is set to any one of the I2C_CLASS_* based on our need.
*algo         — a pointer to the struct i2c_algorithm structure
bus_lock   — Mutex lock.
timeout    — Timeout in jiffies.
retries     — Number of retries.
dev             — the adapter device.
nr               — bus number which you want to create. This will be applicable only for i2c_add_numbered_adapter().
char name[I2C_NAME_SIZE] — I2C bus driver name. This value shows up in the sysfs filename associated with this I2C adapter.

After you create the two structures, then we have to add the adapter to the i2c subsystem.
-------------------------------------------------------------------------------------------
Add the adapter to the subsystem

i2c_add_adapter
- This API is used to register the adapter to the subsystem. 
But this will assign the dynamic bus number.

int i2c_add_adapter (struct i2c_adapter * adapter);
adapter – the adapter to add

It returns zero when a new bus number was allocated and stored in adap->nr, and the 
specified adapter became available for clients. 
Otherwise, a negative errno value is returned
-------------------------------------------------------------------------------------------
i2c_add_numbered_adapter

- This API is used to register the adapter to the subsystem. 
- But it assigns the number that we asked for if only it is available. 
- We have to initialize the member called nr in the i2c_adapter structure before 
calling this.

int i2c_add_numbered_adapter ( struct i2c_adapter * adap);

adap – the adapter to register (with adap->nr initialized)

This returns zero when the specified adapter is available for clients using the bus number provided in adap->nr. Otherwise, a negative errno value is returned.

Example
/*
** I2C adapter Structure
*/
static struct i2c_adapter etx_i2c_adapter = {
    .owner  = THIS_MODULE,
    .class  = I2C_CLASS_HWMON,//| I2C_CLASS_SPD,
    .algo   = &etx_i2c_algorithm,
    .name   = ADAPTER_NAME,
    .nr     = 5,
};


/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
    int ret = -1;
    
    //create bus driver as i2c-5 if it is available
    ret = i2c_add_numbered_adapter(&etx_i2c_adapter);
    
    pr_info("Bus Driver Added!!!\n");
    return ret;
}

- If bus number 5 is not used by anyone, then it will assign that bus number to the client
Otherwise, it will return a negative number.
-------------------------------------------------------------------------------------------
Delete the adapter from the subsystem

- This API is used to unregister the adapter from the subsystem.

void i2c_del_adapter ( struct i2c_adapter * adap);

adap – the adapter being unregistered
-------------------------------------------------------------------------------------------
Example Programming

- We are going to write the I2C bus driver which does nothing. I meant it won’t send any 
START, STOP, ADDRESS, READ with ACK, READ with NACK, etc. 
- It won’t communicate with the slave device. 
- The i2c bus driver just prints the message that it gets instead. 
- So actually it is a dummy I2C bus driver. 
- What is the use of this dummy I2C bus driver "?" 
- This will give you some basic ideas about how it is working. 
- What if you call i2c_master_send(), i2c_master_recv() and any SMBUS API like 
i2c_smbus_read_byte() from the I2C Client Device driver "?" So this example will give you 
an overview.
-------------------------------------------------------------------------------------------
I2C Bus Driver Source Code
[Get the source code from the GitHub]

/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple I2C Bus driver explanation
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
* *******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>

#define ADAPTER_NAME     "ETX_I2C_ADAPTER"

/*
** This function used to get the functionalities that are supported 
** by this bus driver.
*/
static u32 etx_func(struct i2c_adapter *adapter)
{
    return (I2C_FUNC_I2C             |
            I2C_FUNC_SMBUS_QUICK     |
            I2C_FUNC_SMBUS_BYTE      |
            I2C_FUNC_SMBUS_BYTE_DATA |
            I2C_FUNC_SMBUS_WORD_DATA |
            I2C_FUNC_SMBUS_BLOCK_DATA);
}

/*
** This function will be called whenever you call I2C read, wirte APIs like
** i2c_master_send(), i2c_master_recv() etc.
*/
static s32 etx_i2c_xfer( struct i2c_adapter *adap, struct i2c_msg *msgs,int num )
{
    int i;
    
    for(i = 0; i < num; i++)
    {
        int j;
        struct i2c_msg *msg_temp = &msgs[i];
        
        pr_info("[Count: %d] [%s]: [Addr = 0x%x] [Len = %d] [Data] = ", i, __func__, msg_temp->addr, msg_temp->len);
        
        for( j = 0; j < msg_temp->len; j++ )
        {
            pr_cont("[0x%02x] ", msg_temp->buf[j]);
        }
    }
    return 0;
}

/*
** This function will be called whenever you call SMBUS read, wirte APIs
*/
static s32 etx_smbus_xfer(  struct i2c_adapter *adap, 
                            u16 addr,
                            unsigned short flags, 
                            char read_write,
                            u8 command, 
                            int size, 
                            union i2c_smbus_data *data
                         )
{
    pr_info("In %s\n", __func__);
    return 0;
}

/*
** I2C algorithm Structure
*/
static struct i2c_algorithm etx_i2c_algorithm = {
    .smbus_xfer     = etx_smbus_xfer,
    .master_xfer    = etx_i2c_xfer,
    .functionality  = etx_func,
};

/*
** I2C adapter Structure
*/
static struct i2c_adapter etx_i2c_adapter = {
    .owner  = THIS_MODULE,
    .class  = I2C_CLASS_HWMON,//| I2C_CLASS_SPD,
    .algo   = &etx_i2c_algorithm,
    .name   = ADAPTER_NAME,
};


/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
    int ret = -1;
    
    ret = i2c_add_adapter(&etx_i2c_adapter);
    
    pr_info("Bus Driver Added!!!\n");
    return ret;
}

/*
** Module Exit function
*/
static void __exit etx_driver_exit(void)
{
    i2c_del_adapter(&etx_i2c_adapter);
    pr_info("Bus Driver Removed!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <[email protected]>");
MODULE_DESCRIPTION("Simple I2C Bus driver explanation");
MODULE_VERSION("1.35");
Makefile
obj-m += driver_bus.o
 
KDIR = /lib/modules/$(shell uname -r)/build
 
 
all:
    make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
    make -C $(KDIR)  M=$(shell pwd) clean
-------------------------------------------------------------------------------------------
- Since we have used i2c_add_adapter(), it will assign the dynamic bus number. 
- So we have to check the bus number before using that bus by the i2c client driver.

- Once you have written the I2C bus driver build that using sudo make, then load the 
I2C bus driver using 
sudo insmod driver_bus.ko 
- Then you just read the busses available using  tree/sys/bus/i2c/. 
You must get something like below.

[email protected]:~/Desktop/workspace $ tree /sys/bus/i2c/
/sys/bus/i2c/
├── devices
│   ├── i2c-1 -> ../../../devices/platform/soc/fe804000.i2c/i2c-1
│   └── i2c-11 -> ../../../devices/i2c-11
├── drivers
│   ├── dummy
│   │   ├── bind
│   │   ├── uevent
│   │   └── unbind
│   └── stmpe-i2c
│       ├── bind
│       ├── uevent
│       └── unbind
├── drivers_autoprobe
├── drivers_probe
└── uevent

6 directories, 9 files

- Here, in my case, I got a new I2C bus called i2c-11. 
- So this I2C bus driver gave me the bus number as 11. 
- You just get yours. 
- That number will be used in the I2C client device driver.
-------------------------------------------------------------------------------------------
I2C Client Device Driver Source code

- We just took the last tutorial’s I2C Client driver example code. 
We made the below changes on top of that.

- Changed the macro I2C_BUS_AVAILABLE value from 1 to 11 as our i2c bus driver is 11 now

- I just commented (removed) the function call SSD1306_Fill() wherever it was being called
- Because if we add that we will see more prints in dmesg. 
- In this code, we will just see the prints of init commands from SSD1306_DisplayInit().
[Get the source code from the GitHub]

/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple I2C driver explanation (SSD_1306 OLED Display Interface)
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
* *******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>

#define I2C_BUS_AVAILABLE   (         11 )              // I2C Bus that we have created
#define SLAVE_DEVICE_NAME   ( "ETX_OLED" )              // Device and Driver Name
#define SSD1306_SLAVE_ADDR  (       0x3C )              // SSD1306 OLED Slave Address

static struct i2c_adapter *etx_i2c_adapter     = NULL;  // I2C Adapter Structure
static struct i2c_client  *etx_i2c_client_oled = NULL;  // I2C Cient Structure (In our case it is OLED)

/*
** This function writes the data into the I2C client
**
**  Arguments:
**      buff -> buffer to be sent
**      len  -> Length of the data
**   
*/
static int I2C_Write(unsigned char *buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit, 
    ** ACK/NACK and Stop condtions will be handled internally.
    */ 
    int ret = i2c_master_send(etx_i2c_client_oled, buf, len);
    
    return ret;
}

/*
** This function reads one byte of the data from the I2C client
**
**  Arguments:
**      out_buff -> buffer wherer the data to be copied
**      len      -> Length of the data to be read
** 
*/
static int I2C_Read(unsigned char *out_buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit, 
    ** ACK/NACK and Stop condtions will be handled internally.
    */ 
    int ret = i2c_master_recv(etx_i2c_client_oled, out_buf, len);
    
    return ret;
}

/*
** This function is specific to the SSD_1306 OLED.
** This function sends the command/data to the OLED.
**
**  Arguments:
**      is_cmd -> true = command, flase = data
**      data   -> data to be written
** 
*/
static void SSD1306_Write(bool is_cmd, unsigned char data)
{
    unsigned char buf[2] = {0};
    int ret;
    
    /*
    ** First byte is always control byte. Data is followed after that.
    **
    ** There are two types of data in SSD_1306 OLED.
    ** 1. Command
    ** 2. Data
    **
    ** Control byte decides that the next byte is, command or data.
    **
    ** -------------------------------------------------------                        
    ** |              Control byte's | 6th bit  |   7th bit  |
    ** |-----------------------------|----------|------------|    
    ** |   Command                   |   0      |     0      |
    ** |-----------------------------|----------|------------|
    ** |   data                      |   1      |     0      |
    ** |-----------------------------|----------|------------|
    ** 
    ** Please refer the datasheet for more information. 
    **    
    */ 
    if( is_cmd == true )
    {
        buf[0] = 0x00;
    }
    else
    {
        buf[0] = 0x40;
    }
    
    buf[1] = data;
    
    ret = I2C_Write(buf, 2);
}


/*
** This function sends the commands that need to used to Initialize the OLED.
**
**  Arguments:
**      none
** 
*/
static int SSD1306_DisplayInit(void)
{
    msleep(100);               // delay

    /*
    ** Commands to initialize the SSD_1306 OLED Display
    */
    SSD1306_Write(true, 0xAE); // Entire Display OFF
    SSD1306_Write(true, 0xD5); // Set Display Clock Divide Ratio and Oscillator Frequency
    SSD1306_Write(true, 0x80); // Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended
    SSD1306_Write(true, 0xA8); // Set Multiplex Ratio
    SSD1306_Write(true, 0x3F); // 64 COM lines
    SSD1306_Write(true, 0xD3); // Set display offset
    SSD1306_Write(true, 0x00); // 0 offset
    SSD1306_Write(true, 0x40); // Set first line as the start line of the display
    SSD1306_Write(true, 0x8D); // Charge pump
    SSD1306_Write(true, 0x14); // Enable charge dump during display on
    SSD1306_Write(true, 0x20); // Set memory addressing mode
    SSD1306_Write(true, 0x00); // Horizontal addressing mode
    SSD1306_Write(true, 0xA1); // Set segment remap with column address 127 mapped to segment 0
    SSD1306_Write(true, 0xC8); // Set com output scan direction, scan from com63 to com 0
    SSD1306_Write(true, 0xDA); // Set com pins hardware configuration
    SSD1306_Write(true, 0x12); // Alternative com pin configuration, disable com left/right remap
    SSD1306_Write(true, 0x81); // Set contrast control
    SSD1306_Write(true, 0x80); // Set Contrast to 128
    SSD1306_Write(true, 0xD9); // Set pre-charge period
    SSD1306_Write(true, 0xF1); // Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK
    SSD1306_Write(true, 0xDB); // Set Vcomh deselect level
    SSD1306_Write(true, 0x20); // Vcomh deselect level ~ 0.77 Vcc
    SSD1306_Write(true, 0xA4); // Entire display ON, resume to RAM content display
    SSD1306_Write(true, 0xA6); // Set Display in Normal Mode, 1 = ON, 0 = OFF
    SSD1306_Write(true, 0x2E); // Deactivate scroll
    SSD1306_Write(true, 0xAF); // Display ON in normal mode
    
    return 0;
}

/*
** This function Fills the complete OLED with this data byte.
**
**  Arguments:
**      data  -> Data to be filled in the OLED
** 
*/
static void SSD1306_Fill(unsigned char data)
{
    unsigned int total  = 128 * 8;  // 8 pages x 128 segments x 8 bits of data
    unsigned int i      = 0;
    
    //Fill the Display
    for(i = 0; i < total; i++)
    {
        SSD1306_Write(false, data);
    }
}

/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int etx_oled_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    SSD1306_DisplayInit();
    
    //fill the OLED with this data
    //SSD1306_Fill(0xFF);           //commenting this line as this is not required for this tutorial

    pr_info("OLED Probed!!!\n");
    
    return 0;
}

/*
** This function getting called when the slave has been removed
** Note : This will be called only once when we unload the driver.
*/
static int etx_oled_remove(struct i2c_client *client)
{   
    //fill the OLED with this data
    //SSD1306_Fill(0xFF);           //commenting this line as this is not required for this tutorial
    
    pr_info("OLED Removed!!!\n");
    return 0;
}

/*
** Structure that has slave device id
*/
static const struct i2c_device_id etx_oled_id[] = {
        { SLAVE_DEVICE_NAME, 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, etx_oled_id);

/*
** I2C driver Structure that has to be added to linux
*/
static struct i2c_driver etx_oled_driver = {
        .driver = {
            .name   = SLAVE_DEVICE_NAME,
            .owner  = THIS_MODULE,
        },
        .probe          = etx_oled_probe,
        .remove         = etx_oled_remove,
        .id_table       = etx_oled_id,
};

/*
** I2C Board Info strucutre
*/
static struct i2c_board_info oled_i2c_board_info = {
        I2C_BOARD_INFO(SLAVE_DEVICE_NAME, SSD1306_SLAVE_ADDR)
    };

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
    
    etx_i2c_adapter     = i2c_get_adapter(I2C_BUS_AVAILABLE);
    
    if( etx_i2c_adapter != NULL )
    {
        etx_i2c_client_oled = i2c_new_device(etx_i2c_adapter, &oled_i2c_board_info);
        
        if( etx_i2c_client_oled != NULL )
        {
            i2c_add_driver(&etx_oled_driver);
        }
        
        i2c_put_adapter(etx_i2c_adapter);
    }
    
    pr_info("Client Driver Added!!!\n");
    return 0;
}

/*
** Module Exit function
*/
static void __exit etx_driver_exit(void)
{
    i2c_unregister_device(etx_i2c_client_oled);
    i2c_del_driver(&etx_oled_driver);
    pr_info("Client Driver Removed!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <[email protected]>");
MODULE_DESCRIPTION("Simple I2C driver explanation (SSD_1306 OLED Display Interface)");
MODULE_VERSION("1.36");
Makefile
obj-m += driver_client.o
 
KDIR = /lib/modules/$(shell uname -r)/build
 
 
all:
    make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
    make -C $(KDIR)  M=$(shell pwd) clean
Testing the Device Driver
Since we have loaded the i2c bus driver already, now we will load the i2c client driver.

Build the driver by using Makefile (sudo make)
Load the driver using sudo insmod driver_client.ko
Once we have loaded the i2c client driver, it should have called the probe function.
So in that probe function, we were using some I2C write functions. That must get us to the i2c bus drivers function. Let’s check the dmesg.

[email protected]:~/Desktop/workspace $ dmesg 
[10645.945371] Bus Driver Added!!!
[10652.873687] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xae] 
[10652.873694] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xd5] 
[10652.873705] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x80] 
[10652.873713] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xa8] 
[10652.873721] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x3f] 
[10652.873729] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xd3] 
[10652.873737] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x00] 
[10652.873745] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x40] 
[10652.873753] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x8d] 
[10652.873761] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x14] 
[10652.873768] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x20] 
[10652.873776] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x00] 
[10652.873784] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xa1] 
[10652.873792] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xc8] 
[10652.873800] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xda] 
[10652.873808] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x12] 
[10652.873816] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x81] 
[10652.873823] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x80] 
[10652.873831] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xd9] 
[10652.873839] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xf1] 
[10652.873847] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xdb] 
[10652.873854] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x20] 
[10652.873862] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xa4] 
[10652.873870] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xa6] 
[10652.873878] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0x2e] 
[10652.873885] [Count: 0] [etx_i2c_xfer]: [Addr = 0x3c] [Len = 2] [Data] = [0x00] [0xaf] 
[10652.873893] OLED Probed!!!
[10652.873988] Client Driver Added!!!
yeah hoooo. We got it. We can able to see all the bytes that we are transmitting for initializing the SSD1306 display.
Now you can unload the i2c client driver using sudo rmmod driver_client
After that unload the i2c bus driver using sudo rmmod driver_bus
In our next tutorial, we will see how to communicate with the slave device from the I2C bus driver.

Please find the other Linux device driver tutorials here.

You can also read the below tutorials.

##########################################################################################
Real I2C Bus Linux Device Driver

Prerequisites
I2C Introduction – Part 1 (Basics)
I2C Introduction – Part 2 (Advanced Topics)
I2C Client Driver in Linux kernel
I2C Dummy Driver in Linux kernel

GPIO Linux Driver
Hardware Required
Raspberry Pi
SSD1306 OLED I2C Display

Bring up Raspberry PI
Install Raspberry Pi OS (32-bit) with desktop on the SD card.
Then install the kernel header using sudo apt install raspberrypi-kernel-headers
For your information, In our Raspberry PI 4 board, kernel 5.4.51-v7l+ is installed.

Please enable the I2C in the Raspberry Pi.
-------------------------------------------------------------------------------------------
Introduction

- In our last tutorial, we have seen how to write the dummy I2C bus driver in the Linux 
kernel. It doesn’t send any data to the Salve device. 
- So in this tutorial, we have come up with the real I2C bus Linux device driver. 
- Using this I2C bus driver, we can send data to the slave device. Let’s start.

- We have discussed the APIs too in the last tutorials. In this tutorial, 
we just used a new API called i2c_add_numbered_adapter(). 
- This API will allocate the bus number that we are asking if that is available. 
- In the last tutorial, we have not used this. Instead, we used i2c_add_adapter(). 
- This API will allocate the bus number dynamically.

- We can move to the example straightaway since we don’t have any other thing to discuss.
-------------------------------------------------------------------------------------------
Example Programming

- In this I2C client Driver tutorial, we have just written the I2C Client driver which 
communicates to the slave device called SSD1306 OLED I2C Display by using its internal 
I2C bus driver. 
- That example demonstrates it will just fill something into the display while loading 
and it will clear the display while unloading. 
- The same thing only we are going to try with our own I2C bus driver in this tutorial.
-------------------------------------------------------------------------------------------
Connection Diagram

SCL – GPIO 20
SDA – GPIO 21
-------------------------------------------------------------------------------------------
I2C-bus-driver-ssd1306-interface-with-raspberry-pi

I2C Bus Driver Source Code
[Get the source code from GitHub]

Note: In the below example I have implemented the I2C communication by using the 
"bit-banging" method (Linux Kernel’s  GPIO API has been used). 
- I have not implemented the I2C read part. 
- And also we have not handled arbitration, clock stretching, etc in this tutorial 
as this is just an example program. This is only for demonstration purposes.

/***************************************************************************//**
*  \file       driver_bus.c
*
*  \details    Simple I2C Bus driver explanation
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
* *******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/gpio.h>

#define ADAPTER_NAME     "ETX_I2C_ADAPTER"

/*------------ I2C related functions - Bit banging method - START --------------*/

#define SCL_GPIO  20    //GPIO act as SCL
#define SDA_GPIO  21    //GPIO act as SDA

#define I2C_DELAY usleep_range(5, 10) 

/*
** Function to read the SCL GPIO
*/
static bool ETX_I2C_Read_SCL(void)
{
  gpio_direction_input(SCL_GPIO);
  return gpio_get_value(SCL_GPIO);
}

/*
** Function to read the SDA GPIO
*/
static bool ETX_I2C_Read_SDA(void)
{
  gpio_direction_input(SDA_GPIO);
  return gpio_get_value(SDA_GPIO);
}

/*
** Function to clear the SCL GPIO
*/
static void ETX_I2C_Clear_SCL(void)
{
  gpio_direction_output(SCL_GPIO, 0);
  gpio_set_value(SCL_GPIO, 0);
}

/*
** Function to clear the SDA GPIO
*/
static void ETX_I2C_Clear_SDA(void)
{
  gpio_direction_output(SDA_GPIO, 0);
  gpio_set_value(SDA_GPIO, 0);
}

/*
** Function to set the SCL GPIO
*/
static void ETX_I2C_Set_SCL(void)
{
  gpio_direction_output(SCL_GPIO, 1);
  gpio_set_value(SCL_GPIO, 1);
}

/*
** Function to set the SDA GPIO
*/
static void ETX_I2C_Set_SDA(void)
{
  gpio_direction_output(SDA_GPIO, 1);
  gpio_set_value(SDA_GPIO, 1);
}

/*
** Function to Initialize the GPIOs
*/
static int ETX_I2C_Init( void )
{
  int ret = 0;

  do      //Break if any error
  {
    //Checking the SCL GPIO is valid or not
    if(gpio_is_valid(SCL_GPIO) == false){
          pr_err("SCL GPIO %d is not valid\n", SCL_GPIO);
          ret = -1;
          break;
    }

    //Checking the SDA GPIO is valid or not
    if(gpio_is_valid(SDA_GPIO) == false){
          pr_err("SDA GPIO %d is not valid\n", SDA_GPIO);
          ret = -1;
          break;
    }
    
    //Requesting the SCL GPIO
    if(gpio_request(SCL_GPIO,"SCL_GPIO") < 0){
          pr_err("ERROR: SCL GPIO %d request\n", SCL_GPIO);
          ret = -1;
          break;
    }

    //Requesting the SDA GPIO
    if(gpio_request(SDA_GPIO,"SDA_GPIO") < 0){
          pr_err("ERROR: SDA GPIO %d request\n", SDA_GPIO);
          //free already requested SCL GPIO
          gpio_free(SCL_GPIO);
          ret = -1;
          break;
    }
    
    /*
    ** configure the SCL GPIO as output, We will change the 
    ** direction later as per our need.
    */
    gpio_direction_output(SCL_GPIO, 1);

    /*
    ** configure the SDA GPIO as output, We will change the 
    ** direction later as per our need.
    */
    gpio_direction_output(SDA_GPIO, 1);

  } while(false);

  return ret;  
}

/*
** Function to Deinitialize the GPIOs
*/
static void ETX_I2C_DeInit( void )
{
  //free both the GPIOs
  gpio_free(SCL_GPIO);
  gpio_free(SDA_GPIO);
}

/*
** Function to send the START condition
**      ______  
**            \
**  SDA        \_____
**              :
**              :
**           ___:____
**  SCL     /
**      ___/ 
*/
static void ETX_I2C_Sart( void )
{
  ETX_I2C_Set_SDA();
  ETX_I2C_Set_SCL();
  I2C_DELAY;
  ETX_I2C_Clear_SDA();
  I2C_DELAY;  
  ETX_I2C_Clear_SCL();
  I2C_DELAY;  
}

/*
** Function to send the STOP condition
**                _____
**               /
**  SDA  _______/
**              :
**              :
**            __:______
**  SCL      /
**       ___/ 
*/
static void ETX_I2C_Stop( void )
{
  ETX_I2C_Clear_SDA();
  I2C_DELAY;
  ETX_I2C_Set_SCL();
  I2C_DELAY;
  ETX_I2C_Set_SDA();
  I2C_DELAY;
  ETX_I2C_Clear_SCL();
}

/*
** Function to reads the SDA to get the status
** 
** Retutns 0 for NACK, returns 1 for ACK
*/
static int ETX_I2C_Read_NACK_ACK( void )
{
  int ret = 1;

  //reading ACK/NACK
  I2C_DELAY;
  ETX_I2C_Set_SCL();
  I2C_DELAY;
  if( ETX_I2C_Read_SDA() )      //check for ACK/NACK
  {
    ret = 0;
  }

  ETX_I2C_Clear_SCL();
  return ret;
}

/*
** Function to send the 7-bit address to the slave
** 
** Retutns 0 if success otherwise negative number
*/
static int ETX_I2C_Send_Addr( u8 byte, bool is_read )
{
  int ret   = -1;
  u8  bit;
  u8  i     = 0;
  u8  size  = 7;

  //Writing 7bit slave address
  for(i = 0; i < size ; i++)
  {
    bit = ( ( byte >> ( size - ( i + 1 ) ) ) & 0x01 );  //store MSB value
    (bit) ? ETX_I2C_Set_SDA() : ETX_I2C_Clear_SDA();    //write MSB value     
    I2C_DELAY;
    ETX_I2C_Set_SCL();
    I2C_DELAY;
    ETX_I2C_Clear_SCL();
  }

  // Writing Read/Write bit (8th bit)
  (is_read) ? ETX_I2C_Set_SDA() : ETX_I2C_Clear_SDA();  //read = 1, write = 0
  I2C_DELAY;
  ETX_I2C_Set_SCL();
  I2C_DELAY;
  ETX_I2C_Clear_SCL();
  I2C_DELAY;

  if( ETX_I2C_Read_NACK_ACK() )
  {
    //got ACK
    ret = 0;
  }

  return ret;
}

/*
** Function to send the one byte to the slave
** 
** Retutns 0 if success otherwise negative number
*/
static int ETX_I2C_Send_Byte( u8 byte )
{
  int ret   = -1;
  u8  bit;
  u8  i     = 0;
  u8  size  = 7;

  for(i = 0; i <= size ; i++)
  {
    bit = ( ( byte >> ( size - i ) ) & 0x01 );        //store MSB value
    (bit) ? ETX_I2C_Set_SDA() : ETX_I2C_Clear_SDA();  //write MSB value     
    I2C_DELAY;
    ETX_I2C_Set_SCL();
    I2C_DELAY;
    ETX_I2C_Clear_SCL();
  }
  
  if( ETX_I2C_Read_NACK_ACK() )
  {
    //got ACK
    ret = 0;
  }

  return ret;
}

/*
** Function to read the one byte from the slave
** 
** Retutns 0 if success otherwise negative number
*/
static int ETX_I2C_Read_Byte( u8 *byte )
{
  int ret = 0;
  
  //TODO: Implement this

  return ret;
}

/*
** Function to send the number of bytes to the slave
** 
** Retutns 0 if success otherwise negative number
*/
static int ETX_I2C_Send( u8 slave_addr, u8 *buf, u16 len )
{
  int ret = 0;
  u16 num = 0;

  do      //Break if any error
  {

    if( ETX_I2C_Send_Addr(slave_addr, false) < 0 )   // Send the slave address
    {
      pr_err("ERROR: ETX_I2C_Send_Byte - Slave Addr\n");
      ret = -1;
      break;
    }

    for( num = 0; num < len; num++)
    {
      if( ETX_I2C_Send_Byte(buf[num]) < 0 )   // Send the data bytes
      {
        pr_err("ERROR: ETX_I2C_Send_Byte - [Data = 0x%02x]\n", buf[num]);
        ret = -1;
        break;
      }
    }
  } while(false);

  return ret;  
}

/*
** Function to read the number of bytes from the slave
** 
** Retutns 0 if success otherwise negative number
*/
static int ETX_I2C_Read( u8 slave_addr, u8 *buf, u16 len )
{
  int ret = 0;
  
  //TODO: Implement this

  return ret; 
}

/*------------ I2C related functions - Bit banging method - END ----------------*/

/*
** This function used to get the functionalities that are supported 
** by this bus driver.
*/
static u32 etx_func(struct i2c_adapter *adapter)
{
  return (I2C_FUNC_I2C             |
          I2C_FUNC_SMBUS_QUICK     |
          I2C_FUNC_SMBUS_BYTE      |
          I2C_FUNC_SMBUS_BYTE_DATA |
          I2C_FUNC_SMBUS_WORD_DATA |
          I2C_FUNC_SMBUS_BLOCK_DATA);
}

/*
** This function will be called whenever you call I2C read, wirte APIs like
** i2c_master_send(), i2c_master_recv() etc.
*/
static s32 etx_i2c_xfer( struct i2c_adapter *adap, struct i2c_msg *msgs,int num )
{
  int i;
  s32 ret = 0;
  do      //Break if any error
  {

    if( ETX_I2C_Init() < 0 )                  // Init the GPIOs
    {
      pr_err("ERROR: ETX_I2C_Init\n");
      break;
    }

    ETX_I2C_Sart();                           // Send START conditon
    
    for(i = 0; i < num; i++)
    {
      //int j;
      struct i2c_msg *msg_temp = &msgs[i];

      if( ETX_I2C_Send(msg_temp->addr, msg_temp->buf, msg_temp->len) < 0 )
      {
        ret = 0;
        break;
      }
      ret++;
#if 0
      pr_info("[Count: %d] [%s]: [Addr = 0x%x] [Len = %d] [Data] = ", i, __func__, msg_temp->addr, msg_temp->len);
      
      for( j = 0; j < msg_temp->len; j++ )
      {
          pr_cont("[0x%02x] ", msg_temp->buf[j]);
      }
#endif
    }
  } while(false);

  ETX_I2C_Stop();                             // Send STOP condtion

  ETX_I2C_DeInit();                           // Deinit the GPIOs

  return ret;
}

/*
** This function will be called whenever you call SMBUS read, wirte APIs
*/
static s32 etx_smbus_xfer(  struct i2c_adapter *adap, 
                            u16 addr,
                            unsigned short flags, 
                            char read_write,
                            u8 command, 
                            int size, 
                            union i2c_smbus_data *data
                         )
{
  pr_info("In %s\n", __func__);

  // TODO: Implement this

  return 0;
}

/*
** I2C algorithm Structure
*/
static struct i2c_algorithm etx_i2c_algorithm = {
  .smbus_xfer       = etx_smbus_xfer,
  .master_xfer    = etx_i2c_xfer,
  .functionality  = etx_func,
};

/*
** I2C adapter Structure
*/
static struct i2c_adapter etx_i2c_adapter = {
  .owner  = THIS_MODULE,
  .class  = I2C_CLASS_HWMON,//| I2C_CLASS_SPD,
  .algo   = &etx_i2c_algorithm,
  .name   = ADAPTER_NAME,
  .nr     = 5,
};

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
  int ret = -1;
  
  ret = i2c_add_numbered_adapter(&etx_i2c_adapter);
  
  pr_info("Bus Driver Added!!!\n");
  return ret;
}

/*
** Module Exit function
*/
static void __exit etx_driver_exit(void)
{
  i2c_del_adapter(&etx_i2c_adapter);
  pr_info("Bus Driver Removed!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <[email protected]>");
MODULE_DESCRIPTION("Simple I2C Bus driver explanation");
MODULE_VERSION("1.37");
Makefile
obj-m += driver_bus.o
 
KDIR = /lib/modules/$(shell uname -r)/build
 
 
all:
    make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
    make -C $(KDIR)  M=$(shell pwd) clean
I2C Client Device Driver Source code
We just took the last tutorial’s I2C Client driver example code. We made the below changes on top of that.

Changed the macro I2C_BUS_AVAILABLE value from 1 to 5 as we have asked our i2c bus number as 5.
[Get the source code from GitHub]

/***************************************************************************//**
*  \file       driver_client.c
*
*  \details    Simple I2C driver explanation (SSD_1306 OLED Display Interface)
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
* *******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
 
#define I2C_BUS_AVAILABLE   (          5 )              // I2C Bus available in our Raspberry Pi
#define SLAVE_DEVICE_NAME   ( "ETX_OLED" )              // Device and Driver Name
#define SSD1306_SLAVE_ADDR  (       0x3C )              // SSD1306 OLED Slave Address
 
static struct i2c_adapter *etx_i2c_adapter     = NULL;  // I2C Adapter Structure
static struct i2c_client  *etx_i2c_client_oled = NULL;  // I2C Cient Structure (In our case it is OLED)
 
/*
** This function writes the data into the I2C client
**
**  Arguments:
**      buff -> buffer to be sent
**      len  -> Length of the data
**   
*/
static int I2C_Write(unsigned char *buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit, 
    ** ACK/NACK and Stop condtions will be handled internally.
    */ 
    int ret = i2c_master_send(etx_i2c_client_oled, buf, len);
    
    return ret;
}
 
/*
** This function reads one byte of the data from the I2C client
**
**  Arguments:
**      out_buff -> buffer wherer the data to be copied
**      len      -> Length of the data to be read
** 
*/
static int I2C_Read(unsigned char *out_buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit, 
    ** ACK/NACK and Stop condtions will be handled internally.
    */ 
    int ret = i2c_master_recv(etx_i2c_client_oled, out_buf, len);
    
    return ret;
}
 
/*
** This function is specific to the SSD_1306 OLED.
** This function sends the command/data to the OLED.
**
**  Arguments:
**      is_cmd -> true = command, flase = data
**      data   -> data to be written
** 
*/
static void SSD1306_Write(bool is_cmd, unsigned char data)
{
    unsigned char buf[2] = {0};
    int ret;
    
    /*
    ** First byte is always control byte. Data is followed after that.
    **
    ** There are two types of data in SSD_1306 OLED.
    ** 1. Command
    ** 2. Data
    **
    ** Control byte decides that the next byte is, command or data.
    **
    ** -------------------------------------------------------                        
    ** |              Control byte's | 6th bit  |   7th bit  |
    ** |-----------------------------|----------|------------|    
    ** |   Command                   |   0      |     0      |
    ** |-----------------------------|----------|------------|
    ** |   data                      |   1      |     0      |
    ** |-----------------------------|----------|------------|
    ** 
    ** Please refer the datasheet for more information. 
    **    
    */ 
    if( is_cmd == true )
    {
        buf[0] = 0x00;
    }
    else
    {
        buf[0] = 0x40;
    }
    
    buf[1] = data;
    
    ret = I2C_Write(buf, 2);
}
 
 
/*
** This function sends the commands that need to used to Initialize the OLED.
**
**  Arguments:
**      none
** 
*/
static int SSD1306_DisplayInit(void)
{
    msleep(100);               // delay
 
    /*
    ** Commands to initialize the SSD_1306 OLED Display
    */
    SSD1306_Write(true, 0xAE); // Entire Display OFF
    SSD1306_Write(true, 0xD5); // Set Display Clock Divide Ratio and Oscillator Frequency
    SSD1306_Write(true, 0x80); // Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended
    SSD1306_Write(true, 0xA8); // Set Multiplex Ratio
    SSD1306_Write(true, 0x3F); // 64 COM lines
    SSD1306_Write(true, 0xD3); // Set display offset
    SSD1306_Write(true, 0x00); // 0 offset
    SSD1306_Write(true, 0x40); // Set first line as the start line of the display
    SSD1306_Write(true, 0x8D); // Charge pump
    SSD1306_Write(true, 0x14); // Enable charge dump during display on
    SSD1306_Write(true, 0x20); // Set memory addressing mode
    SSD1306_Write(true, 0x00); // Horizontal addressing mode
    SSD1306_Write(true, 0xA1); // Set segment remap with column address 127 mapped to segment 0
    SSD1306_Write(true, 0xC8); // Set com output scan direction, scan from com63 to com 0
    SSD1306_Write(true, 0xDA); // Set com pins hardware configuration
    SSD1306_Write(true, 0x12); // Alternative com pin configuration, disable com left/right remap
    SSD1306_Write(true, 0x81); // Set contrast control
    SSD1306_Write(true, 0x80); // Set Contrast to 128
    SSD1306_Write(true, 0xD9); // Set pre-charge period
    SSD1306_Write(true, 0xF1); // Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK
    SSD1306_Write(true, 0xDB); // Set Vcomh deselect level
    SSD1306_Write(true, 0x20); // Vcomh deselect level ~ 0.77 Vcc
    SSD1306_Write(true, 0xA4); // Entire display ON, resume to RAM content display
    SSD1306_Write(true, 0xA6); // Set Display in Normal Mode, 1 = ON, 0 = OFF
    SSD1306_Write(true, 0x2E); // Deactivate scroll
    SSD1306_Write(true, 0xAF); // Display ON in normal mode
    
    return 0;
}
 
/*
** This function Fills the complete OLED with this data byte.
**
**  Arguments:
**      data  -> Data to be filled in the OLED
** 
*/
static void SSD1306_Fill(unsigned char data)
{
    unsigned int total  = 128 * 8;  // 8 pages x 128 segments x 8 bits of data
    unsigned int i      = 0;
    
    //Fill the Display
    for(i = 0; i < total; i++)
    {
        SSD1306_Write(false, data);
    }
}
 
/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int etx_oled_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    SSD1306_DisplayInit();
    
    //fill the OLED with this data
    SSD1306_Fill(0xFF);
 
    pr_info("OLED Probed!!!\n");
    
    return 0;
}
 
/*
** This function getting called when the slave has been removed
** Note : This will be called only once when we unload the driver.
*/
static int etx_oled_remove(struct i2c_client *client)
{   
    //fill the OLED with this data
    SSD1306_Fill(0x00);
    
    pr_info("OLED Removed!!!\n");
    return 0;
}
 
/*
** Structure that has slave device id
*/
static const struct i2c_device_id etx_oled_id[] = {
        { SLAVE_DEVICE_NAME, 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, etx_oled_id);
 
/*
** I2C driver Structure that has to be added to linux
*/
static struct i2c_driver etx_oled_driver = {
        .driver = {
            .name   = SLAVE_DEVICE_NAME,
            .owner  = THIS_MODULE,
        },
        .probe          = etx_oled_probe,
        .remove         = etx_oled_remove,
        .id_table       = etx_oled_id,
};
 
/*
** I2C Board Info strucutre
*/
static struct i2c_board_info oled_i2c_board_info = {
        I2C_BOARD_INFO(SLAVE_DEVICE_NAME, SSD1306_SLAVE_ADDR)
    };
 
/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
    int ret = -1;
    etx_i2c_adapter     = i2c_get_adapter(I2C_BUS_AVAILABLE);
    
    if( etx_i2c_adapter != NULL )
    {
        etx_i2c_client_oled = i2c_new_device(etx_i2c_adapter, &oled_i2c_board_info);
        
        if( etx_i2c_client_oled != NULL )
        {
            i2c_add_driver(&etx_oled_driver);
            ret = 0;
        }
        
        i2c_put_adapter(etx_i2c_adapter);
    }
    
    pr_info("Driver Added!!!\n");
    return ret;
}
 
/*
** Module Exit function
*/
static void __exit etx_driver_exit(void)
{
    i2c_unregister_device(etx_i2c_client_oled);
    i2c_del_driver(&etx_oled_driver);
    pr_info("Driver Removed!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <[email protected]>");
MODULE_DESCRIPTION("Simple I2C driver explanation (SSD_1306 OLED Display Interface)");
MODULE_VERSION("1.36");
Makefile
obj-m += driver_client.o
 
KDIR = /lib/modules/$(shell uname -r)/build
 
 
all:
    make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
    make -C $(KDIR)  M=$(shell pwd) clean
Testing the Device Driver
Build the driver by using Makefile (sudo make) in both bus and client driver directories.
Load the bus driver first using sudo insmod driver_bus.ko
Load the client driver using sudo insmod driver_client.ko
See the Display is filled.
For your understanding, I have given the I2C’s capture for the first command in SSD1306_DisplayInit() which is 0xAE.
Click here if you don’t see the timing diagram

real-i2c-bus-linux-driver-timing-diagram

Unload the driver using sudo rmmod driver_client
See the Display has been cleared.
Unload the driver using sudo rmmod driver_bus

##########################################################################################
I2C Bus Linux Device Driver using I2C-GPIO

Prerequisites
I2C Introduction – Part 1 (Basics)
I2C Introduction – Part 2 (Advanced Topics)
I2C Client Driver in Linux kernel
I2C Dummy Driver in Linux kernel
GPIO Linux Driver
I2C Bus driver using Bit banging

Hardware Required
Raspberry Pi
SSD1306 OLED I2C Display

Bring up Raspberry PI
Install Raspberry Pi OS (32-bit) with desktop in the SD card.
Then install the kernel header using sudo apt install raspberrypi-kernel-headers
For your information, In our Raspberry PI 4 board, kernel 5.4.51-v7l+ is installed.

Please enable the I2C in the Raspberry Pi.
-------------------------------------------------------------------------------------------
Introduction

- In our last tutorial, we have implemented everything using Bit banging right from the 
START condition, STOP condition, Read ACK/NACK, etc. 
- But we have not taken care of advanced I2C operations like arbitration, 
clock stretching, etc. 
- So even if we implement that we have to test it fully. Maybe our approach will take 
more time than normal.

- So in this tutorial, we are going to use "i2c-gpio driver" where all the START condition,
STOP condition, ACK/NACK, Arbitration, clock stretching, etc are implemented (bit-banging) 
and it has been tested fully as it is part of the Linux kernel. 
- So we will see how to use that. Let’s get started.
-------------------------------------------------------------------------------------------
APIs used for the I2C bus driver

struct i2c_algo_bit_data
/*
 * This struct contains the hw-dependent functions of bit-style adapters to
 * manipulate the line states, and to init any hw-specific features. This is
 * only used if you have more than one hw-type of adapter running.
 */
struct i2c_algo_bit_data {
    void *data;     /* private data for lowlevel routines */
    void (*setsda) (void *data, int state);
    void (*setscl) (void *data, int state);
    int  (*getsda) (void *data);
    int  (*getscl) (void *data);
    int  (*pre_xfer)  (struct i2c_adapter *);
    void (*post_xfer) (struct i2c_adapter *);

    /* local settings */
    int udelay;     /* half clock cycle time in us,
                   minimum 2 us for fast-mode I2C,
                   minimum 5 us for standard-mode I2C and SMBus,
                   maximum 50 us for SMBus */
    int timeout;        /* in jiffies */
    bool can_do_atomic; /* callbacks don't sleep, we can be atomic */
};

- If you see the structure, setscl, getscl, setsda, getsda are present. 
- We have to give our callback function to these members. For example, 
If the i2c-gpio driver wants to send the START condition, it needs to set the 
SCL/SDA GPIO to High/Low. 
- But it doesn’t know which GPIO we are using. 
- That’s why they have made this as a callback. 
See the example code given below to understand more.

data – private data (Optional)

pre_xfer – Function that will be called before i2C-gpio starts the I2C transfer  (Optional)

post_xfer – Function that will be called after i2C-gpio finishes the I2C transfer (Optional)

delay – delay in uS

timout – Timeout in jiffies

can_do_atomic – This will tells callback can sleep or not (Optional)

 Note: When you are using this method, you have to add this structure to the i2c_adapter structure’s algo_data member. Make sure that you have requested the GPIO before accessing it.
Example
#define ADAPTER_NAME     "ETX_I2C_ADAPTER"

#define SCL_GPIO  20    //GPIO act as SCL
#define SDA_GPIO  21    //GPIO act as SDA

/*
** Function to read the SCL GPIO
*/
static int ETX_I2C_Read_SCL(void *data)
{
  gpio_direction_input(SCL_GPIO);
  return gpio_get_value(SCL_GPIO);
}

/*
** Function to read the SDA GPIO
*/
static int ETX_I2C_Read_SDA(void *data)
{
  gpio_direction_input(SDA_GPIO);
  return gpio_get_value(SDA_GPIO);
}

/*
** Function to set the SCL GPIO
*/
static void ETX_I2C_Set_SCL(void *data, int state)
{
  gpio_direction_output(SCL_GPIO, state);
  gpio_set_value(SCL_GPIO, state);
}

/*
** Function to set the SDA GPIO
*/
static void ETX_I2C_Set_SDA(void *data, int state)
{
  gpio_direction_output(SDA_GPIO, state);
  gpio_set_value(SDA_GPIO, state);
}

/*
** I2C bit algorithm Structure
*/
struct i2c_algo_bit_data etx_bit_data = {
  .setsda = ETX_I2C_Set_SDA,
  .setscl = ETX_I2C_Set_SCL,
  .getscl = ETX_I2C_Read_SCL,
  .getsda = ETX_I2C_Read_SDA,
  .udelay = 5,
  .timeout = 100,       /* 100 ms */
};

/*
** I2C adapter Structure
*/
static struct i2c_adapter etx_i2c_adapter = {
  .owner      = THIS_MODULE,
  .class      = I2C_CLASS_HWMON | I2C_CLASS_SPD,
  .name       = ADAPTER_NAME,
  .algo_data  = &etx_bit_data,
  .nr         = 5,
};
i2c_bit_add_bus
This API is used to register the adapter to the subsystem. But this will assign the dynamic bus number.

int i2c_bit_add_bus (struct i2c_adapter * adapter);

where,
adapter – the adapter to add

It returns zero when a new bus number was allocated and stored in adap->nr, and the specified adapter became available for clients. Otherwise, a negative errno value is returned

i2c_bit_add_numbered_bus
This API is used to register the adapter to the subsystem. But it assigns the number that we asked for if only it is available. We have to initialize the member called nr in the i2c_adapter structure before calling this.

int i2c_bit_add_numbered_bus ( struct i2c_adapter * adap);

Where,
adap – the adapter to register (with adap->nr initialized)

This returns zero when the specified adapter is available for clients using the bus number provided in adap->nr. Otherwise, a negative errno value is returned.

I think we have discussed other APIs in our previous tutorials. So we can jump into the programming now.

Example Programming
In this tutorial, we will demonstrate the same example (last tutorial’s) which fills data in the display while loading the module and clears the display while unloading the module. The only difference from the last tutorial is the code that we are using in the bus driver.

Connection Diagram
SCL – GPIO 20
SDA – GPIO 21
I2C-bus-driver-ssd1306-interface-with-raspberry-pi

I2C Bus Driver Source Code
[Get the source code from GitHub]

/***************************************************************************//**
*  \file       driver_bus.c
*
*  \details    Simple I2C Bus driver explanation
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
* *******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/i2c-algo-bit.h>

#define ADAPTER_NAME     "ETX_I2C_ADAPTER"

#define SCL_GPIO  20    //GPIO act as SCL
#define SDA_GPIO  21    //GPIO act as SDA

/*
** Function to read the SCL GPIO
*/
static int ETX_I2C_Read_SCL(void *data)
{
  gpio_direction_input(SCL_GPIO);
  return gpio_get_value(SCL_GPIO);
}

/*
** Function to read the SDA GPIO
*/
static int ETX_I2C_Read_SDA(void *data)
{
  gpio_direction_input(SDA_GPIO);
  return gpio_get_value(SDA_GPIO);
}

/*
** Function to set the SCL GPIO
*/
static void ETX_I2C_Set_SCL(void *data, int state)
{
  gpio_direction_output(SCL_GPIO, state);
  gpio_set_value(SCL_GPIO, state);
}

/*
** Function to set the SDA GPIO
*/
static void ETX_I2C_Set_SDA(void *data, int state)
{
  gpio_direction_output(SDA_GPIO, state);
  gpio_set_value(SDA_GPIO, state);
}

/*
** Function to Initialize the GPIOs
*/
static int ETX_I2C_Init( void )
{
  int ret = 0;

  pr_info("In %s\n", __func__);
  do      //Break if any error
  {
    //Checking the SCL GPIO is valid or not
    if(gpio_is_valid(SCL_GPIO) == false){
          pr_err("SCL GPIO %d is not valid\n", SCL_GPIO);
          ret = -1;
          break;
    }

    //Checking the SDA GPIO is valid or not
    if(gpio_is_valid(SDA_GPIO) == false){
          pr_err("SDA GPIO %d is not valid\n", SDA_GPIO);
          ret = -1;
          break;
    }
    
    //Requesting the SCL GPIO
    if(gpio_request(SCL_GPIO,"SCL_GPIO") < 0){
          pr_err("ERROR: SCL GPIO %d request\n", SCL_GPIO);
          ret = -1;
          break;
    }

    //Requesting the SDA GPIO
    if(gpio_request(SDA_GPIO,"SDA_GPIO") < 0){
          pr_err("ERROR: SDA GPIO %d request\n", SDA_GPIO);
          //free already requested SCL GPIO
          gpio_free(SCL_GPIO);
          ret = -1;
          break;
    }
    
    /*
    ** configure the SCL GPIO as output, We will change the 
    ** direction later as per our need.
    */
    gpio_direction_output(SCL_GPIO, 1);

    /*
    ** configure the SDA GPIO as output, We will change the 
    ** direction later as per our need.
    */
    gpio_direction_output(SDA_GPIO, 1);

  } while(false);

  return ret;  
}

/*
** Function to Deinitialize the GPIOs
*/
static void ETX_I2C_DeInit( void )
{
  //free both the GPIOs
  gpio_free(SCL_GPIO);
  gpio_free(SDA_GPIO);
}

/*
** I2C bit algorithm Structure
*/
struct i2c_algo_bit_data etx_bit_data = {
  .setsda = ETX_I2C_Set_SDA,
  .setscl = ETX_I2C_Set_SCL,
  .getscl = ETX_I2C_Read_SCL,
  .getsda = ETX_I2C_Read_SDA,
  .udelay = 5,
  .timeout = 100,       /* 100 ms */
};

/*
** I2C adapter Structure
*/
static struct i2c_adapter etx_i2c_adapter = {
  .owner      = THIS_MODULE,
  .class      = I2C_CLASS_HWMON | I2C_CLASS_SPD,
  .name       = ADAPTER_NAME,
  .algo_data  = &etx_bit_data,
  .nr         = 5,
};


/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
  int ret = -1;
  
  ETX_I2C_Init();
  
  ret = i2c_bit_add_numbered_bus(&etx_i2c_adapter);

  pr_info("Bus Driver Added!!!\n");

  return ret;
}

/*
** Module Exit function
*/
static void __exit etx_driver_exit(void)
{
  ETX_I2C_DeInit();
  i2c_del_adapter(&etx_i2c_adapter);
  pr_info("Bus Driver Removed!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <[email protected]>");
MODULE_DESCRIPTION("Simple I2C Bus driver explanation");
MODULE_VERSION("1.39");
Makefile
obj-m += driver_bus.o
 
KDIR = /lib/modules/$(shell uname -r)/build
 
 
all:
    make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
    make -C $(KDIR)  M=$(shell pwd) clean
2C Client Device Driver Source code
There are no modifications in the client driver from the last tutorial.

[Get the source code from GitHub]

/***************************************************************************//**
*  \file       driver_client.c
*
*  \details    Simple I2C driver explanation (SSD_1306 OLED Display Interface)
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
* *******************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
 
#define I2C_BUS_AVAILABLE   (          5 )              // I2C Bus available in our Raspberry Pi
#define SLAVE_DEVICE_NAME   ( "ETX_OLED" )              // Device and Driver Name
#define SSD1306_SLAVE_ADDR  (       0x3C )              // SSD1306 OLED Slave Address
 
static struct i2c_adapter *etx_i2c_adapter     = NULL;  // I2C Adapter Structure
static struct i2c_client  *etx_i2c_client_oled = NULL;  // I2C Cient Structure (In our case it is OLED)
 
/*
** This function writes the data into the I2C client
**
**  Arguments:
**      buff -> buffer to be sent
**      len  -> Length of the data
**   
*/
static int I2C_Write(unsigned char *buf, unsigned int len)
{
  /*
  ** Sending Start condition, Slave address with R/W bit, 
  ** ACK/NACK and Stop condtions will be handled internally.
  */ 
  int ret = i2c_master_send(etx_i2c_client_oled, buf, len);
  
  return ret;
}
 
/*
** This function reads one byte of the data from the I2C client
**
**  Arguments:
**      out_buff -> buffer wherer the data to be copied
**      len      -> Length of the data to be read
** 
*/
static int I2C_Read(unsigned char *out_buf, unsigned int len)
{
  /*
  ** Sending Start condition, Slave address with R/W bit, 
  ** ACK/NACK and Stop condtions will be handled internally.
  */ 
  int ret = i2c_master_recv(etx_i2c_client_oled, out_buf, len);
  
  return ret;
}
 
/*
** This function is specific to the SSD_1306 OLED.
** This function sends the command/data to the OLED.
**
**  Arguments:
**      is_cmd -> true = command, flase = data
**      data   -> data to be written
** 
*/
static void SSD1306_Write(bool is_cmd, unsigned char data)
{
  unsigned char buf[2] = {0};
  int ret;
  
  /*
  ** First byte is always control byte. Data is followed after that.
  **
  ** There are two types of data in SSD_1306 OLED.
  ** 1. Command
  ** 2. Data
  **
  ** Control byte decides that the next byte is, command or data.
  **
  ** -------------------------------------------------------                        
  ** |              Control byte's | 6th bit  |   7th bit  |
  ** |-----------------------------|----------|------------|    
  ** |   Command                   |   0      |     0      |
  ** |-----------------------------|----------|------------|
  ** |   data                      |   1      |     0      |
  ** |-----------------------------|----------|------------|
  ** 
  ** Please refer the datasheet for more information. 
  **    
  */ 
  if( is_cmd == true )
  {
      buf[0] = 0x00;
  }
  else
  {
      buf[0] = 0x40;
  }
  
  buf[1] = data;
  
  ret = I2C_Write(buf, 2);
}
 
 
/*
** This function sends the commands that need to used to Initialize the OLED.
**
**  Arguments:
**      none
** 
*/
static int SSD1306_DisplayInit(void)
{
  msleep(100);               // delay

  /*
  ** Commands to initialize the SSD_1306 OLED Display
  */
  SSD1306_Write(true, 0xAE); // Entire Display OFF
  SSD1306_Write(true, 0xD5); // Set Display Clock Divide Ratio and Oscillator Frequency
  SSD1306_Write(true, 0x80); // Default Setting for Display Clock Divide Ratio and Oscillator Frequency that is recommended
  SSD1306_Write(true, 0xA8); // Set Multiplex Ratio
  SSD1306_Write(true, 0x3F); // 64 COM lines
  SSD1306_Write(true, 0xD3); // Set display offset
  SSD1306_Write(true, 0x00); // 0 offset
  SSD1306_Write(true, 0x40); // Set first line as the start line of the display
  SSD1306_Write(true, 0x8D); // Charge pump
  SSD1306_Write(true, 0x14); // Enable charge dump during display on
  SSD1306_Write(true, 0x20); // Set memory addressing mode
  SSD1306_Write(true, 0x00); // Horizontal addressing mode
  SSD1306_Write(true, 0xA1); // Set segment remap with column address 127 mapped to segment 0
  SSD1306_Write(true, 0xC8); // Set com output scan direction, scan from com63 to com 0
  SSD1306_Write(true, 0xDA); // Set com pins hardware configuration
  SSD1306_Write(true, 0x12); // Alternative com pin configuration, disable com left/right remap
  SSD1306_Write(true, 0x81); // Set contrast control
  SSD1306_Write(true, 0x80); // Set Contrast to 128
  SSD1306_Write(true, 0xD9); // Set pre-charge period
  SSD1306_Write(true, 0xF1); // Phase 1 period of 15 DCLK, Phase 2 period of 1 DCLK
  SSD1306_Write(true, 0xDB); // Set Vcomh deselect level
  SSD1306_Write(true, 0x20); // Vcomh deselect level ~ 0.77 Vcc
  SSD1306_Write(true, 0xA4); // Entire display ON, resume to RAM content display
  SSD1306_Write(true, 0xA6); // Set Display in Normal Mode, 1 = ON, 0 = OFF
  SSD1306_Write(true, 0x2E); // Deactivate scroll
  SSD1306_Write(true, 0xAF); // Display ON in normal mode
  
  return 0;
}
 
/*
** This function Fills the complete OLED with this data byte.
**
**  Arguments:
**      data  -> Data to be filled in the OLED
** 
*/
static void SSD1306_Fill(unsigned char data)
{
  unsigned int total  = 128 * 8;  // 8 pages x 128 segments x 8 bits of data
  unsigned int i      = 0;
  
  //Fill the Display
  for(i = 0; i < total; i++)
  {
      SSD1306_Write(false, data);
  }
}
 
/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int etx_oled_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
  SSD1306_DisplayInit();
  
  //fill the OLED with this data
  SSD1306_Fill(0xFF);

  pr_info("OLED Probed!!!\n");
  
  return 0;
}
 
/*
** This function getting called when the slave has been removed
** Note : This will be called only once when we unload the driver.
*/
static int etx_oled_remove(struct i2c_client *client)
{   
  //fill the OLED with this data
  SSD1306_Fill(0x00);
  
  pr_info("OLED Removed!!!\n");
  return 0;
}
 
/*
** Structure that has slave device id
*/
static const struct i2c_device_id etx_oled_id[] = {
  { SLAVE_DEVICE_NAME, 0 },
  { }
};
MODULE_DEVICE_TABLE(i2c, etx_oled_id);
 
/*
** I2C driver Structure that has to be added to linux
*/
static struct i2c_driver etx_oled_driver = {
  .driver = {
      .name   = SLAVE_DEVICE_NAME,
      .owner  = THIS_MODULE,
  },
  .probe          = etx_oled_probe,
  .remove         = etx_oled_remove,
  .id_table       = etx_oled_id,
};
 
/*
** I2C Board Info strucutre
*/
static struct i2c_board_info oled_i2c_board_info = {
  I2C_BOARD_INFO(SLAVE_DEVICE_NAME, SSD1306_SLAVE_ADDR)
};
 
/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
  int ret = -1;
  etx_i2c_adapter     = i2c_get_adapter(I2C_BUS_AVAILABLE);
  
  if( etx_i2c_adapter != NULL )
  {
      etx_i2c_client_oled = i2c_new_device(etx_i2c_adapter, &oled_i2c_board_info);
      
      if( etx_i2c_client_oled != NULL )
      {
          i2c_add_driver(&etx_oled_driver);
          ret = 0;
      }
      
      i2c_put_adapter(etx_i2c_adapter);
  }
  
  pr_info("Driver Added!!!\n");
  return ret;
}
 
/*
** Module Exit function
*/
static void __exit etx_driver_exit(void)
{
  i2c_unregister_device(etx_i2c_client_oled);
  i2c_del_driver(&etx_oled_driver);
  pr_info("Driver Removed!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <[email protected]>");
MODULE_DESCRIPTION("Simple I2C driver explanation (SSD_1306 OLED Display Interface)");
MODULE_VERSION("1.38");
Makefile
obj-m += driver_client.o
 
KDIR = /lib/modules/$(shell uname -r)/build
 
 
all:
    make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
    make -C $(KDIR)  M=$(shell pwd) clean
Testing the Device Driver
Build the driver by using Makefile (sudo make) in both bus and client driver directories.
Before loading the bus driver, we should load the i2c-gpio driver. Use sudo modprobe i2c-gpio to load the i2c-gpio driver.
Load the bus driver using sudo insmod driver_bus.ko
Load the client driver using sudo insmod driver_client.ko
See the Display is filled.
Unload the driver using sudo rmmod driver_client
See the Display has been cleared.
Unload the driver using sudo rmmod driver_bus
Unload the i2c-gpio using sudo modprobe -r i2c-gpio

##########################################################################################


##########################################################################################
Statically allocating device number:

dev_t dev = MKDEV(235, 0); //construct the device number in the form dev_t
register_chrdev_region(dev, 1, "Embetronicx_Dev");

- The device numbers allocated for the device file will be in : "/proc/devices" and "sysfs"

sudo mknod -m 666 /dev/etx_device c 246 0

##########################################################################################
Dynamically:

int alloc_chrdev_region(dev_t *dev,unsigned int firstminor,unsigned int count, char *name);

void unregister_chrdev_region(dev_t first, unsigned int count);
##########################################################################################

struct class * class_create(struct module *owner, const char *name);

void class_destroy (struct class * cls);

struct device *device_create(struct *class, struct device *parent, dev_t dev,
														 void * drvdata, const char *fmt, ...);

void device_destroy (struct class * class, dev_t devt);
##########################################################################################
##########################################################################################
##########################################################################################
##########################################################################################
##########################################################################################
##########################################################################################
##########################################################################################
##########################################################################################

##########################################################################################
										Allocating Major and Minor Number

- We can allocate the major and minor numbers in two ways.

1. Statically allocating
2. Dynamically Allocating
-----------------------------------------------------------------------------------------
Statically allocating
- If you want to set a particular major number for your driver, you can use this method. 
- This method will allocate that major number if it is available. Otherwise, it won’t.

int register_chrdev_region(dev_t first, unsigned int count, char *name);

first: is the beginning device number of the range you would like to allocate.
count: is the total number of contiguous device numbers you are requesting. 
- Note that, if the count is large, the range you request could spill over to the next 
major number; but everything will still work properly as long as the number range you 
request is available.
name: is the name of the device that should be associated with this number range; 
it will appear in "/proc/devices" and "sysfs"

- The return value from register_chrdev_region will be 0 if the allocation was successfully
performed. 
- In case of error, a negative error code will be returned, and you will not have access 
to the requested region.

-  The "dev_t" type (defined in <linux/types.h>) is used to hold device numbers—both the 
major and minor parts. 
- dev_t is a 32-bit quantity with 12 bits set aside for the "major number" and 
20 for the "minor number"
<Major, Minor> = <12-bits, 20-bits>
-----------------------------------------------------------------------------------------
- If you want to create the dev_t structure variable for your major and minor number, please use the below function.

MKDEV(int major, int minor);
-----------------------------------------------------------------------------------------
- If you want to get your major number and minor number from dev_t, use the below method.

MAJOR(dev_t dev);
MINOR(dev_t dev);
- If you pass the dev_t structure to this MAJOR or MONOR function, it will return that 
major/minor number of your driver.
-----------------------------------------------------------------------------------------
Example,
dev_t dev = MKDEV(235, 0);
register_chrdev_region(dev, 1, "Embetronicx_Dev");
-----------------------------------------------------------------------------------------
								
											Dynamically Allocating

- If we don’t want the fixed major and minor numbers please use this method. This method will allocate the major number dynamically to your driver which is available.

int alloc_chrdev_region(dev_t *dev,unsigned int firstminor,unsigned int count, char *name);

dev: is an output-only parameter that will, on successful completion, hold the first 
number in your allocated range.
firstminor: should be the requested first minor number to use; it is usually 0.
count: is the total number of contiguous device numbers you are requesting.
name: is the name of the device that should be associated with this number range; it will appear in /proc/devices and sysfs.
-----------------------------------------------------------------------------------------
Difference between static and dynamic method

- A static method is only really useful if you know in advance which major number you want 
to start with. 
- With the Static method, you are telling the kernel that, what device numbers you want 
(the start major/minor number and count) and it either gives them to you or 
not (depending on availability).

- With the Dynamic method, you are telling the kernel that how many device numbers you need
(the starting minor number and count) and it will find a starting major number for you, if 
one is available, of course.

- Partially to avoid conflict with other device drivers, it’s considered preferable to use 
the Dynamic method function, which will dynamically allocate the device numbers for you.

- The disadvantage of dynamic assignment is that you can’t create the device nodes in 
advance, because the major number assigned to your module will vary. 
- For normal use of the driver, this is hardly a problem, because once the number has been 
assigned, you can read it from "/proc/devices"
-----------------------------------------------------------------------------------------
Unregister the Major and Minor Number

- Regardless of how you allocate your device numbers, you should free them when they are 
no longer in use. Device numbers are freed with:

void unregister_chrdev_region(dev_t first, unsigned int count);

- The usual place to call unregister_chrdev_region would be in your module’s cleanup 
function (Exit Function).

##########################################################################################

						Device File Creation - Manual(mknod) and Automatic(sysfs)

##########################################################################################
- Manually Creating Device File
- We can create the device file manually by using mknod.

mknod -m <permissions> <name> <device type> <major> <minor>

<name> – your device file name that should have a full path (/dev/name)

<device type> – Put c or b

c – Character Device

b – Block Device

<major> – major number of your driver

<minor> – minor number of your driver

-m <permissions> – optional argument that sets the permission bits of the new device file to permissions

Example:

sudo mknod -m 666 /dev/etx_device c 246 0
- If you don’t want to give permission, You can also use chmod to set the permissions 
for a device file after creation.

Advantages
Anyone can create the device file using this method.
You can create the device file even before loading the driver.

##########################################################################################

										Automatically Creating Device File

- The automatic creation of device files can be handled with "udev". 
- Udev is the device manager for the Linux kernel that creates/removes device nodes in 
the /dev directory dynamically. Just follow the below steps.

1. Include the header file linux/device.h and linux/kdev_t.h
2. Create the struct Class
3. Create Device with the class which is created by the above step
4. Create the class

- This will create the struct class for our device driver. 
- It will create a structure under /sys/class/.
-----------------------------------------------------------------------------------------
struct class * class_create(struct module *owner, const char *name);

owner:pointer to the module that is to “own” this struct class
name:pointer to a string for the name of this class

- This is used to create a struct class pointer that can then be used in calls to 
class_device_create. 
- The return value can be checked using IS_ERR() macro.

- Note, the pointer created here is to be destroyed when finished by making a call to 
"class_destroy()".
-----------------------------------------------------------------------------------------
void class_destroy (struct class * cls);
-----------------------------------------------------------------------------------------
- Create Device
- This function can be used by char device classes. 
- A struct device will be created in sysfs, registered to the specified class.

struct device *device_create(struct *class, struct device *parent, dev_t dev, 
void * drvdata, const char *fmt, ...);

class:pointer to the struct class that this device should be registered to
parent:pointer to the parent struct device of this new device, if any
devt:the dev_t for the char device to be added
drvdata:the data to be added to the device for callbacks
fmt:string for the device’s name
... – variable arguments

- A “dev” file will be created, showing the dev_t for the device, if the dev_t is not 0,0. 
- If a pointer to a parent struct device is passed in, the newly created struct device 
will be a child of that device in sysfs. 
- The pointer to the struct device will be returned from the call. 
- Any further sysfs files that might be required can be created using this pointer. 
- The return value can be checked using IS_ERR() macro.

Note, you can destroy the device using "device_destroy()".
-----------------------------------------------------------------------------------------
void device_destroy (struct class * class, dev_t devt);

##########################################################################################

						Cdev structure and File Operations of Character drivers

- If we want to open, read, write, and close we need to register some structures to the 
driver.

cdev structure
- In Linux kernel struct inode structure is used to represent files. 
- Therefore, it is different from the file structure that represents an open file 
descriptor. There can be numerous file structures representing multiple open descriptors 
on a single file, but they all point to a single inode structure.

- The inode structure contains a great deal of information about the file. 
As a general rule, cdev structure is useful for writing driver code:

- struct cdev is one of the elements of the inode structure. 
- As you probably may know already, an inode structure is used by the kernel internally 
to represent files. 
- The struct cdev is the kernel’s internal structure that represents char devices.
- This field contains a pointer to that structure when the inode refers to a char device 
file.

struct cdev 
{
    struct kobject kobj; 
    struct module *owner; 
    const struct file_operations *ops; 
    struct list_head list; 
    dev_t dev; 
    unsigned int count; 
};
This is cdev structure. Here we need to fill the two fields,

file_operation: (This we will see after this cdev structure)
owner: (This should be THIS_MODULE)
-------------------------------------------------------------------------------------------
There are two ways of allocating and initializing one of these structures.

Runtime Allocation
Own allocation

- If you wish to obtain a standalone cdev structure at runtime, you may do so with code 
such as:

struct cdev *my_cdev = cdev_alloc( );
my_cdev->ops = &my_fops;

Or 
- else you can embed the cdev structure within a device-specific structure of your own by 
using the below function.
-------------------------------------------------------------------------------------------
void cdev_init(struct cdev *cdev, struct file_operations *fops);

- Once the cdev structure is set up with file_operations and owner, the final step is to 
tell the kernel about it with a call to:
-------------------------------------------------------------------------------------------
int cdev_add(struct cdev *dev, dev_t num, unsigned int count);

dev: is the cdev structure,
num: is the first device number to which this device responds, and
count: is the number of device numbers that should be associated with the device. 
Often count is one, but there are situations where it makes sense to have more than 
one device number correspond to a specific device.

- If this function returns a negative error code, your device has not been added to the 
system. So check the return value of this function.
-------------------------------------------------------------------------------------------
- After a call to cdev_add(), your device is immediately alive. All functions you defined 
(through the file_operations structure) can be called.
-------------------------------------------------------------------------------------------
To remove a char device from the system, call:

void cdev_del(struct cdev *dev);

Clearly, you should not access the cdev structure after passing it to cdev_del.
-------------------------------------------------------------------------------------------
File_Operations

- The file_operations structure is how a char driver sets up this connection. 
- The structure, (defined in <linux/fs.h>), is a collection of function pointers. 
- Each open file is associated with its own set of functions. 
- The operations are mostly in charge of implementing the system calls and are, 
therefore, named open, read, and so on.

- A file_operations structure is called fops. Each field in the structure must point to 
the function in the driver that implements a specific operation or have to left NULL for 
unsupported operations. The whole structure is mentioned below snippet.

struct file_operations 
{
    struct module *owner;
    loff_t (*llseek) (struct file *, loff_t, int);
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    ssize_t (*read_iter) (struct kiocb *, struct iov_iter *);
    ssize_t (*write_iter) (struct kiocb *, struct iov_iter *);
    int (*iterate) (struct file *, struct dir_context *);
    int (*iterate_shared) (struct file *, struct dir_context *);
    unsigned int (*poll) (struct file *, struct poll_table_struct *);
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
    long (*compat_ioctl) (struct file *, unsigned int, unsigned long);
    int (*mmap) (struct file *, struct vm_area_struct *);
    int (*open) (struct inode *, struct file *);
    int (*flush) (struct file *, fl_owner_t id);
    int (*release) (struct inode *, struct file *);
    int (*fsync) (struct file *, loff_t, loff_t, int datasync);
    int (*fasync) (int, struct file *, int);
    int (*lock) (struct file *, int, struct file_lock *);
    ssize_t (*sendpage) (struct file *, struct page *, int, size_t, loff_t *, int);
    unsigned long (*get_unmapped_area)(struct file *, unsigned long, unsigned long, unsigned long, unsigned long);
    int (*check_flags)(int);
    int (*flock) (struct file *, int, struct file_lock *);
    ssize_t (*splice_write)(struct pipe_inode_info *, struct file *, loff_t *, size_t, unsigned int);
    ssize_t (*splice_read)(struct file *, loff_t *, struct pipe_inode_info *, size_t, unsigned int);
    int (*setlease)(struct file *, long, struct file_lock **, void **);
    long (*fallocate)(struct file *file, int mode, loff_t offset,
              loff_t len);
    void (*show_fdinfo)(struct seq_file *m, struct file *f);
#ifndef CONFIG_MMU
    unsigned (*mmap_capabilities)(struct file *);
#endif
    ssize_t (*copy_file_range)(struct file *, loff_t, struct file *,
            loff_t, size_t, unsigned int);
    int (*clone_file_range)(struct file *, loff_t, struct file *, loff_t,
            u64);
    ssize_t (*dedupe_file_range)(struct file *, u64, u64, struct file *,
            u64);
};

- This file_operations structure contains many fields. But we will concentrate on very 
basic functions. Below we will see some fields explanation.
-------------------------------------------------------------------------------------------
struct module *owner:

- The first file_operations field is not an operation at all; it is a pointer to the 
module that “owns” the structure. 
- This field is used to prevent the module from being unloaded while its operations are in 
use. Almost all the time, it is simply initialized to THIS_MODULE, a macro defined in 
<linux/module.h>.
-------------------------------------------------------------------------------------------
read

ssize_t (*read) (struct file *, char _ _user *, size_t, loff_t *);

- This is used to retrieve data from the device. A null pointer in this position causes 
the read system call to fail with -EINVAL (“Invalid argument”). 
- A non-negative return value represents the number of bytes successfully read 
(the return value is a “signed size” type, usually the native integer type for the 
target platform).
-------------------------------------------------------------------------------------------
write

ssize_t (*write) (struct file *, const char _ _user *, size_t, loff_t *);

- It is used to sends the data to the device. If NULL -EINVAL is returned to the 
program calling the write system call. 
- The return value, if non-negative, represents the number of bytes successfully written.
-------------------------------------------------------------------------------------------
ioctl

int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);

- The ioctl system call offers a way to issue device-specific commands (such as formatting 
a track of a floppy disk, which is neither reading nor writing). 
- Additionally, a few ioctl commands are recognized by the kernel without referring to the 
fops table. 
- If the device doesn’t provide an ioctl method, the system call returns an error for any 
request that isn’t predefined (-ENOTTY, “No such ioctl for device”). 
-------------------------------------------------------------------------------------------
Open

int (*open) (struct inode *, struct file *);

- Though this is always the first operation performed on the device file, the driver is 
not required to declare a corresponding method. 
- If this entry is NULL, opening the device always succeeds, but your driver isn’t notified
-------------------------------------------------------------------------------------------
release (close)

int (*release) (struct inode *, struct file *);

- This operation is invoked when the file structure is being released. 
Like open, release can be NULL.
-------------------------------------------------------------------------------------------
Example
static struct file_operations fops =
{
.owner          = THIS_MODULE,
.read           = etx_read,
.write          = etx_write,
.open           = etx_open,
.release        = etx_release,
};

##########################################################################################

- We already know that in Linux everything is a File.

1. User Space application (User program)
2. Kernel Space program (Driver)

- The user Program will communicate with the kernel space program using the device file. 
Lets Start.
-------------------------------------------------------------------------------------------
Kernel Space Program (Device Driver)

- We already know about major, and minor numbers, device files, and file operations of the 
device drivers. 

Open driver
Write Driver
Read Driver
Close Driver
-------------------------------------------------------------------------------------------
Concept
- Using this driver we can send strings or data to the kernel device driver using the
 write function. 
- It will store that string in the kernel space. Then when I read the device file, it will 
send the data which is written by write by function to the userspace application.
-------------------------------------------------------------------------------------------
Functions used in this driver
kmalloc()
kfree()
copy_from_user()
copy_to_user()
kmalloc()
-------------------------------------------------------------------------------------------

- kmalloc function is used to allocate the memory in kernel space. This is like a malloc() 
function in userspace. 
- The function is fast (unless it blocks) and doesn’t clear the memory it obtains. 
- The allocated region still holds its previous content. The allocated region is also 
contiguous in physical memory.

#include <linux/slab.h>

void *kmalloc(size_t size, gfp_t flags);

size:how many bytes of memory are required.
flags: the type of memory to allocate.

The flags argument may be one of:

GFP_USER:Allocate memory on behalf of the user. May sleep.
GFP_KERNEL:Allocate normal kernel ram. May sleep.
GFP_ATOMIC:Allocation will not sleep. May use emergency pools. 
For example, use this inside interrupt handler.
GFP_HIGHUSER:Allocate pages from high memory.
GFP_NOIO:Do not do any I/O at all while trying to get memory.
GFP_NOFS:Do not make any fs calls while trying to get memory.
GFP_NOWAIT:Allocation will not sleep.
__GFP_THISNODE:Allocate node-local memory only.
GFP_DMA:Allocation is suitable for DMA. Should only be used for kmalloc caches. 
Otherwise, use a slab created with SLAB_DMA.

- Also, it is possible to set different flags by OR’ing in one or more of the following 
additional flags:

__GFP_COLD:Request cache-cold pages instead of trying to return cache-warm pages.
__GFP_HIGH:This allocation has high priority and may use emergency pools.
__GFP_NOFAIL:Indicate that this allocation is in no way allowed to fail (think twice 
before using).
__GFP_NORETRY:If memory is not immediately available, then give up at once.
__GFP_NOWARN:If allocation fails, don’t issue any warnings.
__GFP_REPEAT:If allocation fails initially, try once more before failing.

- There are other flags available as well, but these are not intended for general use, 
and so are not documented here. For a full list of potential flags, always refer to 
linux/gfp.h.
-------------------------------------------------------------------------------------------
kfree()

This is like a free() function in the userspace. This is used to free the previously 
allocated memory.

void kfree(const void *objp)

*objp – pointer returned by kmalloc
-------------------------------------------------------------------------------------------
copy_from_user()

- This function is used to Copy a block of data from user space (Copy data from user 
space to kernel space).

unsigned long copy_from_user(void *to, const void __user *from, unsigned long  n);

to:Destination address, in the kernel space
from:The source address in the user space
n:Number of bytes to copy

- Returns number of bytes that could not be copied. On success, this will be zero.
-------------------------------------------------------------------------------------------
copy_to_user()

- This function is used to Copy a block of data into userspace (Copy data from kernel 
space to user space).

unsigned long copy_to_user(const void __user *to, const void *from, unsigned long  n);

to:Destination address, in the user space
from:The source address in the kernel space
n:Number of bytes to copy

- Returns number of bytes that could not be copied. On success, this will be zero.
-------------------------------------------------------------------------------------------
Open()

- This function is called first, whenever we are opening the device file. In this function,
I am going to allocate the memory using kmalloc. In the userspace application, you can use 
open() system call to open the device file.

static int etx_open(struct inode *inode, struct file *file)
{
        /*Creating Physical memory*/
        if((kernel_buffer = kmalloc(mem_size , GFP_KERNEL)) == 0){
            printk(KERN_INFO "Cannot allocate memory in kernel\n");
            return -1;
        }
        printk(KERN_INFO "Device File Opened...!!!\n");
        return 0;
}
-------------------------------------------------------------------------------------------
write()

- When writing the data to the device file it will call this write function. Here I will 
copy the data from user space to kernel space using copy_from_user() function. 
- In the userspace application, you can use write() system call to write any data the 
device file.

static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        copy_from_user(kernel_buffer, buf, len);
        printk(KERN_INFO "Data Write : Done!\n");
        return len;
}
-------------------------------------------------------------------------------------------
read()

- When we read the device file it will call this function. In this function, 
I used copy_to_user(). 
- This function is used to copy the data to the userspace application. In the userspace 
application, you can use read() system call to read the data from the device file.

static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        copy_to_user(buf, kernel_buffer, mem_size);
        printk(KERN_INFO "Data Read : Done!\n");
        return mem_size;
}
-------------------------------------------------------------------------------------------
close()
- When we close the device file that will call this function. Here I will free the memory 
that is allocated by kmalloc using kfree(). 
- In the userspace application, you can use close() system call to close the device file.

static int etx_release(struct inode *inode, struct file *file)
{
        kfree(kernel_buffer);
        printk(KERN_INFO "Device File Closed...!!!\n");
        return 0;
}

##########################################################################################

													IOCTL

- The operating system segregates virtual memory into kernel space and userspace.  
- Kernel space is strictly reserved for running the kernel, kernel extensions, and most 
device drivers. 
- In contrast, user space is the memory area where all user-mode applications work, 
and this memory can be swapped out when necessary. 

- There are many ways to Communicate between the Userspace and Kernel Space, they are:
1. IOCTL
2. Procfs
3. Sysfs
4. Configfs
5. Debugfs
6. Sysctl
7. UDP Sockets
8. Netlink Sockets

In this tutorial, we will see IOCTL.
-------------------------------------------------------------------------------------------
													IOCTL in Linux

- IOCTL is referred to as Input and Output Control, which is used to talk to device drivers
- This system call is available in most driver categories.  The major use of this is in 
case of handling some specific operations of a device for which the kernel does not have 
a system call by default.

- Some real-time applications of ioctl are Ejecting the media from a “cd” drive, changing 
the Baud Rate of Serial port, Adjusting the Volume, Reading or Writing device registers, 
etc. We already have the write and read function in our device driver. 
But it is not enough for all cases.
-------------------------------------------------------------------------------------------
Steps involved in IOCTL

There are some steps involved to use IOCTL.

1. Create IOCTL command in the driver
2. Write IOCTL function in the driver
3. Create IOCTL command in a Userspace application
4. Use the IOCTL system call in a Userspace
5. Create IOCTL Command in the Driver

To implement a new ioctl command we need to follow the following steps.

1. Define the ioctl command

#define "ioctl name" __IOX("magic number","command number","argument type")

IOX:
“IO“: an ioctl with no parameters
“IOW“: an ioctl with write parameters (copy_from_user)
“IOR“: an ioctl with read parameters (copy_to_user)
“IOWR“: an ioctl with both write and read parameters

Magic_Number: is a unique number or character that will differentiate our set of ioctl 
calls from the other ioctl calls. 
- some times the major number for the device is used here.
Command_Number: is the number that is assigned to the ioctl. This is used to differentiate 
the commands from one another.
argument_type: The last is the type of data.
-------------------------------------------------------------------------------------------
2. Add the header file linux/ioctl.h to make use of the above-mentioned calls.

Example
#include <linux/ioctl.h>

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
-------------------------------------------------------------------------------------------
Write IOCTL function in the driver

- The next step is to implement the ioctl call we defined into the corresponding driver. 
We need to add the ioctl function to our driver. Find the prototype of the function below.

int  ioctl(struct inode *inode,struct file *file,unsigned int cmd,unsigned long arg)

<inode>: is the inode number of the file being worked on.
<file>: is the file pointer to the file that was passed by the application.
<cmd>: is the ioctl command that was called from the userspace.
<arg>: are the arguments passed from the userspace

- Within the function “ioctl” we need to implement all the commands that we defined above 
(WR_VALUE, RD_VALUE). 
- We need to use the same commands in the switch statement which is defined above.

- Then we need to inform the kernel that the ioctl calls are implemented in the function 
“etx_ioctl“. 
- This is done by making the fops pointer “unlocked_ioctl” to point to “etx_ioctl” as 
shown below.

Example
/*
** This function will be called when we write IOCTL on the Device file
*/
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case WR_VALUE:
                        if( copy_from_user(&value ,(int32_t*) arg, sizeof(value)) )
                        {
                                pr_err("Data Write : Err!\n");
                        }
                        pr_info("Value = %d\n", value);
                        break;
                case RD_VALUE:
                        if( copy_to_user((int32_t*) arg, &value, sizeof(value)) )
                        {
                                pr_err("Data Read : Err!\n");
                        }
                        break;
                default:
                        pr_info("Default\n");
                        break;
        }
        return 0;
}

/*
** File operation sturcture
*/
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = etx_read,
        .write          = etx_write,
        .open           = etx_open,
        .unlocked_ioctl = etx_ioctl,
        .release        = etx_release,
};
-------------------------------------------------------------------------------------------
- Now we need to call the ioctl command from a user application.

Create IOCTL command in a Userspace application
Just define the ioctl command like how we defined it in the driver.

Example:

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

Use IOCTL system call in Userspace
- Include the header file <sys/ioctl.h>.Now we need to call the new ioctl command from a 
user application.

long ioctl( "file descriptor","ioctl command","Arguments");

<file descriptor>: This the open file on which the ioctl command needs to be executed, 
which would generally be device files.
<ioctl command>: ioctl command which is implemented to achieve the desired functionality
<arguments>: The arguments need to be passed to the ioctl command.

Example
ioctl(fd, WR_VALUE, (int32_t*) &number); 

ioctl(fd, RD_VALUE, (int32_t*) &value);

##########################################################################################


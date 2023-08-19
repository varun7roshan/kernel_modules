##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################
														Procfs in Linux

- Many or most Linux users have at least heard of proc. Some of you may wonder why this 
folder is so important.

- On the root, there is a folder titled “proc”. This folder is not really on /dev/sda1 or 
where ever you think the folder resides. 
- This folder is a mount point for the procfs (Process Filesystem) which is a filesystem 
in memory. 
- Many processes store information about themselves on this virtual filesystem. 
- ProcFS also stores other system information.

- It can act as a bridge connecting the user space and the kernel space. 
- Userspace programs can use proc files to read the information exported by the kernel. 
- Every entry in the proc file system provides some information from the kernel.
-------------------------------------------------------------------------------------------
- The entry "meminfo"  gives the details of the memory being used in the system.

To read the data in this entry just run
cat /proc/meminfo
-------------------------------------------------------------------------------------------
- Similarly the "modules" entry gives details of all the modules that are currently a 
part of the kernel.

cat /proc/modules

- It gives similar information as lsmod. Like this more, proc entries are there.
-------------------------------------------------------------------------------------------

/proc/devices — registered character and block major numbers

/proc/iomem — on-system physical RAM and bus device addresses

/proc/ioports — on-system I/O port addresses (especially for x86 systems)

/proc/interrupts — registered interrupt request numbers

/proc/softirqs — registered soft IRQs

/proc/swaps — currently active swaps

/proc/kallsyms — running kernel symbols, including from loaded modules

/proc/partitions — currently connected block devices and their partitions

/proc/filesystems — currently active filesystem drivers

"/proc/cpuinfo" — information about the CPU(s) on the system
-------------------------------------------------------------------------------------------

- Most proc files are read-only and only expose kernel information to user space programs.

- proc files can also be used to control and modify kernel behavior on the fly. 
- The proc files need to be writable in this case.

For example, to enable IP forwarding of iptable, one can use the command below,

echo 1 > /proc/sys/net/ipv4/ip_forward
-------------------------------------------------------------------------------------------
- The proc file system is also very useful when we want to debug a kernel module. 
- While debugging we might want to know the values of various variables in the module or 
maybe the data that the module is handling. 
- In such situations, we can create a proc entry for ourselves and dump whatever data we 
want to look into in the entry.

- We will be using the same example character driver that we created in the previous post 
to create the proc entry.

- The proc entry can also be used to pass data to the kernel by writing into the kernel, 
so there can be two kinds of proc entries.

1. An entry that only reads data from the kernel space.
2. An entry that reads as well as writes data into and from kernel space.
-------------------------------------------------------------------------------------------
Creating procfs directory

- You can create the directory under /proc/*  */using the below API.

struct proc_dir_entry *proc_mkdir(const char *name, struct proc_dir_entry *parent)

name: The name of the directory that will be created under /proc.
parent: In case the folder needs to be created in a subfolder under /proc a pointer to the 
same is passed else it can be left as NULL.
-------------------------------------------------------------------------------------------
Creating Procfs Entry

- The creation of proc entries has undergone a considerable change in kernel version 3.10 
and above. 
- In this post, we will see one of the methods we can use in Linux kernel version 3.10. 
- Let us see how we can create proc entries in version 3.10.
-------------------------------------------------------------------------------------------
- Above we just created a directory in procfs - now we need to create files in it 
>> We have to give the parent name - directory name which was used above in "proc_mkdir()"
>> If we pass NULL in parent arg here - then the file - entry is going to sit in "/proc"

struct proc_dir_entry *proc_create(const char *name, umode_t mode, 
																	 struct proc_dir_entry *parent, 
                                   const struct file_operations *proc_fops);

The function is defined in proc_fs.h

name: The name of the proc entry
mode: The access mode for proc entry
parent: The name of the parent directory under /proc. If NULL is passed as a parent, 
the /proc directory will be set as a parent.
proc_fops: The structure in which the file operations for the proc entry will be created.

Note: The above proc_create is valid in the Linux Kernel v3.10 to v5.5. From v5.6, there 
is a change in this API. 
- The fourth argument const struct file_operations *proc_fops is changed to 
const struct proc_ops *proc_ops.
-------------------------------------------------------------------------------------------

- For example to create a proc entry by the name “etx_proc” under /proc the above function 
will be defined as below,

proc_create("etx_proc",0666,NULL,&proc_fops);

- This proc entry should be created in the Driver init function.
-------------------------------------------------------------------------------------------
- If you are using the kernel version "below 3.10", please use the below functions to 
create proc entry.

create_proc_read_entry()
create_proc_entry() 

- Both of these functions are defined in the file linux/proc_fs.h.

- The "create_proc_entry" is a generic function that allows creating both the read as 
well as the write entries.

- "create_proc_read_entry" is a function specific to create only read entries.

- It is possible that most of the proc entries are created to read data from the kernel 
space that is why the kernel developers have provided a direct function to create a 
read proc entry.
-------------------------------------------------------------------------------------------
Procfs File System

- Now we need to create file_operations structure proc_fops in which we can map the read 
and write functions for the proc entry.

static struct file_operations proc_fops = 
{
    .open = open_proc,
    .read = read_proc,
    .write = write_proc,
    .release = release_proc
};

- This is like a device driver file system. We need to register our proc entry filesystem. 
- If you are using the kernel version "below 3.10", "this will not work" 
- If you are using the Linux kernel "v5.6 and above", you "should not use this structure". 
- You have to use struct proc_ops instead of struct file_operationslike below.

static struct proc_ops proc_fops = 
{
        .proc_open = open_proc,
        .proc_read = read_proc,
        .proc_write = write_proc,
        .proc_release = release_proc
};

Next, we need to add all functions to the driver.
-------------------------------------------------------------------------------------------
Open and Release Function

These functions are optional.

static int open_proc(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "proc file opend.....\t");
    return 0;
}

static int release_proc(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "proc file released.....\n");
    return 0;
}
-------------------------------------------------------------------------------------------
Write Function

- The write function will receive data from the user space using the function 
copy_from_user into an array “etx_array”.

Thus the write function will look as below.

static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    printk(KERN_INFO "proc file write.....\t");
    copy_from_user(etx_array,buff,len);
    return len;
}
-------------------------------------------------------------------------------------------
Read Function

- Once data is written to the proc entry we can read from the proc entry using a read 
function, i.e transfer data to the user space using the function copy_to_user function.

The read function can be as below
static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset)
{
    printk(KERN_INFO "proc file read.....\n");
    if(len)
        len=0;
    else{
        len=1;
        return 0;
    }
    copy_to_user(buffer,etx_array,20);

    return length;;
}
-------------------------------------------------------------------------------------------
Remove Proc Entry

- Proc entry should be removed in the Driver exit function using the below function.

void remove_proc_entry(const char *name, struct proc_dir_entry *parent);

Example:
remove_proc_entry("etx_proc",NULL);

- And you can remove the complete parent directory using 
proc_remove(struct proc_dir_entry *parent).

##########################################################################################


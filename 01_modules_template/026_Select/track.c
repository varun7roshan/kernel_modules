##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################
Prerequisites

- Poll in Linux Device Driver
- SysFs in Linux Device Driver
- Wait queue in Linux Device Driver
- Select Linux Example Device Driver
- IO Multiplexing
------------------------------------------------------------------------------------------
The solution is to use a kernel mechanism for polling over a set of file descriptors. There are 3 options you can use in Linux:

1. poll
>> file_operations.poll() - here we will call poll_wait() passing wait-queue - will be 
blocked till someone calls wake_up() for this wait-queue and will return with Flags
>> BETTER TO USE - poll() than select()

2. select

3. epoll
------------------------------------------------------------------------------------------
In this tutorial, we will focus on the select Linux example

What is a select and its use "?"
- The select API should have been deprecated years ago. But it is still there. 
"Select provides the same functionality as the poll"
------------------------------------------------------------------------------------------
How to use a select in the Application "?"

- Before we see the select API, we must be familiar with some other macros. 
- We are going to watch the file descriptors for reading, writing, and error events.
- So we have to pass the file descriptors to the select API. 
- The contents of a file descriptor set can be manipulated using the following macros:

FD_ZERO()
FD_SET()
FD_CLR()
FD_ISSET()
FD_ZERO()

- This macro clears (removes all file descriptors from) the set. 
- It should be employed as the first step in initializing a file descriptor set.
------------------------------------------------------------------------------------------
FD_SET()

- This macro adds the file descriptor fd to the set. 
- Adding a file descriptor that is already present in the set is a no-op, and does not 
produce an error.
------------------------------------------------------------------------------------------
FD_CLR()

- This macro removes the file descriptor fd from the set. 
- Removing a file descriptor that is not present in the set is a no-op, and does not 
produce an error.
------------------------------------------------------------------------------------------
FD_ISSET()

- select modifies the contents of the sets according to the rules described below.

- After calling select(), the FD_ISSET() macro can be used to test if a file descriptor 
is still present in a set. 
- FD_ISSET() returns nonzero if the file descriptor fd is present in set, and zero 
if it is not.
------------------------------------------------------------------------------------------
Select()

- Now we are good to go. Let’s see the select API below.

int select( int nfds, fd_set *readfds,  fd_set *writefds,  fd_set *exceptfds, 
						struct timeval *timeout );

nfds: – This argument should be set to the highest-numbered file descriptor in any of the 
three sets, plus 1. 
- The indicated file descriptors in each set are checked, up to this limit.

- For example, if you want to monitor the two file descriptors of 26 and 5. 
- Then you have to provide 26 + 1 = 27, not 2. 
- So, highest-numbered file descriptor + 1.

- Note : select() can monitor only file descriptors numbers that are less than 
FD_SETSIZE which is 1024.

readfds: – The file descriptors in this set are watched to see if they are ready for 
reading. 
- After select() has returned, readfds will be cleared of all file descriptors except 
for those that are ready for reading.

writefds: – The file descriptors in this set are watched to see if they are ready for 
writing. 
- After select() has returned, writefds will be cleared of all file descriptors except 
for those that are ready for writing.

exceptfds: – The file descriptors in this set are watched for "exceptional conditions". 
- For example, POLLPRI in the poll. 
- After select() has returned, exceptfds will be cleared of all file descriptors 
except for those for which an exceptional condition has occurred.

timeout: – The timeout argument is a timeval structure that specifies the interval that 
select() should block waiting for a file descriptor to become ready. 

The call will block until either:
1. a file descriptor becomes ready
2. the call is interrupted by a signal handler, or
3. the timeout expires.

- If both fields of the timeval structure are zero, then select() returns immediately. 
(This is useful for polling.)

- If timeout is specified as NULL, select() blocks indefinitely waiting for a file 
descriptor to become ready.
------------------------------------------------------------------------------------------
What is the difference between select and poll (select vs poll) "?"

- select was introduced in BSD Unix, released in August 1983, whereas poll was introduced 
in SVR3 Unix, released in 1986.

- Operations in poll and select is linear and slow because of having a lot of checks.

- "The main advantage of select is the fact that it is very portable – every UNIX-like "
"OS has it. Some Unix systems do not support poll"

- With select, the file descriptor sets are reconstructed on return, so each subsequent 
call must reinitialize them. 

- The poll system call separates the input (events field) from the output (revents field), 
allowing the array to be reused without change.

- The timeout parameter to select is undefined on return. 
- So, we need to reinitialize it.

- poll does not require the user to calculate the value of the highest-numbered file 
descriptor +1. 
- Whereas in select, we have to calculate the nfds or pass the maximum number using 
FD_SETSIZE.

- "poll is more efficient for large-valued file descriptors". 
- For example, when you want to wait for the events in the file descriptor (fd) 1000, 
poll will directly use that fd. 

************************** Major in-efficiency *********************************

- But if you use this 1000 fd in select, it has to iterate (loop) from 0 to 999 to check 
any file has data.

- In the select, we need to iterate over the file descriptors to check if it exists on 
the set returned from select. 
- But in the poll, we can check only we wanted to.

- In the poll, you can use the file descriptor which is more than 1024. 
- But in select you cannot use it. If you use the file descriptor which is more than or 
equal to 1024 (FD_SETSIZE), then it may destroy your stack. 
- How "?" select’s file descriptor sets are statically sized.

- If you see the structure fd_set‘s declaration, it looks like the below.

/* fd_set for select and pselect.  */
typedef struct
{
  /* XPG4.2 requires this member name.  Otherwise avoid the name
     from the global namespace.  */
#ifdef __USE_XOPEN
  __fd_mask fds_bits[__FD_SETSIZE / __NFDBITS];
# define __FDS_BITS(set) ((set)->fds_bits)
#else
  __fd_mask __fds_bits[__FD_SETSIZE / __NFDBITS];
# define __FDS_BITS(set) ((set)->__fds_bits)
#endif
} fd_set;

- Don’t get confused by seeing the above code. Actually, we can simplify the above 
structure like the below.

/* fd_set for select and pselect.  */
typedef struct
{
  long fds_bits[1024 / 64];  //__FD_SETSIZE = 1024, __NFDBITS = 8 * 8
} fd_set;

- So, if you see the above-simplified structure, it allocates (8 * 16) bytes. 
- Which are 1024 bits. 
- So, you can maximum set or unset until 1024 bits. 
- What will happen if you are trying to access more than that "?" 
- it will corrupt other memory which is not intended to. 
- So, it will destroy the stack.
------------------------------------------------------------------------------------------
How to use a select in the Linux Device Driver "?"

- We have already implemented the poll functionality in the Linux device driver side by 
our previous tutorial. 
- So, we can use the same Linux device driver for select() too as 
"select()" also calls the same "poll_wait()" in the Linux kernel.
------------------------------------------------------------------------------------------
Select Linux Example Programming

- I have not made any changes to that Linux device driver source code. And, the concept 
is also the same as the previous tutorial.

- I’ve just copy-pasted the concept below from the previous tutorial for your understanding

- This example is just for educational purposes and explains an select in Linux device 
driver. This might not be the real use case.
------------------------------------------------------------------------------------------
- In this example, I’ve created one sysfs entry (/sys/kernel/etx_sysfs/etx_value) with the 
reading and writing option. 
- The select Linux example driver and application supposed to work like below.
------------------------------------------------------------------------------------------
Driver Concept

- When you write data using the sysfs entry (/sys/kernel/etx_sysfs/etx_value), that means data is available in the kernel. So, we have to inform the userspace application to read the available data (Driver gives the permission to the app for reading).
- When you read the data using the sysfs entry (/sys/kernel/etx_sysfs/etx_value), that means, data has been read by the kernel, and the userspace app has to write the data into the kernel space. So that driver can read the data. (Driver gives the permission to the app for writing).
- So, whenever the app gets read permission, it can read the data from the driver and whenever the app gets the write permission, it can write some data into the driver.
------------------------------------------------------------------------------------------
Application concept

- The application will open the driver’s device file (/dev/etx_device).
- Clear the read and write fd using FD_ZERO().
- Then set that descriptor to the read fd and write fd using FD_SET().
- Set the timeout value in the struct timeval. 
- In our example, we are setting 5 seconds timeout.
- Then call the select function. So, every 5 seconds it will exit the select and 
again it calls the select.
- If this app gets the read permission from the Linux kernel driver, then it reads the data.
- If this app gets the write permission from the Linux kernel driver, then it writes 
the data to the driver.
------------------------------------------------------------------------------------------
Select Linux Example – Driver Source Code
[Get the full updated source code from GitHub]

/***************************************************************************//**
*  \file       poll_driver.c
*
*  \details    Poll driver 
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
* *****************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>
#include <linux/wait.h>                 //Required for the wait queues
#include <linux/poll.h>
#include <linux/sysfs.h> 
#include <linux/kobject.h>
#include <linux/err.h>
//Waitqueue
DECLARE_WAIT_QUEUE_HEAD(wait_queue_etx_data);

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
struct kobject *kobj_ref;

static bool can_write = false;
static bool can_read  = false;
static char etx_value[20];

/*
** Function Prototypes
*/
static int      __init etx_driver_init(void);
static void     __exit etx_driver_exit(void);
 
/*************** Driver Fuctions **********************/
static int      etx_open(struct inode *inode, struct file *file);
static int      etx_release(struct inode *inode, struct file *file);
static ssize_t  etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static unsigned int etx_poll(struct file *filp, struct poll_table_struct *wait);

/*************** Sysfs Fuctions **********************/
static ssize_t  sysfs_show(struct kobject *kobj, 
                        struct kobj_attribute *attr, char *buf);
static ssize_t  sysfs_store(struct kobject *kobj, 
                        struct kobj_attribute *attr,const char *buf, size_t count);
struct kobj_attribute etx_attr = __ATTR(etx_value, 0660, sysfs_show, sysfs_store);

/*
** File operation sturcture
*/
static struct file_operations fops =
{
  .owner          = THIS_MODULE,
  .read           = etx_read,
  .write          = etx_write,
  .open           = etx_open,
  .release        = etx_release,
  .poll           = etx_poll
};

/*
** This function will be called when we read the sysfs file
*/
static ssize_t sysfs_show(struct kobject *kobj, 
                          struct kobj_attribute *attr, 
                          char *buf)
{
  pr_info("Sysfs Show - Write Permission Granted!!!\n");
  
  can_write = true;
  
  //wake up the waitqueue
  wake_up(&wait_queue_etx_data);
  
  return sprintf(buf, "%s", "Success\n");
}

/*
** This function will be called when we write the sysfsfs file
*/
static ssize_t sysfs_store(struct kobject *kobj, 
                           struct kobj_attribute *attr,
                           const char *buf, 
                           size_t count)
{
  pr_info("Sysfs Store - Read Permission Granted!!!\n");
  
  strcpy(etx_value, buf);
  
  can_read = true;

  //wake up the waitqueue
  wake_up(&wait_queue_etx_data);
  
  return count;
}

/*
** This function will be called when we open the Device file
*/
static int etx_open(struct inode *inode, struct file *file)
{
  pr_info("Device File Opened...!!!\n");
  return 0;
}

/*
** This function will be called when we close the Device file
*/
static int etx_release(struct inode *inode, struct file *file)
{
  pr_info("Device File Closed...!!!\n");
  return 0;
}

/*
** This function will be called when we read the Device file
*/
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
  pr_info("Read Function : etx_value = %s\n",etx_value);   
  
  len = strlen(etx_value);
  
  strcpy(buf, etx_value);
  
#if 0  
  if( copy_to_user(buf, etx_value, len) > 0)
  {
    pr_err("ERROR: Not all the bytes have been copied to user\n");
  }
#endif
  
  return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
  strcpy(etx_value, buf);
  
  pr_info("Write function : etx_value = %s\n", etx_value);

  return len;
}

/*
** This function will be called when app calls the poll function
*/
static unsigned int etx_poll(struct file *filp, struct poll_table_struct *wait)
{
  __poll_t mask = 0;
  
  poll_wait(filp, &wait_queue_etx_data, wait);
  pr_info("Poll function\n");
  
  if( can_read )
  {
    can_read = false;
    mask |= ( POLLIN | POLLRDNORM );
  }
  
  if( can_write )
  {
    can_write = false;
    mask |= ( POLLOUT | POLLWRNORM );
  }
    
  return mask;
}
 
/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
  /*Allocating Major number*/
  if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0)
  {
    pr_err("Cannot allocate major number\n");
    return -1;
  }
  pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
  
  /*Creating cdev structure*/
  cdev_init(&etx_cdev,&fops);
  etx_cdev.owner = THIS_MODULE;
  etx_cdev.ops = &fops;
  
  /*Adding character device to the system*/
  if((cdev_add(&etx_cdev,dev,1)) < 0)
  {
    pr_err("Cannot add the device to the system\n");
    goto r_class;
  }
 
  /*Creating struct class*/
  if(IS_ERR(dev_class = class_create(THIS_MODULE,"etx_class")))
  {
    pr_err("Cannot create the struct class\n");
    goto r_class;
  }
        
  /*Creating device*/
  if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"etx_device")))
  {
    pr_err("Cannot create the Device 1\n");
    goto r_device;
  }
  
  /*Creating a directory in /sys/kernel/ */
  kobj_ref = kobject_create_and_add("etx_sysfs",kernel_kobj);

  /*Creating sysfs file for etx_value*/
  if(sysfs_create_file(kobj_ref,&etx_attr.attr))
  {
    printk(KERN_INFO"Cannot create sysfs file......\n");
    goto r_sysfs;
  }
  
  //Initialize wait queue
  //init_waitqueue_head(&wait_queue_etx_data);
  
  pr_info("Device Driver Insert...Done!!!\n");
  return 0;

r_sysfs:
  kobject_put(kobj_ref); 
  sysfs_remove_file(kernel_kobj, &etx_attr.attr);
r_device:
  class_destroy(dev_class);
r_class:
  unregister_chrdev_region(dev,1);
  return -1;
}

/*
** Module exit function
*/ 
static void __exit etx_driver_exit(void)
{
  kobject_put(kobj_ref); 
  sysfs_remove_file(kernel_kobj, &etx_attr.attr);
  device_destroy(dev_class,dev);
  class_destroy(dev_class);
  cdev_del(&etx_cdev);
  unregister_chrdev_region(dev, 1);
  pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <[email protected]>");
MODULE_DESCRIPTION("Simple linux driver (poll)");
MODULE_VERSION("1.41");
Userspace application code
/***************************************************************************//**
*  \file       select_userspace.c
*
*  \details    select user space application
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
* *******************************************************************************/

#include <assert.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
  char   kernel_val[20];
  fd_set read_fd, write_fd;
  struct timeval timeout;
  int    ret;
  int    fd = open("/dev/etx_device", O_RDWR | O_NONBLOCK);
  
  if( fd == -1 )  
  {
    perror("open");
    exit(EXIT_FAILURE);
  }
  
  while( 1 ) 
  {
    puts("Starting Select...");
    
    /* Initialize the file descriptor set. */
    FD_ZERO( &read_fd );
    FD_SET( fd, &read_fd );
    FD_ZERO( &write_fd );
    FD_SET( fd, &write_fd );
    
    /* Initialize the timeout */
    timeout.tv_sec  = 5;       //5 Seconds
    timeout.tv_usec = 0;
    
    ret = select(FD_SETSIZE, &read_fd, &write_fd, NULL, &timeout);
    
    if( ret < 0 ) 
    {
      perror("select");
      assert(0);
    }
    
    if( FD_ISSET( fd, &read_fd ) )
    {
      read(fd, &kernel_val, sizeof(kernel_val));
      printf("READ : Kernel_val = %s\n", kernel_val);
    }
    
    if( FD_ISSET( fd, &write_fd ) )
    {
      strcpy( kernel_val, "User Space");
      write(fd, &kernel_val, strlen(kernel_val));
      printf("WRITE : Kernel_val = %s\n", kernel_val);
    }
  }
  
  return 0;
}
Makefile
This makefile contains the build command for both app and driver.

obj-m +=poll_driver.o

KDIR = /lib/modules/$(shell uname -r)/build


all:
  gcc -o poll_app poll_userspace.c
  gcc -o select_app select_userspace.c
  make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
  make -C $(KDIR)  M=$(shell pwd) clean
Testing the Device Driver
Open three different command terminals ( One for the app, One for the Driver, and another one for the Sysfs).
In the Driver terminal,
do sudo make.
Load the driver using sudo insmod poll_driver.ko
Check the dmesg,
[  +5.885320] Major = 234 Minor = 0 
[  +0.001418] Device Driver Insert...Done!!!
In the App terminal,
Run the application using sudo ./select_app
Now it should print “Starting select…” every 5 seconds. Actually, it’s getting a timeout every 5 seconds as it doesn’t get read or write permission.
[email protected]:~/Desktop/workspace/poll $ sudo ./select_app 
Starting Select...
Starting Select...
Starting Select...
In sysfs terminal,
Enter as admin using sudo su (Enter the password if it asks).
Then read the sysfs entry using cat /sys/kernel/etx_sysfs/etx_value.
Check the app message.
WRITE : Kernel_val = User Space
Starting Select...
At this time, the app writes “User Space” to the kernel.
Now check the dmesg.
[  +0.000066] Poll function
[  +4.154719] Sysfs Show - Write Permission Granted!!!
[  +0.000059] Poll function
[  +0.000052] Write function : etx_value = User Space
You can see that the kernel has given write permission to the app. So app writes data “User Space” to the kernel and the kernel stores that to etx_value variable.
Then write the data to the sysfs entry using echo "EmbeTronicX" > /sys/kernel/etx_sysfs/etx_value.
Now the driver has given the read permission to the app, and the app reads the data and it prints. Check the app’s message.
Starting Select...
Starting Select...
READ : Kernel_val = EmbeTronicX

Starting Select...
Check the dmesg.
[  +4.874488] Sysfs Store - Read Permission Granted!!!
[  +0.000059] Poll function
[  +0.000038] Read Function : etx_value = EmbeTronicX

[  +0.000090] Poll function
[  +5.005117] Poll function
You can close the app using the “Ctrl + C” and unload the driver once you are done using sudo rmmod poll_driver.
In our next tutorial, we will discuss the epoll.

Please find the other Linux device driver tutorials here.

##########################################################################################


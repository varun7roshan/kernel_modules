##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################
Prerequisites

- SysFs in Linux Device Driver
- Wait queue in Linux Device Driver
- Poll Linux Example Device Driver
------------------------------------------------------------------------------------------
Introduction

- As usual, we will go with the problem first and then the solution.

- Let’s assume the case where the app wants to read/write many IOs (Inputs and Outputs) 
based on its state. 
- What do we do in this case "?" 
- We have to wait for the IO to reach that specified state (blocking) and then do the 
IO operation. 
- We will follow the same process for all the IOs. 
- So in this case we may lose the data or timing. 
- So Synchronous and blocking won’t work every time.

- We can overcome this by using separate threads for each IOs (clients). 
- Then process that in their threads. 
- But this workaround will work until you have some fewer IOs. 
- What if you are having more IOs and want to monitor that all "?" 
- That’s where the asynchronous and non-blocking call comes into the picture.
------------------------------------------------------------------------------------------
IO Multiplexing

- The solution is to use a kernel mechanism for polling over a set of file descriptors. 
- There are 3 options you can use in Linux:

1. poll
2. select
3. epoll

In this tutorial, we will focus on the poll Linux example.
------------------------------------------------------------------------------------------
- What is a poll and its use "?"

- The poll allows a process to determine whether it can read from or write to one or more 
open files without blocking. 
- The poll is used in applications that must use multiple input or output streams without 
blocking any one of them.

- If you want to use the poll in the application, the driver must support that poll. 
- The poll will monitor multiple file descriptors or single file descriptors with 
multiple events.
------------------------------------------------------------------------------------------
- What are the events available "?"

- There are multiple events available

POLLIN: – This bit must be set if the device can be "read without blocking"

POLLRDNORM: – This bit must be set if "normal" data is available for "reading". 
- A readable device returns (POLLIN | POLLRDNORM).

POLLRDBAND: – This bit indicates that "out-of-band data" is available for "reading" 
from the device. 
- It is currently used only in one place in the Linux kernel (the DECnet code) and is 
not generally applicable to device drivers.

POLLPRI: – High-priority data ("out-of-band") can be "read without blocking". 
- This bit causes select to report that an exception condition occurred on the file 
because select reports out-of-band data as an exception condition.

POLLHUP: – When a process reading this device sees "end-of-file", the driver must set 
POLLHUP (hang-up). 
- A process calling select will be told that the device is readable, as dictated by the 
select functionality.

POLLERR: – An error condition has occurred on the device. 
- When a poll is invoked, the device is reported as both readable and writable, 
since both read and write will return an error code without blocking.

POLLOUT: – This bit is set in the return value if the "device can be written" to 
"without blocking"

POLLWRNORM: – This bit has the same meaning as POLLOUT, and sometimes it actually is the 
same number. A writable device returns (POLLOUT | POLLWRNORM).

POLLWRBAND: – Like POLLRDBAND, this bit means that data with nonzero priority can be 
written to the device. Only the datagram implementation of the poll uses this bit since 
a datagram can transmit out-of-band data.
------------------------------------------------------------------------------------------
How to use a poll in the Application "?"

- In the application, we have to add the file descriptors into the 
struct pollfd and the events you want to monitor.

struct pollfd 
{
    int   fd;         /* file descriptor */
    short events;     /* requested events */
    short revents;    /* returned events */
};

fd – File descriptor that you want to monitor,
events – Events that you want to monitor in the file descriptor
revents – If any change in the descriptor, the occurred events will be written into this 
member by the kernel.

Example:
#include <poll.h>

struct pollfd pfd;

pfd.fd = fd; // File Descriptor that you want to monitor

// Event that you want to monitor for this fd
pfd.events = ( POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM );

- Note: You can add multiple file descriptors by creating struct pollfd pfd[num_of_fd];

Once you create the structure and initialized it, then call the poll().

int poll(struct pollfd *fds, nfds_t nfds, int timeout);

fds – file descriptors array
nfds – number of file descriptors
timeout – specifies the number of milliseconds that poll() should block waiting for a file 
descriptor to become ready. The call will block until either:
------------------------------------------------------------------------------------------
- a file descriptor becomes ready, the call is interrupted by a signal handler, 
or the timeout expires.

- When you set timeout as -1, then there is no timeout and it will work like a blocking 
call.

Return:
- On success, a positive number is returned; this is the number of structures that have 
"nonzero revents" fields (in other words, those descriptors with events or errors reported)
- A value of  0 indicates that the call timed out and no file descriptors were ready. 
- On error, -1 is returned and errno is set appropriately.
------------------------------------------------------------------------------------------
How to use a poll in the Linux Device Driver "?"

wait queue
- In the Linux device driver, the poll is internally using the wait queue. 
- So we have to declare the wait queue and initialize that. 
- You can use static or dynamic method.

Example:
#include <linux/wait.h>                 //Required for the wait queues

/* Static method */
//Waitqueue
DECLARE_WAIT_QUEUE_HEAD(wait_queue_etx_data);
			
			OR

/* Dynamic method */
//Initialize wait queue
init_waitqueue_head(&wait_queue_etx_data);
------------------------------------------------------------------------------------------
poll_wait

- In the driver, we need to define poll_wait function and assign that to the 
file operation’s structure.

unsigned int (*poll) (struct file *filp, struct poll_table_struct *wait);

filep – File Structure.
wait – Poll table, We can add one or more wait queues into this table.

Example:
/*
** This fuction will be called when app calls the poll function
*/
static unsigned int etx_poll(struct file *filp, struct poll_table_struct *wait)
{
  __poll_t mask = 0;
  
  pr_info("Poll function\n");
  
  /* Do your Operation */
    
  return mask;
}

/*
** File operation sturcture
*/
static struct file_operations fops =
{
  .poll           = etx_poll
};
------------------------------------------------------------------------------------------
How do the poll_wait is working "?"

- Whenever the application calls the poll function, the kernel calls the "poll_wait" 
of the driver with the file structure and "poll_table". 
- In that poll table, we need to add the wait queue that we have created. 
- Then this "poll_wait" will immediately return.

- When we got something we wanted, we can wake up the wait queue. 
- That will again call the poll_wait as wait queue is part of the poll table. 
- Then we have to return the proper event to the application.
------------------------------------------------------------------------------------------

- For example, we got some free memory space in the kernel. 
- So, we want to tell the application to write into the memory. 
- The application will keep on checking the poll function if we have given a timeout as an 
argument. 
- Otherwise, It will be blocked. When we found free space, we will wake up the wait queue. 
- So, poll_wait function will be getting called. 
- Then we check whether we got free space or not. 
- If we got free space then we return (POLLOUT | POLLWRNORM) from the poll_wait function. 
- This event will be written into the application’s struct pollfd‘s revents member. 
- Then the application will check revents and write the data to the kernel space as we 
sent  (POLLOUT | POLLWRNORM). 
- If we don’t find free space we will return 0 from the poll_wait. 
- So, the kernel will again wait for the wake-up of the wait queue.

- I know that you got confused. You will feel good when you see the example and 
work out that example.

Now, it’s time to make our hands dirty. Let’s start programming.
------------------------------------------------------------------------------------------
Poll Linux Example Programming

- This example is just for educational purposes and explains a poll in Linux device driver
- This might not be the real use case.

- In this example, I’ve created one sysfs entry (/sys/kernel/etx_sysfs/etx_value) with the 
reading and writing option. 
- The poll Linux example driver and application supposed to work like below.
------------------------------------------------------------------------------------------
Driver Concept

- When you write data using the sysfs entry (/sys/kernel/etx_sysfs/etx_value), that means 
data is available in the kernel. 
- So, we have to inform the userspace application to read the available data 
(Driver gives the permission to the app for reading).
- When you read the data using the sysfs entry (/sys/kernel/etx_sysfs/etx_value), that 
means, data has been read by the kernel, and the userspace app has to write the data into 
the kernel space. 
- So that driver can read the data. (Driver gives the permission to the app for writing).
- So, whenever the app gets read permission, it can read the data from the driver and 
whenever the app gets the write permission, it can write some data into the driver.
------------------------------------------------------------------------------------------
Application concept

- The application will open the driver’s device file (/dev/etx_device).
- Then register that descriptor with reading and writing operation using Poll 
( 5 Seconds timeout ). 
- So, every 5 seconds it will exit the poll and again it polls.
- If this app gets the read permission from the poll Linux example kernel driver, 
then it reads the data.
- If this app gets the write permission from the poll Linux example kernel driver, 
then it writes the data to the driver.

- I hope that I’ve explained the concept properly without confusing you. 
- If you are confused still, please look at the output and the example code. 
- Then you should be better, I guess.
------------------------------------------------------------------------------------------

Driver Source Code
[Get the source code from GitHub]

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
  wake_up(&wait_queue_etx_data); //--------------------------(1) WAKE-UP
  
  return sprintf(buf, "%s", "Success\n");
}

/*
** This function will be called when we write the sysfs file
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
  wake_up(&wait_queue_etx_data); //---------------------------------(2) WAKE-UP
  
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
** This fuction will be called when we read the Device file
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
** This fuction will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
  strcpy(etx_value, buf);
  
  pr_info("Write function : etx_value = %s\n", etx_value);

  return len;
}

/*
** This fuction will be called when app calls the poll function
*/
static unsigned int etx_poll(struct file *filp, struct poll_table_struct *wait)
{
  __poll_t mask = 0;
  
  poll_wait(filp, &wait_queue_etx_data, wait); //------> POLL_WAIT - BLOCKED, till WAKE-UP
  pr_info("Poll function\n");
  
  if( can_read ) //Flag set before WAKE-UP is invoked
  {
    can_read = false;
    mask |= ( POLLIN | POLLRDNORM );
  }
  
  if( can_write ) //Flag set before WAKE-UP is invoked
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
  
  //Initialize wait queue - DYNAMIC - But we have done STATIC initialization at TOP 
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
*  \file       poll_userspace.c
*
*  \details    poll user space application
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
  char kernel_val[20];
  int fd, ret, n;
  struct pollfd pfd;
  
  fd = open("/dev/etx_device", O_RDWR | O_NONBLOCK);
    
  if( fd == -1 )  
  {
    perror("open");
    exit(EXIT_FAILURE);
  }
    
  pfd.fd = fd;
  pfd.events = ( POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM );
    
  while( 1 ) 
  {
    puts("Starting poll...");
    
    ret = poll(&pfd, (unsigned long)1, 5000);   //wait for 5secs
    
    if( ret < 0 ) 
    {
      perror("poll");
      assert(0);
    }
    
    if( ( pfd.revents & POLLIN )  == POLLIN )
    {
      read(pfd.fd, &kernel_val, sizeof(kernel_val));
      printf("POLLIN : Kernel_val = %s\n", kernel_val);
    }
    
    if( ( pfd.revents & POLLOUT )  == POLLOUT )
    {
      strcpy( kernel_val, "User Space");
      write(pfd.fd, &kernel_val, strlen(kernel_val));
      printf("POLLOUT : Kernel_val = %s\n", kernel_val);
    }
  }
}
Makefile
This makefile contains the build command for both app and driver.

obj-m +=poll_driver.o

KDIR = /lib/modules/$(shell uname -r)/build


all:
  gcc -o poll_app poll_userspace.c
  make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
  make -C $(KDIR)  M=$(shell pwd) clean
Testing the Device Driver
Open three different command terminals ( One for the app, One for the Driver, and another one for the Sysfs).
In the Driver terminal,
do sudo make.
Load the driver using sudo insmod driver.ko
Check the dmesg,
[ +17.525660] Major = 234 Minor = 0 
[  +0.000281] Device Driver Insert...Done!!!
In the App terminal,
Run the application using sudo ./poll_app
Now it should print “Starting poll…” every 5 seconds. Actually, it’s getting timeout every 5 seconds as it doesn’t get read or write permission.
[email protected]:~/Desktop/workspace/poll $ sudo ./poll_app 
Starting poll...
Starting poll...
Starting poll...
In sysfs terminal,
Enter as admin using sudo su (Enter the password if it asks).
Then read the sysfs entry using cat /sys/kernel/etx_sysfs/etx_value.
Check the app message.
Starting poll...
POLLOUT : Kernel_val = User Space
Starting poll...
At this time, the app writes “User Space” to the kernel.
Now check the dmesg.
[  +4.308296] Sysfs Show - Write Permission Granted!!!
[  +0.000080] Poll function
[  +0.000015] Write function : etx_value = User Space
[  +0.000039] Poll function
You can see that the kernel has given write permission to the app. So app writes data “User Space” to kernel and kernel stores that to etx_value variable.
Then write the data to the sysfs entry using echo "EmbeTronicX" > /sys/kernel/etx_sysfs/etx_value.
Now the driver has given the read permission to the app, and the app reads the data and it prints. Check the app’s message.
Starting poll...
POLLIN : Kernel_val = EmbeTronicX
Starting poll...
Check the dmesg.
[  +0.000058] Poll function
[  +1.902344] Sysfs Store - Read Permission Granted!!!
[  +0.000039] Poll function
[  +0.000016] Read Function : etx_value = EmbeTronicX
[  +0.000050] Poll function
You can close the app using the “Ctrl + C” and unload the driver once you are done using sudo rmmod poll_driver.
I hope, this tutorial helped you. In our next tutorial, we will discuss select in Linux device driver.

Please find the other Linux device driver tutorials here.

##########################################################################################


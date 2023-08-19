##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################
Prerequisites

Poll in Linux Device Driver
Select in Linux Device Driver
SysFs in Linux Device Driver
Wait queue in Linux Device Driver
Epoll in Linux Device Driver

IO Multiplexing
The solution is to use a kernel mechanism for polling over a set of file descriptors. There are 3 options you can use in Linux:

1. poll
>> poll_wait() - will pass wait_queue - and some function/ISR will call wake_up()

2. select
>> Exactly same as poll - no change in driver code - poll_wait() only will be invoked
>> Internally in select() implementation - it will take care of accepting and returning
to user-space

3.epoll
------------------------------------------------------------------------------------------
In this tutorial, we will focus on the "epoll" in Linux Device Driver.

What is epoll"?"
- epoll is an "event poll" and a variant of poll. 
- It can be used either as an "Edge or Level Triggered interface" and scales well to 
large numbers of watched file descriptors.

Edge_triggered: a call to epoll_wait will return only when a new event is enqueued with 
the epoll.

Level_triggered: a call to epoll_wait will return as long as the condition holds.

Example for Level and Edge trigger:
- Let’s take this example to understand the Level and Edge triggered better.
------------------------------------------------------------------------------------------
- You have registered one "pipe" with "epoll" for "reading"
- The application is able to read, only "1Kb" data at a time. 
- So, If you want to read "5Kb", you have to read 5 times.

- Assume someone has written 5Kb data into the pipe and it is available to read. 
- When the application calls "epoll_wait", it will return immediately as the data is 
available. 
- So, the application reads 1Kb data and again calls the epoll_wait. 

- If you have configured "epoll" as "edge-triggered", then epoll won’t return until 
someone writes (new events) again into the pipe even though 4Kb data is remaining for 
reading. 

- Whereas in the "level-triggered", epoll_wait will return immediately as 4Kb data is 
remaining to read. 
- So, the application can call epoll_wait for another 4 times without the new events.
------------------------------------------------------------------------------------------
How to use epoll in Application "?"

- There are three system calls are provided to set up and control an epoll.

epoll_create,
epoll_ctl,
epoll_wait
------------------------------------------------------------------------------------------
epoll_create

- This epoll_create creates an epoll instance. 
- You have to include the header #include <sys/epoll.h>.

int epoll_create(int size);

size – Size of the epoll instance. 
- Since Linux 2.6.8, the size argument is ignored but must be greater than zero

Return:
- It "returns a file descriptor referring to the new epoll instance". 
- This file descriptor is used for all the subsequent calls to the epoll interface. 
- When no longer required, the file descriptor returned by epoll_create should be closed 
by using close. 
- When all file descriptors referring to an epoll instance have been closed, the kernel 
destroys the instance and releases the associated resources for reuse.

- Note: You can use epoll_create1() also.
------------------------------------------------------------------------------------------
epoll_ctl

- This system call performs control operations on the epoll instance referred to by the 
file descriptor epfd.

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

epfd:– This is an epoll instance that has been created using epoll_create.
op:– Operations need to be performed on epfd. Example operations are,

EPOLL_CTL_ADD: – Register the target file descriptor fd on the epoll instance referred to 
by the file descriptor epfd and associate the event with the internal file linked to fd.
>>>> HERE the Link between the REAL "FD" and "EFD" is done

EPOLL_CTL_MOD: – Change the event event associated with the target file descriptor fd.
EPOLL_CTL_DEL: – Remove (deregister) the target file descriptor fd from the epoll instance 
referred to by epfd.

fd: – File descriptor to monitor.
>> The Real FD of File or PIPE

event: – Events to be monitored in the file descriptor fd. Example events are,

EPOLLIN: – The associated file is available for read operations.
EPOLLOUT: – The associated file is available for write operations.
EPOLLPRI: – There is urgent data available for read operations.
EPOLLERR: – Error condition happened on the associated file descriptor.
EPOLLHUP: – Hang up happened on the associated file descriptor.
EPOLLET: – Sets the Edge Triggered behavior for the associated file descriptor. 
- The default behavior for epoll is "Level Triggered"

EPOLLONESHOT: (since Linux 2.6.2) – Sets the one-shot behavior for the associated file 
descriptor. 
- This means that after an event is pulled out with epoll_wait the associated file 
descriptor is internally disabled and no other events will be reported by the epoll 
interface. 
- The user must call epoll_ctl with EPOLL_CTL_MOD to rearm the file descriptor with a 
new event mask.

EPOLLRDHUP: (since Linux 2.6.17) – Stream socket peer closed connection or shut down 
writing half of the connection. (This flag is especially useful for writing simple code 
to detect peer shutdown when using Edge Triggered monitoring.)

EPOLLWAKEUP: – Ensure that system does not enter "suspend" or "hibernate" when this event 
is pending or is being processed. (EPOLLONESHOT and EPOLLET must be clear and the process 
should have the CAP_BLOCK_SUSPEND capability.)

EPOLLEXCLUSIVE: – Sets up the exclusive mode of a wake-up for the epoll file descriptor 
to which this file descriptor is being attached. 
- Useful for avoiding the thundering herd problem in certain scenarios.

Return:
- It returns zero on the successful call. 
- When an error occurs, epoll_ctl returns -1 and errno is set appropriately.
------------------------------------------------------------------------------------------
epoll_wait

- This system call waits for events on the epoll instance referred to by the file 
descriptor epfd.

int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);

epfd   – This is an epoll instance that has been created using epoll_create.
events  – events that will be available for the caller (returned events).
maxevents – Maximum events that can be returned by the epoll_wait. 
- This must be greater than zero.
timeout – The minimum number of milliseconds that epoll_wait will block. 
- Specifying a timeout of -1 causes epoll_wait to block indefinitely, while specifying a 
timeout equal to zero causes epoll_wait to return immediately, even if no events are 
available.

Return:
- epoll_wait returns the number of file descriptors ready for the requested I/O on a 
successful call, or zero if no file descriptor became ready during the requested timeout 
milliseconds. 
- When an error occurs, epoll_wait returns -1 and errno is set appropriately.
------------------------------------------------------------------------------------------
Please refer to the application code for better understanding.
------------------------------------------------------------------------------------------
How to use a epoll in Linux Device Driver "?"

- We have already implemented the poll functionality in the Linux device driver side in 
our previous tutorial. So, we can use the same Linux device driver for epoll() too as 
"epoll() also calls the same poll_wait()" in the Linux kernel.
------------------------------------------------------------------------------------------
- What is the difference between epoll, poll, and select (epoll vs poll vs select) "?"

- In our last tutorial, we have discussed poll vs select. 
- Now we will compare epoll with them.

- epoll can behave as "level-triggered" or "edge-triggered"

- the poll is as per the POSIX standard. epoll is Linux-specific. 
- So, "epoll is non-portable"

- epoll_wait returns only the objects with ready file descriptors

- Select supports only up to 1024 file descriptors. 
- The performance of the poll is bad if we use more file descriptors. 
- E-poll solves those two problems. Please refer to the below table.

Number of File Descriptors	Poll (CPU Time)	 Select (CPU Time)	 E-Poll (CPU Time)
10	                          0.61	                0.73	         0.41
100	                          2.9	                  3	             0.42
1000	                        35	                  35	           0.53
10000	                        990	                  930	           0.66
------------------------------------------------------------------------------------------
- Epoll in Linux – Example Programming

- I have not made any changes to that Linux device driver source code (poll).
And, the concept is also the same as the previous tutorial (poll in Linux).

- I’ve just copy-pasted the concept below from the previous tutorial for your understanding

- In this example, I’ve created one sysfs entry (/sys/kernel/etx_sysfs/etx_value) with the 
reading and writing option. 
- The epoll Linux example driver and application are supposed to work like below.
------------------------------------------------------------------------------------------
Driver Concept
When you write data using the sysfs entry (/sys/kernel/etx_sysfs/etx_value), which means data is available in the kernel. So, we have to inform the userspace application to read the available data (Driver gives the permission to the app for reading).
When you read the data using the sysfs entry (/sys/kernel/etx_sysfs/etx_value), that means, data has been read by the kernel, and the userspace app has to write the data into the kernel space. So that driver can read the data. (Driver gives permission to the app for writing).
So, whenever the app gets read permission, it can read the data from the driver and whenever the app gets the write permission, it can write some data into the driver.
Application concept
The application will open the driver’s device file (/dev/etx_device).
Create epoll instance using epoll_create() or epoll_create1().
Add the file descriptors and events that you want to wait for, using the epoll_ctl().
Call the epoll_wait() with your desired timeout. In our example, we are setting 5 seconds timeout.
So, every 5 seconds it will exit the epoll_wait and again it calls the epoll_wait.
If this app gets the read permission from the Linux kernel driver, then it reads the data.
If this app gets the write permission from the Linux kernel driver, then it writes the data to the driver.

------------------------------------------------------------------------------------------
Driver Source Code
[Get the full updated source code from GitHub]

/***************************************************************************//**
*  \file       poll_driver.c
*
*  \details    Poll / Select / E-Poll driver 
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
 
/*************** Driver functions **********************/
static int      etx_open(struct inode *inode, struct file *file);
static int      etx_release(struct inode *inode, struct file *file);
static ssize_t  etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static unsigned int etx_poll(struct file *filp, struct poll_table_struct *wait);

/*************** Sysfs functions **********************/
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
    pr_err("Cannot create sysfs file......\n");
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
MODULE_DESCRIPTION("Simple linux driver (Poll / Select / E-Poll driver )");
MODULE_VERSION("1.41");
Userspace application code
/***************************************************************************//**
*  \file       epoll_userspace.c
*
*  \details    epoll user space application
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
* *******************************************************************************/

#include <assert.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define EPOLL_SIZE ( 256 )
#define MAX_EVENTS (  20 )

int main()
{
  char kernel_val[20];
  int fd, epoll_fd, ret, n;
  struct epoll_event ev,events[20];
  
  fd = open("/dev/etx_device", O_RDWR | O_NONBLOCK);
    
  if( fd == -1 )  
  {
    perror("open");
    exit(EXIT_FAILURE);
  }
  
  //Create epoll instance
  epoll_fd = epoll_create(EPOLL_SIZE);
  
  if( epoll_fd < 0 )  
  {
    perror("epoll_create");
    exit(EXIT_FAILURE);
  }
    
  ev.data.fd = fd;
  ev.events  = ( EPOLLIN | EPOLLOUT );
  
  //Add the fd to the epoll
  if( epoll_ctl( epoll_fd, EPOLL_CTL_ADD, fd, &ev ) )
  {
    perror("Failed to add file descriptor to epoll\n");
    close(epoll_fd);
    exit(EXIT_FAILURE);
  }
    
  while( 1 ) 
  {
    puts("Starting epoll...");
    
    ret = epoll_wait( epoll_fd, events, MAX_EVENTS, 5000);;   //wait for 5secs
    
    if( ret < 0 ) 
    {
      perror("epoll_wait");
      close(epoll_fd);
      assert(0);
    }
    
    for( n=0; n<ret; n++ )
    {    
      if( ( events[n].events & EPOLLIN )  == EPOLLIN )
      {
        read(events[n].data.fd, &kernel_val, sizeof(kernel_val));
        printf("EPOLLIN : Kernel_val = %s\n", kernel_val);
      }
      
      if( ( events[n].events & EPOLLOUT )  == EPOLLOUT )
      {
        strcpy( kernel_val, "User Space");
        write(events[n].data.fd, &kernel_val, strlen(kernel_val));
        printf("EPOLLOUT : Kernel_val = %s\n", kernel_val);
      }
    }
  }
  
  if(close(epoll_fd))
  {
    perror("Failed to close epoll file descriptor\n");
  }
  
  if(close(fd))
  {
    perror("Failed to close file descriptor\n");
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
  gcc -o epoll_app epoll_userspace.c
  make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
  make -C $(KDIR)  M=$(shell pwd) clean
Testing the Device Driver
Open three different command terminals ( One for the app, One for the Driver, and another one for the Sysfs).
In the Driver terminal,
do sudo make.
Load the driver using sudo insmod poll_driver.ko
Check the dmesg,
[Mar21 16:41] Major = 234 Minor = 0 
[  +0.003924] Device Driver Insert...Done!!!
In the App terminal,
Run the application using sudo ./epoll_app
Now it should print “Starting epoll…” every 5 seconds. Actually, it’s getting timeout every 5 seconds as it doesn’t get read or write permission.
[email protected]:~/Desktop/workspace/Git/Tutorials/Linux/Device_Driver/Poll $ sudo ./epoll_app 
Starting poll...
Starting poll...
In sysfs terminal,
Enter as admin using sudo su (Enter the password if it asks).
Then read the sysfs entry using cat /sys/kernel/etx_sysfs/etx_value.
Check the app message.
EPOLLOUT : Kernel_val = User Space
Starting poll...
At this time, the app writes “User Space” to the kernel.
Now check the dmesg.
[Mar21 16:46] Sysfs Show - Write Permission Granted!!!
[  +0.000071] Poll function
[  +0.000037] Write function : etx_value = User Space
[  +0.000163] Poll function
You can see that the kernel has given write permission to the app. So app writes data “User Space” to the kernel and the kernel stores that to etx_value variable.
Then write the data to the sysfs entry using echo "EmbeTronicX" > /sys/kernel/etx_sysfs/etx_value.
Now the driver has given the read permission to the app, and the app reads the data and it prints. Check the app’s message.
EPOLLIN : Kernel_val = EmbeTronicX

Starting poll...
Check the dmesg.
[Mar21 16:48] Sysfs Store - Read Permission Granted!!!
[  +0.000236] Poll function
[  +0.000018] Read Function : etx_value = EmbeTronicX

[  +0.000047] Poll function
You can close the app using the “Ctrl + C” and unload the driver once you are done using sudo rmmod poll_driver.
In our next tutorial, softirq in Linux device driver.

##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################
																	Signals:
Introduction
- Generally, A signal is an action that is intended to send a particular message. 
- It can be sound, gesture, event, etc. Below are the normal signals which we are using 
the day to day life.

- What about the signal in Linux "?" 
- Signals are a way of sending simple messages which are used to notify a process or 
thread of a particular event. 
- In Linux, there are many processes that will be running at a time. 
- We can send a signal from one process to another process. 
- Signals are one of the oldest inter-process communication methods. 
- These signals are asynchronous. Like User space signals, can we send a signal to 
userspace from kernel space "?" Yes, why not.

- Sending Signal from Linux Device Driver to User Space
Using the following steps easily we can send the signals.

1. Decide the signal that you want to send.
2. Register the user space application with the driver.
3. Once something happened (in our example we used interrupts) send signals to userspace.
4. Unregister the user space application when you have done with it.
5. Decide the signal that you want to send
5. First, select the signal number which you want to send. 
In our case, we are going to send signal 44.
-------------------------------------------------------------------------------------------
Example:

#define SIGETX    44

1. Register the user space application with the driver

- Before sending the signal, your device driver should know to whom it needs to send the 
signal. For that, we need to register the process to the driver. 
- So we need to send the PID to the driver first. Then that driver will use the PID and 
sends the signal. You can register the application PID in anyways like IOCTL, 
Open/read/write call. 
- In our example, we are going to register using IOCTL.

Example:
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    if (cmd == REG_CURRENT_TASK) {
        printk(KERN_INFO "REG_CURRENT_TASK\n");
        task = get_current();
        signum = SIGETX;
    }
    return 0;
}
-------------------------------------------------------------------------------------------
2. Send signals to user space

- After registering the application to the driver, the driver can able to send the signal 
when it wants to. In our example, we will send the signal when we get the interrupt.

Example:
//Interrupt handler for IRQ 11. 
static irqreturn_t irq_handler(int irq,void *dev_id) {
    struct siginfo info;
    printk(KERN_INFO "Shared IRQ: Interrupt Occurred");
    
    //Sending signal to app
    memset(&info, 0, sizeof(struct siginfo));
    info.si_signo = SIGETX;
    info.si_code = SI_QUEUE;
    info.si_int = 1;

    if (task != NULL) {
        printk(KERN_INFO "Sending signal to app\n");
        if(send_sig_info(SIGETX, &info, task) < 0) {
            printk(KERN_INFO "Unable to send signal\n");
        }
    }
    return IRQ_HANDLED;
}
-------------------------------------------------------------------------------------------
3. Unregister the user space application

- When you are done with your task, you can unregister your application. 
- Here we are unregistering when that application closes the driver.

Example:
static int etx_release(struct inode *inode, struct file *file)
{
    struct task_struct *ref_task = get_current();
    printk(KERN_INFO "Device File Closed...!!!\n");
    
    //delete the task
    if(ref_task == task) {
        task = NULL;
    }
    return 0;
}

##########################################################################################

/* used to create numbers */
#define _IO(type,nr)    _IOC(_IOC_NONE,(type),(nr),0)
#define _IOR(type,nr,size)  _IOC(_IOC_READ,(type),(nr),sizeof(size))
#define _IOW(type,nr,size)  _IOC(_IOC_WRITE,(type),(nr),sizeof(size))
#define _IOWR(type,nr,size) _IOC(_IOC_READ|_IOC_WRITE,(type),(nr),sizeof(size))


#define VIDIOC_QUERYCAP    _IOR('V',  0, struct v4l2_capability)
#define VIDIOC_ENUM_FMT         _IOWR('V',  2, struct v4l2_fmtdesc)
#define VIDIOC_G_FMT    _IOWR('V',  4, struct v4l2_format)
#define VIDIOC_S_FMT    _IOWR('V',  5, struct v4l2_format)
#define VIDIOC_REQBUFS    _IOWR('V',  8, struct v4l2_requestbuffers)
#define VIDIOC_QUERYBUF   _IOWR('V',  9, struct v4l2_buffer)


In the Linux kernel, when defining new ioctl commands, the `_IOR()` macro is often used to specify input/output ioctls. Let's break down the parameters of the `_IOR()` macro:

1. **type**: This parameter represents the type of the ioctl command. It's typically a unique identifier or a number that identifies a specific device or driver. It helps in categorizing and distinguishing different ioctl commands. The type parameter should be defined consistently across the ioctl interface to ensure proper communication between user-space and kernel-space code.

2. **nr**: This parameter specifies the number or identifier of the ioctl command within the given type. It uniquely identifies the particular ioctl command within the specified type. Each ioctl command should have a unique number or identifier within its type.

3. **size**: This parameter indicates the size of the data associated with the ioctl command. It specifies the size of the data buffer used for input or output operations. The size parameter helps in ensuring that the proper amount of memory is allocated for transferring data between user-space and kernel-space code.

The `_IOR()` macro is actually a combination of the `_IOC()` macro and the `_IOC_READ` flag. The `_IOC()` macro is a general macro used for constructing ioctl commands, and `_IOC_READ` is a flag indicating that the ioctl command involves reading data from the device.

Putting it all together, `_IOR()` constructs an ioctl command that indicates an input/output operation, specifying the type, number, and size of the ioctl command. This macro helps in defining ioctl commands in a consistent and readable manner within the Linux kernel codebase.

##########################################################################################

/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple Linux device driver (Signals)
*
*  \author     EmbeTronicX
*
* *******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/err.h>
#define SIGETX 44
 
#define REG_CURRENT_TASK _IOW('a','a',int32_t*)
 
#define IRQ_NO 11
 
/* Signaling to Application */
static struct task_struct *task = NULL;
static int signum = 0;
 
int32_t value = 0;
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
 
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
 
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = etx_read,
        .write          = etx_write,
        .open           = etx_open,
        .unlocked_ioctl = etx_ioctl,
        .release        = etx_release,
};
 
//Interrupt handler for IRQ 11. 
static irqreturn_t irq_handler(int irq,void *dev_id) {
    struct siginfo info;
    printk(KERN_INFO "Shared IRQ: Interrupt Occurred");
    
    //Sending signal to app
    memset(&info, 0, sizeof(struct siginfo));
    info.si_signo = SIGETX;
    info.si_code = SI_QUEUE;
    info.si_int = 1;
 
    if (task != NULL) {
        printk(KERN_INFO "Sending signal to app\n");
        if(send_sig_info(SIGETX, &info, task) < 0) {
            printk(KERN_INFO "Unable to send signal\n");
        }
    }
 
    return IRQ_HANDLED;
}
 
static int etx_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device File Opened...!!!\n");
    return 0;
}
 
static int etx_release(struct inode *inode, struct file *file)
{
    struct task_struct *ref_task = get_current();
    printk(KERN_INFO "Device File Closed...!!!\n");
    
    //delete the task
    if(ref_task == task) {
        task = NULL;
    }
    return 0;
}
 
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Read Function\n");
    asm("int $0x3B");  //Triggering Interrupt. Corresponding to irq 11
    return 0;
}

static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    printk(KERN_INFO "Write function\n");
    return 0;
}
 
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    if (cmd == REG_CURRENT_TASK) {
        printk(KERN_INFO "REG_CURRENT_TASK\n");
        task = get_current();
        signum = SIGETX;
    }
    return 0;
}
 
 
static int __init etx_driver_init(void)
{
    /*Allocating Major number*/
    if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
            printk(KERN_INFO "Cannot allocate major number\n");
            return -1;
    }
    printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
    /*Creating cdev structure*/
    cdev_init(&etx_cdev,&fops);
 
    /*Adding character device to the system*/
    if((cdev_add(&etx_cdev,dev,1)) < 0){
        printk(KERN_INFO "Cannot add the device to the system\n");
        goto r_class;
    }
 
    /*Creating struct class*/
    if(IS_ERR(dev_class = class_create(THIS_MODULE,"etx_class"))){
        printk(KERN_INFO "Cannot create the struct class\n");
        goto r_class;
    }
 
    /*Creating device*/
    if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"etx_device"))){
        printk(KERN_INFO "Cannot create the Device 1\n");
        goto r_device;
    }
 
    if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "etx_device", (void *)(irq_handler))) {
        printk(KERN_INFO "my_device: cannot register IRQ ");
        goto irq;
    }
 
    printk(KERN_INFO "Device Driver Insert...Done!!!\n");
    return 0;
irq:
    free_irq(IRQ_NO,(void *)(irq_handler));
r_device:
    class_destroy(dev_class);
r_class:
    unregister_chrdev_region(dev,1);
    return -1;
}
 
static void __exit etx_driver_exit(void)
{
    free_irq(IRQ_NO,(void *)(irq_handler));
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <[email protected]>");
MODULE_DESCRIPTION("A simple device driver - Signals");
MODULE_VERSION("1.20");

##########################################################################################
User_Space_APP:

/***************************************************************************//**
*  \file       test_app.c
*
*  \details    Userspace application to test the Device driver
*
*  \author     EmbeTronicX
*
* *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
 
#define REG_CURRENT_TASK _IOW('a','a',int32_t*)
 
#define SIGETX 44
 
static int done = 0;
int check = 0;
 
void ctrl_c_handler(int n, siginfo_t *info, void *unused)
{
    if (n == SIGINT) {
        printf("\nrecieved ctrl-c\n");
        done = 1;
    }
}
 
void sig_event_handler(int n, siginfo_t *info, void *unused)
{
    if (n == SIGETX) {
        check = info->si_int;
        printf ("Received signal from kernel : Value =  %u\n", check);
    }
}
 
int main()
{
    int fd;
    int32_t value, number;
    struct sigaction act;
 
    printf("*********************************\n");
    printf("*******WWW.EmbeTronicX.com*******\n");
    printf("*********************************\n");
 
    /* install ctrl-c interrupt handler to cleanup at exit */
    sigemptyset (&act.sa_mask);
    act.sa_flags = (SA_SIGINFO | SA_RESETHAND);
    act.sa_sigaction = ctrl_c_handler;
    sigaction (SIGINT, &act, NULL);
 
    /* install custom signal handler */
    sigemptyset(&act.sa_mask);
    act.sa_flags = (SA_SIGINFO | SA_RESTART);
    act.sa_sigaction = sig_event_handler;
    sigaction(SIGETX, &act, NULL);
 
    printf("Installed signal handler for SIGETX = %d\n", SIGETX);
 
    printf("\nOpening Driver\n");
    fd = open("/dev/etx_device", O_RDWR);
    if(fd < 0) {
            printf("Cannot open device file...\n");
            return 0;
    }
 
    printf("Registering application ...");
    /* register this task with kernel for signal */
    if (ioctl(fd, REG_CURRENT_TASK,(int32_t*) &number)) {
        printf("Failed\n");
        close(fd);
        exit(1);
    }
    printf("Done!!!\n");
   
    while(!done) {
        printf("Waiting for signal...\n");
 
        //blocking check
        while (!done && !check);
        check = 0;
    }
 
    printf("Closing Driver\n");
    close(fd);
}

##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################




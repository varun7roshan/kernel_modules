##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################
Prerequisites

Interrupts in Linux Kernel
Interrupts example in Linux kernel
Workqueue Example – Static Method
Workqueue Example – Dynamic Method
Workqueue Example – Own Workqueue
Tasklet – Dynamic Method
Tasklet – Static Method
------------------------------------------------------------------------------------------
As we are using interrupts, we came across the word called bottom half.

Bottom Half
- When Interrupt triggers, the Interrupt handler should be executed very quickly and it 
should not run for more time (it should not perform time-consuming tasks). 
- If we have the interrupt handler which is doing more tasks then we need to divide it 
into two halves.

Top Half
Bottom Half
------------------------------------------------------------------------------------------
- The top Half is nothing but our interrupt handler. 
- If we want to do less work, then the top half is more than enough. 
- No need for the bottom half in that situation. 
- But if we have more work when interrupt hits, then we need the bottom half. 
- The bottom half runs in the future, at a more convenient time, with all interrupts 
enabled. 
- So, The job of bottom halves is to perform any interrupt-related work not performed by 
the interrupt handler.
------------------------------------------------------------------------------------------
There are 4 bottom half mechanisms available in Linux:

1. Workqueue – Executed in a process context.
2. Threaded IRQs
3. Softirqs – Executed in an atomic context.
4. Tasklets – Executed in an atomic context.
------------------------------------------------------------------------------------------
In this tutorial, we will discuss softirq in Linux Kernel.

Softirq in Linux Kernel
- Softirq is also known as a software interrupt request. 
- When we have more work do to in the ISR, we can defer some work to later. 
- So, we do the necessary work in the original ISR (top half) and trigger the softirq. 
- The softirq is rarely used as we have tasklets already. 
- Most of the stuff, we can do using the tasklets. 

- Do you know one thing "?" Tasklets also built on softirq.
------------------------------------------------------------------------------------------
How to use the softirq in the Linux kernel "?"

- There are three simple steps that we have to follow in order to use this.

1. Edit the Linux kernel source code
2. Declare softirq
3. Create the softirq handler
4. Register the softirq with the handler
5. Trigger the softirq
6. Edit the Linux kernel source code
------------------------------------------------------------------------------------------
- We have to make some changes in the Linux kernel source code to use the softirq.

Why do we need to make changes in the Linux Kernel "?"
- Unlike tasklets, Softirqs are statically allocated at the compile time. 
- So we cannot dynamically create or kill the softirqs. 
- Softirqs are represented using the 
struct softirq_action 
- which is defined in <linux/interrupt.h>.

struct softirq_action
{
  void (*action)(struct softirq_action *);  //softirq handler
};

- There are only a couple of entries are available in the array to register the softirq.

static struct softirq_action softirq_vec[NR_SOFTIRQS];

- Each registered softirq consumes one entry in the array softirq_vec. 
- Consequently, there can be a maximum of 32 registered softirqs. 
- Note that this cap is fixed the maximum number of registered softirqs cannot be 
dynamically changed. 
- NR_SOFTIRQS is the last element of the enum. 
So, a total of 11 softirqs are registered as of now. 
Please refer to the enum in <linux/interrupt.h>.

enum
{
  HI_SOFTIRQ=0,
  TIMER_SOFTIRQ,
  NET_TX_SOFTIRQ,
  NET_RX_SOFTIRQ,
  BLOCK_SOFTIRQ,
  IRQ_POLL_SOFTIRQ,
  TASKLET_SOFTIRQ,
  SCHED_SOFTIRQ,
  HRTIMER_SOFTIRQ,
  RCU_SOFTIRQ,    /* Preferable RCU should always be the last softirq */

  NR_SOFTIRQS
};

- So, there are a few softirqs that are already defined. 
- If we want to create our own softirq, we need to edit the source code. 
- So, obviously, we have to re-compile the kernel. 
- Not only this, we have to export some APIs too (This will be discussed later in 
this tutorial).
------------------------------------------------------------------------------------------
Declare the softirq

- Add your own softirq in the enum in interrupt.h like below.

enum
{
  HI_SOFTIRQ=0,
  TIMER_SOFTIRQ,
  NET_TX_SOFTIRQ,
  NET_RX_SOFTIRQ,
  BLOCK_SOFTIRQ,
  IRQ_POLL_SOFTIRQ,
  TASKLET_SOFTIRQ,
  SCHED_SOFTIRQ,
  HRTIMER_SOFTIRQ,
  RCU_SOFTIRQ,    /* Preferable RCU should always be the last softirq */
 
  EMBETRONICX_SOFT_IRQ,    /* Manually added for educational purpose */

  NR_SOFTIRQS
};

- In this enum, we have added a new element called EMBETRONICX_SOFT_IRQ. 
- We can use this enum for our softirq. 
- If you want higher priority softirq, then add the element after HI_SOFTIRQ or 
even before that.
------------------------------------------------------------------------------------------
Create the softirq handler

- We have to create our own callback function to do our operations in the bottom half. 
- When the kernel runs a softirq handler, it executes this callback function. 
- The callback function should be created like this prototype.

void softirq_handler(struct softirq_action *);
Example:

/*
** This function is the softirq handler. We are toggling the LED.
*/
static void gpio_interrupt_softirq_handler(struct softirq_action *action)
{
  led_toggle = (0x01 ^ led_toggle);                             // toggle the old value
  gpio_set_value(GPIO_21_OUT, led_toggle);                      // toggle the GPIO_21_OUT
  pr_info("Interrupt Occurred : GPIO_21_OUT : %d ",gpio_get_value(GPIO_21_OUT));
}
------------------------------------------------------------------------------------------
Register the softirq with the handler

- Till now, we have created the softirq entry and the softirq handler. 
- Now, we have to register the softirq entry with the softirq handler. 
- To do that, use the below API.

void open_softirq( int nr, void (*action)(struct softirq_action *) );

nr – Softirq entry. In our case, it should be EMBETRONICX_SOFT_IRQ .
action– Softirq handler
------------------------------------------------------------------------------------------
Trigger the softirq

- We are all set. When we want to call our softirq handler, we have to trigger( raise) 
the softirq from the ISR. 

- The below API is used to trigger the softirq.

void raise_softirq(unsigned int nr);

nr – Softirq entry. In our case, it should be EMBETRONICX_SOFT_IRQ .

- This API will mark this EMBETRONICX_SOFT_IRQ as pending. 

- So, the softirq handler will be called when the processor calls "do_softirq" next time. 

- If the interrupt is already off, then you can use the 
raise_softirq_irqoff(EMBETRONICX_SOFT_IRQ) directly instead of 
raise_softirq(EMBETRONICX_SOFT_IRQ)

- Note: As these APIs ( raise_softirq, raise_softirq_irqoff, open_softirq) are not 
exported by default, you have to manually export those APIs in the Linux kernel source code
------------------------------------------------------------------------------------------
When will be our softirq handler called "?"

- If we marked the softirq as pending using the raise_softirq or raise_softirq_irqoff, 

- Pending softirqs are checked and executed in the following places.

1. In the return from hardware interrupt code (ISR)
2. In the ksoftirqd kernel thread
3. In any code that explicitly checks for and executes pending softirqs, such as the 
networking subsystem
------------------------------------------------------------------------------------------
Points to be noted

- A softirq never preempts another softirq. 
>> Either - once A Softirq is running - another softirq cannot be started - by same thread
until - the one executing - finishes - so the softirqs are serialized

- In fact, the only event that can preempt a softirq is an interrupt handler
>> Interrupt handler can preempt the sofirq execution
>> But scheduler preemption will be disabled when softirq is running
>> If the do_softirq is invoked from ISR - return - the interrupts may be still disabled
until softirq is running - but there is timer counter for this - so it cannot disable
the IRQ for a long time - it will mark the pending softirqs and exit - interrupts
are enabled back - Next when ksoftirqd starts executing softirqs - it will run with
interrupts enabled - but shceduler disabled

- Another softirq even the same one can run on another processor, however.
>> Yes another softirq of same type can run on another CPU - as the IRQs can be handelled
local to the CPU - and return from ISR - can invoke "do_softirq()" - which will execute
softirqs - so spinlocks() are needed here 
>> but it will never execute same tasklets on different CPU - if its already running
on one CPU - here we need not worry about locking

- Softirqs are reserved for the most timing-critical and important bottom-half processing 
on the system. 

- If you are adding a new softirq, you normally want to ask yourself why using a tasklet 
is insufficient. 

- Tasklets are dynamically created and are simpler to use because of their weaker locking 
requirements, and they still perform quite well. 

- Nonetheless, for "timing-critical applications" that are able to do their own locking in 
an efficient way, softirqs might be the correct solution.
>> The only reason to use SOFT_IRQ is because it is time-critical to be handled

- Softirqs are most often raised from within interrupt handlers - meaing - marked as
pending - so that when return from ISR - it will call "do_softirq()" to execute it
------------------------------------------------------------------------------------------

Kernel code changes

- As we have told you already, we have to make changes in the Linux kernel source code to 
implement this softirq. 
- These are the below changes that we have to make in the Linux kernel source code.

1. Add element EMBETRONICX_SOFT_IRQ in the enum in <linux/interrupt.h>
2. Export these APIs ( raise_softirq, raise_softirq_irqoff, open_softirq)
------------------------------------------------------------------------------------------
- Added the kernel source code diff below for your reference.

[email protected]:~/Desktop/workspace/linux_cus_kernel/linux $ git diff
diff --git a/include/linux/interrupt.h b/include/linux/interrupt.h
index ee8299eb1..0553cb127 100644
--- a/include/linux/interrupt.h
+++ b/include/linux/interrupt.h
@@ -537,6 +537,7 @@ enum
        SCHED_SOFTIRQ,
        HRTIMER_SOFTIRQ,
        RCU_SOFTIRQ,    /* Preferable RCU should always be the last softirq */
+        EMBETRONICX_SOFT_IRQ,    /* Manually added for educational purpose */
 
        NR_SOFTIRQS
 };
diff --git a/kernel/softirq.c b/kernel/softirq.c
index 09229ad82..887dd60d3 100644
--- a/kernel/softirq.c
+++ b/kernel/softirq.c
@@ -469,6 +469,7 @@ inline void raise_softirq_irqoff(unsigned int nr)
        if (!in_interrupt())
                wakeup_softirqd();
 }
+EXPORT_SYMBOL(raise_softirq_irqoff);
 
 void raise_softirq(unsigned int nr)
 {
@@ -478,6 +479,7 @@ void raise_softirq(unsigned int nr)
        raise_softirq_irqoff(nr);
        local_irq_restore(flags);
 }
+EXPORT_SYMBOL(raise_softirq);
 
 void __raise_softirq_irqoff(unsigned int nr)
 {
@@ -490,6 +492,7 @@ void open_softirq(int nr, void (*action)(struct softirq_action *))
 {
        softirq_vec[nr].action = action;
 }
+EXPORT_SYMBOL(open_softirq);
 
 /*
  * Tasklets
*/
------------------------------------------------------------------------------------------
- As we are using the Raspberry Pi – 4, we have used this reference to compile the Linux 
kernel.
------------------------------------------------------------------------------------------
Softirq without kernel code changes

- Some people will be in a situation where they cannot able to edit the source code and 
they don’t want to take this risk. 
- They simply want to learn the softirq. If you fall under that category, 
then you can try this method.
------------------------------------------------------------------------------------------
- As we see in the kernel code, there are many other softirqs are registered already, 
you can use that.

Example:
- In this example, we have used the HRTIMER_SOFTIRQ softriq for educational purposes. 
- If you want to use the softirq in your project, then please create one instead of 
using the existing one. 
- This may break some other functionalities.
------------------------------------------------------------------------------------------
static void gpio_interrupt_softirq_handler(struct softirq_action *action) 
{
  /* do your task */
}

/* Assign gpio_interrupt_softirq_handler to the HRTIMER_SOFTIRQ*/
open_softirq( HRTIMER_SOFTIRQ, gpio_interrupt_softirq_handler );

/* Raise the softirq */
raise_softirq(HRTIMER_SOFTIRQ);
------------------------------------------------------------------------------------------

Softirq in Linux – Example Programming

Concept
- As we are using the GPIO interrupt, the ISR handler gpio_irq_handler will be called 
when we press the button or vibration is detected in the vibration sensor. 

- In the ISR, we are raising the softirq using raise_softirq. 
- Then the processor will call the softirq handler gpio_interrupt_softirq_handler. 

- In that softirq handler, we are toggling the GPIO where the LED has been connected. 
- Please look at the softirq Linux source code below.
------------------------------------------------------------------------------------------
Driver Source Code
[Get the full updated source code from GitHub]

/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple GPIO driver explanation (Softirq)
* 
*              We made the below changes in the linux kernel to support the 
*              softirq.
*           
*              1. Added EMBETRONICX_SOFT_IRQ in interrupt.h
*              2. Exported the APIs(raise_softirq, raise_softirq_irqoff, open_softirq)
*              3. Compiled the modified kernel and used that
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.10.27-v7l-embetronicx-custom+
*
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/gpio.h>     //GPIO
#include <linux/interrupt.h>
#include <linux/err.h>
/* Since debounce is not supported in Raspberry pi, I have addded this to disable 
** the false detection (multiple IRQ trigger for one interrupt).
** Many other hardware supports GPIO debounce, I don't want care about this even 
** if this has any overhead. Our intention is to explain the GPIO interrupt.
** If you want to disable this extra coding, you can comment the below macro.
** This has been taken from : https://raspberrypi.stackexchange.com/questions/8544/gpio-interrupt-debounce
**
** If you want to use Hardaware Debounce, then comment this EN_DEBOUNCE.
**
*/
#define EN_DEBOUNCE

#ifdef EN_DEBOUNCE
#include <linux/jiffies.h>

extern unsigned long volatile jiffies;
unsigned long old_jiffie = 0;
#endif

//LED is connected to this GPIO
#define GPIO_21_OUT (21)

//LED is connected to this GPIO
#define GPIO_25_IN  (25)

//GPIO_25_IN value toggle
unsigned int led_toggle = 0; 

//This used for storing the IRQ number for the GPIO
unsigned int GPIO_irqNumber;

//Interrupt handler for GPIO 25. This will be called whenever there is a raising edge detected. 
static irqreturn_t gpio_irq_handler(int irq,void *dev_id) 
{
  
#ifdef EN_DEBOUNCE
   unsigned long diff = jiffies - old_jiffie;
   if (diff < 20)
   {
     return IRQ_HANDLED;
   }
  
  old_jiffie = jiffies;
#endif  
  
  /* Raise the softirq */
  raise_softirq( EMBETRONICX_SOFT_IRQ );
    
  return IRQ_HANDLED;
}

/*
** This fuction is the softirq handler
*/
static void gpio_interrupt_softirq_handler(struct softirq_action *action)
{
  led_toggle = (0x01 ^ led_toggle);                             // toggle the old value
  gpio_set_value(GPIO_21_OUT, led_toggle);                      // toggle the GPIO_21_OUT
  pr_info("Interrupt Occurred : GPIO_21_OUT : %d ",gpio_get_value(GPIO_21_OUT));
}


dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
 
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
 
 
/*************** Driver functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);
/******************************************************/

//File operation structure 
static struct file_operations fops =
{
  .owner          = THIS_MODULE,
  .read           = etx_read,
  .write          = etx_write,
  .open           = etx_open,
  .release        = etx_release,
};

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
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
  uint8_t gpio_state = 0;
  
  //reading GPIO value
  gpio_state = gpio_get_value(GPIO_21_OUT);
  
  //write to user
  len = 1;
  if( copy_to_user(buf, &gpio_state, len) > 0) {
    pr_err("ERROR: Not all the bytes have been copied to user\n");
  }
  
  pr_info("Read function : GPIO_21 = %d \n", gpio_state);
  
  return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp, 
                const char __user *buf, size_t len, loff_t *off)
{
  uint8_t rec_buf[10] = {0};
  
  if( copy_from_user( rec_buf, buf, len ) > 0) {
    pr_err("ERROR: Not all the bytes have been copied from user\n");
  }
  
  pr_info("Write Function : GPIO_21 Set = %c\n", rec_buf[0]);
  
  if (rec_buf[0]=='1') {
    //set the GPIO value to HIGH
    gpio_set_value(GPIO_21_OUT, 1);
  } else if (rec_buf[0]=='0') {
    //set the GPIO value to LOW
    gpio_set_value(GPIO_21_OUT, 0);
  } else {
    pr_err("Unknown command : Please provide either 1 or 0 \n");
  }
  
  return len;
}

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
  /*Allocating Major number*/
  if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
    pr_err("Cannot allocate major number\n");
    goto r_unreg;
  }
  pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

  /*Creating cdev structure*/
  cdev_init(&etx_cdev,&fops);

  /*Adding character device to the system*/
  if((cdev_add(&etx_cdev,dev,1)) < 0){
    pr_err("Cannot add the device to the system\n");
    goto r_del;
  }

  /*Creating struct class*/
  if(IS_ERR(dev_class = class_create(THIS_MODULE,"etx_class"))){
    pr_err("Cannot create the struct class\n");
    goto r_class;
  }

  /*Creating device*/
  if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"etx_device"))){
    pr_err( "Cannot create the Device \n");
    goto r_device;
  }
  
  //Output GPIO configuration
  //Checking the GPIO is valid or not
  if(gpio_is_valid(GPIO_21_OUT) == false){
    pr_err("GPIO %d is not valid\n", GPIO_21_OUT);
    goto r_device;
  }
  
  //Requesting the GPIO
  if(gpio_request(GPIO_21_OUT,"GPIO_21_OUT") < 0){
    pr_err("ERROR: GPIO %d request\n", GPIO_21_OUT);
    goto r_gpio_out;
  }
  
  //configure the GPIO as output
  gpio_direction_output(GPIO_21_OUT, 0);
  
  //Input GPIO configuratioin
  //Checking the GPIO is valid or not
  if(gpio_is_valid(GPIO_25_IN) == false){
    pr_err("GPIO %d is not valid\n", GPIO_25_IN);
    goto r_gpio_in;
  }
  
  //Requesting the GPIO
  if(gpio_request(GPIO_25_IN,"GPIO_25_IN") < 0){
    pr_err("ERROR: GPIO %d request\n", GPIO_25_IN);
    goto r_gpio_in;
  }
  
  //configure the GPIO as input
  gpio_direction_input(GPIO_25_IN);
  
  /*
  ** I have commented the below few lines, as gpio_set_debounce is not supported 
  ** in the Raspberry pi. So we are using EN_DEBOUNCE to handle this in this driver.
  */ 
#ifndef EN_DEBOUNCE
  //Debounce the button with a delay of 200ms
  if(gpio_set_debounce(GPIO_25_IN, 200) < 0){
    pr_err("ERROR: gpio_set_debounce - %d\n", GPIO_25_IN);
    //goto r_gpio_in;
  }
#endif
  
  //Get the IRQ number for our GPIO
  GPIO_irqNumber = gpio_to_irq(GPIO_25_IN);
  pr_info("GPIO_irqNumber = %d\n", GPIO_irqNumber);
  
  if (request_irq(GPIO_irqNumber,             //IRQ number
                  (void *)gpio_irq_handler,   //IRQ handler
                  IRQF_TRIGGER_RISING,        //Handler will be called in raising edge
                  "etx_device",               //used to identify the device name using this IRQ
                  NULL)) {                    //device id for shared IRQ
    pr_err("my_device: cannot register IRQ ");
    goto r_gpio_in;
  }
  
  /* Assign gpio_interrupt_softirq_handler to the EMBETRONICX_SOFT_IRQ */
  open_softirq( EMBETRONICX_SOFT_IRQ, gpio_interrupt_softirq_handler );
 
  pr_info("Device Driver Insert...Done!!!\n");
  return 0;

r_gpio_in:
  gpio_free(GPIO_25_IN);
r_gpio_out:
  gpio_free(GPIO_21_OUT);
r_device:
  device_destroy(dev_class,dev);
r_class:
  class_destroy(dev_class);
r_del:
  cdev_del(&etx_cdev);
r_unreg:
  unregister_chrdev_region(dev,1);
  
  return -1;
}

/*
** Module exit function
*/ 
static void __exit etx_driver_exit(void)
{
  free_irq(GPIO_irqNumber,NULL);
  gpio_free(GPIO_25_IN);
  gpio_free(GPIO_21_OUT);
  device_destroy(dev_class,dev);
  class_destroy(dev_class);
  cdev_del(&etx_cdev);
  unregister_chrdev_region(dev, 1);
  pr_info("Device Driver Remove...Done!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <[email protected]>");
MODULE_DESCRIPTION("A simple device driver - SoftIRQ (GPIO Interrupt) ");
MODULE_VERSION("1.42");
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
Just press the button or vibrate the vibration sensor.
That output LED should be toggled (This has been done from the softirq handler).
Also, check the dmesg
[  +0.000352] GPIO_irqNumber = 67
[  +0.000044] Device Driver Insert...Done!!!
[Apr11 17:07] Interrupt Occurred : GPIO_21_OUT : 1

##########################################################################################


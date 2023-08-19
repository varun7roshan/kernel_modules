##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################
GPIO Linux Device Driver Basic using Raspberry PI

Hardware Required
Raspberry Pi
Breadboard
Resistor
LED

Bring up Raspberry PI
- Install Raspberry Pi OS (32-bit) with desktop on the SD card.
- Then install the kernel header using sudo apt install raspberrypi-kernel-headers
- For your information, In my Raspberry PI 4 board, kernel 5.4.51-v7l+ is installed.

- Now we are ready to write the GPIO Linux Device Driver. Before we start, 
we will discuss the GPIO API which is available in the Linux kernel.
-------------------------------------------------------------------------------------------
Accessing the GPIO in Linux Kernel

- If anyone wants to access the GPIO from the Kernel GPIO subsystem, they have to 
follow the below steps.

- Verify whether the GPIO is valid or not.
- If it is valid, then you can request the GPIO from the Kernel GPIO subsystem.
- Export the GPIO to sysfs (This is optional).
- Set the direction of the GPIO
- Make the GPIO to High/Low if it is set as an output pin.
- Set the debounce-interval and read the state if it is set as an input pin. 
- You enable IRQ also for edge/level triggered.
- Then release the GPIO while exiting the driver or once you are done.

- The APIs used to do the above steps are given below. Keep reading guys.
-------------------------------------------------------------------------------------------
GPIO APIs in Linux kernel

- Already Kernel GPIO subsystems have provided the APIs to access the GPIOs. 
- We will explain them one by one and its uses. Let’s go. You need to include the 
GPIO header file given below.

#include <linux/gpio.h>
-------------------------------------------------------------------------------------------
Validate the GPIO

- Before using the GPIO, we must check whether the GPIO that we are planning to use is 
valid or not. To do that we have to use the below API.

bool gpio_is_valid(int gpio_number);

gpio_number : GPIO that you are planning to use

- It returns false if it is not valid otherwise, it returns true.

- This API determines whether the GPIO number is valid or not. 
- Sometimes this call will return false even if you send a valid number. 
- Because that GPIO pin might be temporarily unused on a given board.
-------------------------------------------------------------------------------------------
Request the GPIO

- Once you have validated the GPIO, then you can request the GPIO using the below APIs.

int gpio_request(unsigned gpio, const char *label)

gpio : GPIO that you are planning to use.

label :  label used by the kernel for the GPIO in sysfs. 
- You can provide any string that can be seen in /sys/kernel/debug/gpio. 
- Do cat /sys/kernel/debug/gpio. You can see the GPIO assigned to the particular GPIO.

It returns 0 in success and a negative number in failure.
-------------------------------------------------------------------------------------------
Note: GPIOs must be allocated before use.

There are other variants also available. You can use any one of them based on your need.

int gpio_request_one(unsigned gpio, unsigned long flags, const char *label); 
- Request one GPIO.
-------------------------------------------------------------------------------------------
int gpio_request_array(struct gpio *array, size_t num);
- Request multiple GPIOs.
-------------------------------------------------------------------------------------------
Export the GPIO
- For debugging purposes, you can export the GPIO which is allocated using the 
gpio_request() to the sysfs using the below API.

int gpio_export(unsigned int gpio, bool direction_may_change);

gpio :  GPIO that you want to export.

direction_may_change :  This parameter controls whether user space is allowed to change 
the direction of the GPIO. true – Can change, false – Can’t change.

Returns zero on success, else an error.

- Once you export the GPIO, you can see the GPIO in /sys/class/gpio/*. */There you can the 
GPIO’s value, etc. The example is given below.
-------------------------------------------------------------------------------------------
Unexport the GPIO

- If you have exported the GPIO using the gpio_export(), then you can unexport this 
using the below API.

void gpio_unexport(unsigned int gpio)

gpio : GPIO that you want to unexport
-------------------------------------------------------------------------------------------
Set the direction of the GPIO

- When you are working on the GPIOs, you have set the GPIO as output or input. 
- The below APIs are used to achieve that.
-------------------------------------------------------------------------------------------
This API is used to set the GPIO direction as input.

int  gpio_direction_input(unsigned gpio)

gpio : GPIO that you want to set the direction as input.

Returns zero on success, else an error.
-------------------------------------------------------------------------------------------
This API is used to set the GPIO direction as output.

int  gpio_direction_output(unsigned gpio, int value)

gpio : GPIO that you want to set the direction as output.

value : The value of the GPIO once the output direction is effective.

Returns zero on success, else an error.
-------------------------------------------------------------------------------------------
Change the GPIO value

- Once you set the GPIO direction as an output, then you can use the below API to 
change the GPIO value.

gpio_set_value(unsigned int gpio, int value);

gpio :  GPIO that you want to change the value.
value : value to set to the GPIO. 0 – Low, 1 – High.
-------------------------------------------------------------------------------------------
Read the GPIO value

- You can read the GPIO’s value using the below API.

int  gpio_get_value(unsigned gpio);

gpio :  GPIO that you want to read the value.

It returns the GPIO’s value. 0 or 1.
-------------------------------------------------------------------------------------------
GPIO Interrupt (IRQ)
This concept will be part of the next tutorial.
-------------------------------------------------------------------------------------------
Release the GPIO

- Once you have done with the GPIO, then you can release the GPIO which you have 
allocated previously. The below API help in that case.

void gpio_free(unsigned int gpio);

gpio :  GPIO that you want to release.
-------------------------------------------------------------------------------------------
There are other variants also available. You can use any one of them based on your need.

void gpio_free_array(struct gpio *array, size_t num);
- Release multiple GPIOs.

- Now we have seen the APIs that are available in the GPIO subsystem. 
Now let’s see the GPIOs available in Raspberry Pi.
-------------------------------------------------------------------------------------------
GPIO in Raspberry PI

- 40-pins are there in the Raspberry Pi 4 Model B. You can refer here for the GPIO 
pinouts and its details.
-------------------------------------------------------------------------------------------
Example Programming

- This is a very basic GPIO tutorial. So I am not going to add all the functionalities 
to this driver. 
- I will connect the one LED in the GPIO 21. And I will turn ON the LED by writing “1” to 
the driver. If I write “0”, then the LED will go OFF.

##########################################################################################
/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple GPIO driver explanation
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
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
#include <linux/err.h>
//LED is connected to this GPIO
#define GPIO_21 (21)
 
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
  gpio_state = gpio_get_value(GPIO_21);
  
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
    gpio_set_value(GPIO_21, 1);
  } else if (rec_buf[0]=='0') {
    //set the GPIO value to LOW
    gpio_set_value(GPIO_21, 0);
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
  
  //Checking the GPIO is valid or not
  if(gpio_is_valid(GPIO_21) == false){
    pr_err("GPIO %d is not valid\n", GPIO_21);
    goto r_device;
  }
  
  //Requesting the GPIO
  if(gpio_request(GPIO_21,"GPIO_21") < 0){
    pr_err("ERROR: GPIO %d request\n", GPIO_21);
    goto r_gpio;
  }
  
  //configure the GPIO as output
  gpio_direction_output(GPIO_21, 0);
  
  /* Using this call the GPIO 21 will be visible in /sys/class/gpio/
  ** Now you can change the gpio values by using below commands also.
  ** echo 1 > /sys/class/gpio/gpio21/value  (turn ON the LED)
  ** echo 0 > /sys/class/gpio/gpio21/value  (turn OFF the LED)
  ** cat /sys/class/gpio/gpio21/value  (read the value LED)
  ** 
  ** the second argument prevents the direction from being changed.
  */
  gpio_export(GPIO_21, false);
  
  pr_info("Device Driver Insert...Done!!!\n");
  return 0;
 
r_gpio:
  gpio_free(GPIO_21);
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
  gpio_unexport(GPIO_21);
  gpio_free(GPIO_21);
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
MODULE_DESCRIPTION("A simple device driver - GPIO Driver");
MODULE_VERSION("1.32");

##########################################################################################
										GPIO - Interrupt

Accessing the input GPIO in Linux Kernel

- Verify the GPIO is valid or not.
- If it is valid, then you can request the GPIO from the Kernel GPIO subsystem.
- Set the direction of the GPIO as an input
- Set the debounce-interval
- Read the GPIO.
- You enable IRQ also for edge/level triggered if you need it.
- Then release the GPIO while exiting the driver or once you are done.
-------------------------------------------------------------------------------------------
GPIO APIs in Linux kernel

Set the debounce-interval
- The below API is used for sets debounce time for a GPIO. 
- Right now raspberry pi is not supporting that. That’s why we have commented on that 
line in our source code. 
- Instead, we used some software hack to eliminate multiple times IRQ being called for a 
single rising edge transition. 
- You can remove the software hack and uncomment that gpio_set_debounce() if your 
microcontroller supports this.

int gpiod_set_debounce(unsigned gpio, unsigned debounce);

gpio :  GPIO that you want to set the debounce value.
debounce : Delay of the debounce.

It returns 0 on success. otherwise, <0.
-------------------------------------------------------------------------------------------
Get the IRQ number for the GPIO

- Using the below API, you can get the IRQ number for the specific GPIO.

int gpio_to_irq(unsigned gpio);

gpio :  GPIO that you want to get the IRQ number.
-------------------------------------------------------------------------------------------
Request the IRQ

- Here is where you need to register the GPIO IRQ number and its handler to the 
Linux Interrupts with the proper interrupt flags. 
- You can find the details regarding requesting the GPIO here. 
- Please note that you have to free the IRQ once you are done with the interrupt.
-------------------------------------------------------------------------------------------
Interrupt Flags

- While registering the GPIO interrupt using request_irq() you can use any one of the 
flags based on your need.

IRQF_TRIGGER_RISING:;  
IRQF_TRIGGER_FALLING:;
IRQF_TRIGGER_HIGH:;   
IRQF_TRIGGER_LOW:;
-------------------------------------------------------------------------------------------
So we are all set. Let’s jump into the programming.

Example Programming
- In this example, I have just taken the 2 GPIOs. 
- One is for the input and one is for the output.

GPIO 21 – OUTPUT
GPIO 25 – INPUT

- LED has been connected to the OUTPUT pin (GPIO 21) and the Vibration sensor has been 
connected to the INPUT pin (GPIO 25). 
- You can connect the push-button also into the INPUT pin.

- So the concept is whenever the vibration is detected, it will toggle the LED. 
Just simple right. Let’s write the code.

##########################################################################################

/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple GPIO driver explanation (GPIO Interrupt)
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
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
  static unsigned long flags = 0;
  
#ifdef EN_DEBOUNCE
   unsigned long diff = jiffies - old_jiffie;
   if (diff < 20)
   {
     return IRQ_HANDLED;
   }
  
  old_jiffie = jiffies;
#endif  

/*
This makes sense, because interrupts are disabled at different levels.

disable_irq(irq_clk);
- This code disables (masks) interrupt in interrupt controller. 
- If you have disabled interrupt at this level, the interrupt wouldn't be passed to 
internal interrupt controller pipeline. 
- It would not be prioritized, it would not be routed to the destination CPU.

local_irq_save(flags);
- This code disables all interrupts at the level of CPU IRQ interface. 
- Interrupts are passed to CPU IRQ interface after they have passed priority & 
routing pipeline of the interrupt controller and have been asserted to the CPU.
*/

  local_irq_save(flags); //disabling the local IRQ on this CPU - but IRQ can occur on CPU-2
  led_toggle = (0x01 ^ led_toggle);                             // toggle the old value
  gpio_set_value(GPIO_21_OUT, led_toggle);                      // toggle the GPIO_21_OUT
  pr_info("Interrupt Occurred : GPIO_21_OUT : %d ",gpio_get_value(GPIO_21_OUT));
  local_irq_restore(flags);
  return IRQ_HANDLED;
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
MODULE_DESCRIPTION("A simple device driver - GPIO Driver (GPIO Interrupt) ");
MODULE_VERSION("1.33");

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




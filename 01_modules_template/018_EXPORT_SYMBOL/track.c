##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################
EXPORT_SYMBOL in Linux Device Driver

Introduction
- In a programming language, a symbol is either a variable or a function.
- Or more generally, we can say, a symbol is a name representing space in the memory, 
which stores data (variable, for reading and writing) or instructions (function, for 
executing).

- When you look at some kernel codes, you may find EXPORT_SYMBOL() very often. 

- In the Linux Kernel 2.4, all the non-static symbols are exported to the kernel space 
automatically. 
- But later, in Linux Kernel 2.6 instead of exporting all non-static symbols, they wanted 
to export the only symbols which are marked by EXPORT_SYMBOL() macro.
------------------------------------------------------------------------------------------
EXPORT_SYMBOL’s role

- When some symbols (variables or functions) are using EXPORT_SYMBOL macro 
(ex. EXPORT_SYMBOL(func_name)), those symbols are exposed to all the loadable kernel 
drivers. 
- You can call them directly in your kernel module without modifying the kernel code. 
- In other words, It tells the kbuild mechanism that the symbol referred to should be part 
of the global list of kernel symbols. That allows the kernel modules to access them.

- Only the symbols that have been explicitly exported can be used by other modules.

- Another macro is also available to export the symbols like EXPORT_SYMBOL. 
That is EXPORT_SYMBOL_GPL().

EXPORT_SYMBOL: exports the symbol to any loadable module.
EXPORT_SYMBOL_GPL: exports the symbol only to GPL-licensed modules.
------------------------------------------------------------------------------------------
How to use EXPORT_SYMBOL "?"

- Declare and define the symbol (functions or variables) which you want to make it visible 
to other kernel modules. 
- Then below the definition, use EXPORT_SYMBOL(symbol name). Now it is visible to all 
loadable modules.

- Now take the kernel driver who is gonna use the above-exported symbol. 
- Declare the symbol using extern. Then use the symbol directly.

- Finally, load the module first, who has the definition of the export symbol. 
- Then load the caller module using insmod“.

Limitation:
- That symbol should not be static or inline.
- Order of loading the driver is matter. ie. We should load the module which has the 
definition of the symbol, then only we can load the module that is using that symbol.
------------------------------------------------------------------------------------------
Driver Source Code – EXPORT_SYMBOL in Linux

- First, I will explain to you the concept of driver code attached below.

In this tutorial, we have two drivers.

- Driver 1 has one function called etx_shared_func and one global variable called etx_count
- This function and variable have been shared among with all the loadable modules using 
EXPORT_SYMBOL.

- Driver 2 will be using that variable and function which are shared by Driver 1. 
- When we read Driver 2, then it will call the shared function and we can read that 
variable also.

##########################################################################################
/***************************************************************************//**
*  \file       driver1.c
*
*  \details    Simple Linux device driver (EXPORT_SYMBOL)
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
#include <linux/err.h>

int etx_count = 0;              //Exported variable

/*
** exported function
*/ 
void etx_shared_func(void)
{
        pr_info("Shared function been called!!!\n");
        etx_count++;
}

//EXPORT_SYMBOL_GPL(etx_shared_func);
EXPORT_SYMBOL(etx_shared_func);
EXPORT_SYMBOL(etx_count);
 
/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
        /*Allocating Major number*/
}

##########################################################################################
/***************************************************************************//**
*  \file       driver2.c
*
*  \details    Simple Linux device driver (EXPORT_SYMBOL)
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
#include <linux/err.h>

extern int etx_count; //---------------------> Marking the variable as "EXTERN" is imp
void etx_shared_func(void); //Function declaration is by defalut extern
 
/*
** This function will be called when we read the Device file
*/
static ssize_t etx_read(struct file *filp, 
                                char __user *buf, size_t len, loff_t *off)
{
        etx_shared_func();
        pr_info("%d time(s) shared function called!\n", etx_count);
        pr_info("Data Read : Done!\n");
        return 0;
}

##########################################################################################
Compiling and Testing Driver

- Build the driver by using Makefile (sudo make)
- After compiling, you can able to see the file named as "Module.symvers". 
If you open that file, then our shared function and variable will be mentioned there.

0x1db7034a etx_shared_func /home/embetronicx/driver/driver1            EXPORT_SYMBOL
0x6dcb135c etx_count      /home/embetronicx/driver/driver1             EXPORT_SYMBOL

Load driver 1 using sudo insmod driver1.ko(Driver 1 should be loaded first. If you try to load Driver 2 first, then you will get an error like “insmod: ERROR: could not insert module driver2.ko: Unknown symbol in module“).
Load driver 2 using sudo insmod driver2.ko
Now check the dmesg

##########################################################################################


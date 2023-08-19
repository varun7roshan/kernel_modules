##########################################################################################
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/moduleparam.h>

##########################################################################################
modinfo - to get the variables - registered with module_param()
>> Strange that "module_param_cb()" registered variable does not show in modinfo

root@pi:/home/pi# modinfo passing_args.ko
filename:       /home/pi/passing_args.ko
board:          Raspberry_PI_3_Model_B
description:    Template Kernel Module
author:         Varun
license:        GPL
srcversion:     2F786E747972FAD821B2215
depends:
name:           passing_args
vermagic:       4.19.127-v7+ SMP mod_unload modversions ARMv7 p2v8
parm:           my_int:int
parm:           my_string:charp
parm:           my_array:array of int

insmod passing_args.ko my_string="I_have_changed_it_now"
>> Notice that there must be no spaces in string - else it will save only the first word

insmod hello.ko valueETX=14 nameETX="EmbeTronicX" arr_valueETX=100,102,104,106

- We can modify all the variables : 
/sys/module/passing_args/parameters/
my_array  my_int  my_notify_int  my_string

- But notfication will be sent only to the registered variable

- As a hack - we can have 1 notifiy callback - to print all the variables
- useful in debugging
##########################################################################################
								Passing Arguments to Linux Device Driver

- Module Parameters Macros:
module_param()
module_param_array()
module_param_cb()
##########################################################################################
- Before discussing these macros we have to know about the permissions of the variable:

There are several types of permissions:

S_IWUSR
S_IRUSR
S_IXUSR
S_IRGRP
S_IWGRP
S_IXGRP

In this S_I is a common header.
R = read ,W =write ,X= Execute.
USR =user ,GRP =Group

Using OR ‘|’ (or operation) we can set multiple permissions at a time.
------------------------------------------------------------------------------------------

module_param()

- This macro is used to initialize the arguments. 
- module_param takes three parameters: the name of the variable, its type, and a 
permissions mask to be used for an accompanying sysfs entry. 
- The macro should be placed outside of any function and is typically found near the 
head of the source file. 
- module_param() macro, defined in linux/moduleparam.h.

"module_param(name, type, perm);"

module_param() macro creates the sub-directory under /sys/module. For example,

module_param(valueETX, int, S_IWUSR|S_IRUSR);
This will create a sysfs entry. (/sys/module/hello_world_module/parameters/valueETX)
------------------------------------------------------------------------------------------
Numerous types are supported for module parameters:

bool
invbool
- A boolean (true or false) value (the associated variable should be of type int). 
- The invbool type inverts the value, so that true values become false and vice versa.
------------------------------------------------------------------------------------------
charp
- A char pointer value. Memory is allocated for user-provided strings, and the pointer 
is set accordingly.

int
long
short
uint
ulong
ushort
- Basic integer values of various lengths. 
The versions starting with u are for unsigned values.
------------------------------------------------------------------------------------------

module_param_array()

/**
 * module_param_array - a parameter which is an array of some type
 * @name: the name of the array variable
 * @type: the type, as per module_param()
 * @nump: optional pointer filled in with the number written
 * @perm: visibility in sysfs
 *
 * Input and output are as comma-separated values.  Commas inside values
 * don't work properly (eg. an array of charp).
 *
 * ARRAY_SIZE(@name) is used to determine the number of elements in the
 * array, so the definition must be visible.
 */
#define module_param_array(name, type, nump, perm)    \
  module_param_array_named(name, name, type, nump, perm)

- This macro is used to send the array as an argument to the Linux device driver. 
- Array parameters, where the values are supplied as a comma-separated list, are also 
supported by the module loader. 
- To declare an array parameter, use:

"module_param_array(name,type,num,perm);"

name: is the name of your array (and of the parameter),
type: is the type of the array elements,
num: is an integer variable (optional) otherwise NULL,
perm: is the usual permissions value.
------------------------------------------------------------------------------------------

module_param_cb()

#define module_param_cb(name, ops, arg, perm)

- This macro is used to register the callback. 
- Whenever the argument (parameter) got changed, this callback function will be called. 
- I think you don’t understand. Let me explain it properly.

For Example,

I have created one parameter by using module_param().

module_param(valueETX, int, S_IWUSR|S_IRUSR);
This will create a sysfs entry. (/sys/module/hello_world_module/parameters/valueETX)

You can change the value of valueETX from the command line by

sudo su
echo 1 > /sys/module/hello_world_module/parameters/valueETX
- This will update the valueETX variable. 
- But there is no way to notify your module that “valueETX” has changed.

- By using this module_param_cb() macro, we can get a notification.

- If you want to get a notification whenever the value got to change, we need to 
register our handler function to its file operation structure first.

struct kernel_param_ops 
{
 int (*set)(const char *val, const struct kernel_param *kp);
 int (*get)(char *buffer, const struct kernel_param *kp);
 void (*free)(void *arg);
};
 

For further explanation, please refer to the below program.

- When we will need this notification "?"
- I will tell you about the practical scenario. Whenever the value is set to 1, 
you have to write something into a hardware register. 
- How can you do this if the change of value variable is not notified to you "?" 
Got it "?" I think you have understood. 
- If you didn’t understand, just see the explanation posted below.

In this example, i explained all (module_param, module_param_array, module_param_cb).

For module_param(), I have created two variables. 
One is integer (valueETX) and another one is a string (nameETX).

For module_param_array(), I have created one integer array variable (arr_valueETX).

For module_param_cb(), I have created one integer variable (cb_valueETX).

- You can change all variable using their sysfs entry which is under 
/sys/module/hello_world_module/parameters/

- But you won’t get any notification when they got to change, except the variable which 
is created by module_param_cb() macro.
##########################################################################################
insmod hello.ko valueETX=14 nameETX="EmbeTronicX" arr_valueETX=100,102,104,106
##########################################################################################


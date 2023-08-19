##########################################################################################
- Tasklets - it is part of softirqs, either its part of the softirq type:
HI_SOFTIRQ: Handles high priority tasklets
TASKLET_SOFTIRQ: Handles regular tasklets

- And softirq execution is called from return from ISR - do_softirq()
>>>> So softirq - will execute in interrupt context - and if all of the softirqs are not
served - then at later time - ksoftirqd Thread - per-CPU Thread - will execute the 
softirqs - including Tasklets
##########################################################################################


##########################################################################################


##########################################################################################
Bottom Half

- When Interrupt triggers, Interrupt Handler should be executed very quickly and it should 
not run for more time (it should not perform time-consuming tasks). 
- If we have the interrupt handler who is doing more tasks then we need to divide it into 
two halves.

1. Top Half
2. Bottom Half

- The top Half is nothing but our interrupt handler. If we want to do less work, 
then the top half is more than enough. No need for the bottom half in that situation. 
- But if we have more work when interrupt hits, then we need the bottom half. 
- The bottom half runs in the future, at a more convenient time, with all interrupts 
enabled. So, The job of the bottom halves is to perform any interrupt-related work not 
performed by the interrupt handler.

- There are 4 bottom half mechanisms are available in Linux:

1. Workqueue – Executed in a process context.
2. Threaded IRQs
3. Softirqs – Executed in an atomic context.
4. Tasklet in Linux – Executed in an atomic context - Within Softirqs
-------------------------------------------------------------------------------------------
Tasklet in Linux Kernel

- Tasklets are used to queue up work to be done at a later time. 

- Tasklets can be run in parallel, but the same tasklet cannot be run on multiple CPUs at 
the same time.
>> Where as softirqs of same type can run in parallel
>> Also different type of tasklets can run cocurrently on 2 cpus - there are only 2 types
of Tasklets - which are part of softirqs

- Also, each tasklet will run only on the CPU that schedules it, to optimize cache usage. 

- Since the thread that queued up the tasklet must complete before it can run the tasklet, 
race conditions are naturally avoided. 

- However, this arrangement can be suboptimal, as other potentially idle CPUs cannot be 
used to run the tasklet. 

- Therefore workqueues can and should be used instead, and workqueues were already 
discussed here.

- In short, a tasklet in linux is something like a very small thread that has neither 
stack nor context of its own. Such “threads” work quickly and completely.
-------------------------------------------------------------------------------------------
Points To Remember

- Before using Tasklets, you should consider the below points.

- "Tasklets are atomic", so we cannot use sleep() and such synchronization primitives as 
mutexes, semaphores, etc. from them. 
- "But we can use spinlock"

- A tasklet only runs on the same core (CPU) that schedules it.

- Different tasklets can be running in parallel. 
- But at the same time, a tasklet cannot be called concurrently with itself, as it runs 
on one CPU only.
- Tasklets are executed by the principle of non-preemptive scheduling, one by one, in turn.

- We can schedule them with two different priorities: "normal and high"
-------------------------------------------------------------------------------------------
- We can create a tasklet in Two ways.

1. Static Method
2. Dynamic Method

In this tutorial, we will see a static method.

- Tasklet Structure
- This is the important data structure for the tasklet.

struct tasklet_struct
{
	struct tasklet_struct *next;  
  unsigned long state;         
  atomic_t count;               
  void (*func)(unsigned long); 
  unsigned long data;           
};

next: The next tasklet in line for scheduling.
state: This state denotes Tasklet’s State. TASKLET_STATE_SCHED (Scheduled) or 
TASKLET_STATE_RUN (Running).
count: It holds a nonzero value if the tasklet is disabled and 0 if it is enabled.
func: This is the main function of the tasklet. Pointer to the function that needs to 
schedule for execution at a later time.
data: Data to be passed to the function “func“.
-------------------------------------------------------------------------------------------
Create Tasklet

- The below macros used to create a tasklet.

DECLARE_TASKLET()
- This macro used to create the tasklet structure and assigns the parameters to that 
structure.

- If we are using this macro then the tasklet will be in the enabled state.

DECLARE_TASKLET(name, func, data);

name: name of the structure to be created.
func: This is the main function of the tasklet. Pointer to the function that needs to 
schedule for execution at a later time.
data: Data to be passed to the function “func“.

Example
DECLARE_TASKLET(tasklet,tasklet_fn, 1);

- Now we will see how the macro is working. When I call the macro like above, first it 
creates a tasklet structure with the name of tasklet. 
- Then it assigns the parameter to that structure. It will be looks like below.

struct tasklet_struct tasklet = { NULL, 0, 0, tasklet_fn, 1 };

                      (or)

struct tasklet_struct tasklet;
tasklet.next = NULL;
taklet.state = TASKLET_STATE_SCHED;  //Tasklet state is scheduled
tasklet.count = 0;                   //taskelet enabled
tasklet.func = tasklet_fn;           //function
tasklet.data = 1;                    //data arg
-------------------------------------------------------------------------------------------
DECLARE_TASKLET_DISABLED()

- The tasklet can be declared and set at a disabled state, which means that the tasklet 
can be scheduled, but will not run until the tasklet is specifically enabled. 
- You need to use "tasklet_enable()" to enable.

DECLARE_TASKLET_DISABLED(name, func, data);

name: name of the structure to be created.
func: This is the main function of the tasklet. 
Pointer to the function that needs to schedule for execution at a later time.
data: Data to be passed to the function “func“.
-------------------------------------------------------------------------------------------
Enable and Disable Tasklet

tasklet_enable()
- This used to enable the tasklet.

void tasklet_enable(struct tasklet_struct *t);

t: pointer to the tasklet struct
-------------------------------------------------------------------------------------------
tasklet_disable()
- This used to disable the tasklet wait for the completion of the tasklet’s operation.

void tasklet_disable(struct tasklet_struct *t);

t: pointer to the tasklet struct
-------------------------------------------------------------------------------------------
tasklet_disable_nosync()
- This used to disable immediately.

void tasklet_disable_nosync(struct tasklet_struct *t);

t: pointer to the tasklet struct
-------------------------------------------------------------------------------------------
NOTE: If the tasklet has been disabled, we can still add it to the queue for scheduling, 
but it will not be executed on the CPU until it is enabled again. 
- Moreover, if the tasklet has been disabled several times, it should be enabled exactly 
the same number of times, there is the count field in the structure for this purpose.
-------------------------------------------------------------------------------------------
Schedule the tasklet

- When we schedule the tasklet, then that tasklet is placed into one queue out of two, 
depending on the priority. 
- Queues are organized as singly-linked lists. At that, each CPU has its own queues.

There are two priorities.

Normal Priority
High Priority
-------------------------------------------------------------------------------------------
tasklet_schedule()
- Schedule a tasklet with a normal priority. If a tasklet has previously been scheduled 
(but not yet run), the new schedule will be silently discarded.

void tasklet_schedule (struct tasklet_struct *t);

t: pointer to the tasklet struct

Example
/*Scheduling Task to Tasklet*/
tasklet_schedule(&tasklet);
-------------------------------------------------------------------------------------------
tasklet_hi_schedule()

- Schedule a tasklet with high priority. If a tasklet has previously been scheduled 
(but not yet run), the new schedule will be silently discarded.

void tasklet_hi_schedule (struct tasklet_struct *t);

t: pointer to the tasklet struct
-------------------------------------------------------------------------------------------
tasklet_hi_schedule_first()

- This version avoids touching any other tasklets. Needed for kmemcheck in order not to 
take any page faults while enqueueing this tasklet. 
- Consider VERY carefully whether you really need this or tasklet_hi_schedule().

void tasklet_hi_schedule_first(struct tasklet_struct *t);

t: pointer to the tasklet struct
-------------------------------------------------------------------------------------------
Kill Tasklet

- Finally, after a tasklet has been created, it’s possible to delete a tasklet through 
these below functions.

tasklet_kill()
- This will wait for its completion and then kill it.

void tasklet_kill( struct tasklet_struct *t );

t: pointer to the tasklet struct

Example
/*Kill the Tasklet */
tasklet_kill(&tasklet);
-------------------------------------------------------------------------------------------
tasklet_kill_immediate()
- This is used only when a given CPU is in the dead state.

void tasklet_kill_immediate( struct tasklet_struct *t, unsigned int cpu );

t: pointer to the tasklet struct

cpu: CPU num

##########################################################################################
Static Creation of Tasklet :

/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple linux driver (Tasklet Static method)
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
#include<linux/sysfs.h> 
#include<linux/kobject.h> 
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/err.h>
 
#define IRQ_NO 11
 
void tasklet_fn(unsigned long); 

/* Init the Tasklet by Static Method */
DECLARE_TASKLET(tasklet,tasklet_fn, 1);
 
/*Tasklet Function*/
void tasklet_fn(unsigned long arg)
{
	printk(KERN_INFO "Executing Tasklet Function : arg = %ld\n", arg);
}
 
 
//Interrupt handler for IRQ 11. 
static irqreturn_t irq_handler(int irq,void *dev_id) 
{
	printk(KERN_INFO "Shared IRQ: Interrupt Occurred");
  /*Scheduling Task to Tasklet*/
  tasklet_schedule(&tasklet); 
        
  return IRQ_HANDLED;
}
 
/*
** This function will be called when we read the Device file
*/ 
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Read function\n");
        asm("int $0x3B");  // Corresponding to irq 11
        return 0;
}

/*
** Module Init function
*/ 
static int __init etx_driver_init(void)
{
	if(request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "etx_device", (void *)(irq_handler))) 
	{
  	printk(KERN_INFO "my_device: cannot register IRQ ");
    goto irq;
  }
 
  printk(KERN_INFO "Device Driver Insert...Done!!!\n");
  return 0;
}

/*
** Module exit function
*/  
static void __exit etx_driver_exit(void)
{
	/*Kill the Tasklet */ 
  tasklet_kill(&tasklet);
  free_irq(IRQ_NO,(void *)(irq_handler));
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
##########################################################################################
MakeFile
obj-m += driver.o
 
KDIR = /lib/modules/$(shell uname -r)/build
 
all:
    make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
    make -C $(KDIR)  M=$(shell pwd) clean
-------------------------------------------------------------------------------------------
Building and Testing Driver
- Build the driver by using Makefile (sudo make)
- Load the driver using sudo insmod driver.ko

- To trigger the interrupt read device file (sudo cat /dev/etx_device)
Now see the Dmesg (dmesg)
[email protected]: dmesg

[ 8592.698763] Major = 246 Minor = 0
[ 8592.703380] Device Driver Insert...Done!!!
[ 8601.716673] Device File Opened...!!!
[ 8601.716697] Read function
[ 8601.716727] Shared IRQ: Interrupt Occurred
[ 8601.716732] Executing Tasklet Function : arg = 1
[ 8601.716741] Device File Closed...!!!                                            
[ 8603.916741] Device Driver Remove...Done!!!
We can able to see the print “Shared IRQ: Interrupt Occurred“ and “Executing Tasklet Function: arg = 1“
Unload the module using sudo rmmod driver


##########################################################################################
													Tasklets in Linux Driver

- In our Previous Tutorial, we have seen the Tasklet using Static Method. 
- In that method, we had initialized the tasklet statically. 
- Now initialize the tasklet using dynamically. 
- So except the creation of the tasklet, everything will be the same.
>> So only creation will differ - from static to Dynamic - Reset of the tasklet are same

-------------------------------------------------------------------------------------------
Dynamically Creation of Tasklet

tasklet_init()
- This function used to Initialize the tasklet dynamically.

void tasklet_init( struct tasklet_struct *t,void(*)(unsigned long) func,unsigned long data)

t: tasklet struct that should be initialized
func: This is the main function of the tasklet. 
Pointer to the function that needs to schedule for execution at a later time.
data: Data to be passed to the function “func”.

Example
/* Tasklet by Dynamic Method */
struct tasklet_struct *tasklet;

/* Init the tasklet bt Dynamic Method */
tasklet  = kmalloc(sizeof(struct tasklet_struct),GFP_KERNEL);
if(tasklet == NULL) {
    printk(KERN_INFO "etx_device: cannot allocate Memory");
}
tasklet_init(tasklet,tasklet_fn,0);

- Now we will see how the function is working in the background.
- When I call the function like above, it assigns the parameter to the passed tasklet 
structure. It will be looks like below.

tasklet->func = tasklet_fn;             //function
tasklet->data = 0;                      //data arg
tasklet->state = TASKLET_STATE_SCHED;  //Tasklet state is scheduled
atomic_set(&tasklet->count, 0);        //taskelet enabled
-------------------------------------------------------------------------------------------
Tasklets in Linux Programming

Driver Source Code

- In that source code, When we read the device file (/dev/etx_device), interrupt will hit
- Whenever an interrupt hits, I’m scheduling the task to the tasklet. 
- I’m not going to do any job in both interrupt handler and tasklet function (only print) 
since it is a tutorial post. 
- But in a real tasklet, this function can be used to carry out any operations that need 
to be scheduled.
##########################################################################################
/****************************************************************************//**
*  \file       driver.c
*
*  \details    Simple linux driver (Tasklet Dynamic method)
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
#include<linux/sysfs.h> 
#include<linux/kobject.h> 
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/err.h>
 
#define IRQ_NO 11
 
void tasklet_fn(unsigned long); 

/* Tasklet by Dynamic Method */
struct tasklet_struct *tasklet = NULL;
 
 
/*Tasklet Function*/
void tasklet_fn(unsigned long arg)
{
        printk(KERN_INFO "Executing Tasklet Function : arg = %ld\n", arg);
}
 
 
//Interrupt handler for IRQ 11. 
static irqreturn_t irq_handler(int irq,void *dev_id) {
        printk(KERN_INFO "Shared IRQ: Interrupt Occurred");
        /*Scheduling Task to Tasklet*/
        tasklet_schedule(tasklet); 
        
        return IRQ_HANDLED;
}
 
 
/*
** This function will be called when we read the Device file
*/  
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Read function\n");
        asm("int $0x3B");  // Corresponding to irq 11
        return 0;
}

/*
** Module Init function
*/ 
static int __init etx_driver_init(void)
{
        if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "etx_device", (void *)(irq_handler))) {
            printk(KERN_INFO "etx_device: cannot register IRQ ");
            goto irq;
        }

        /* Init the tasklet bt Dynamic Method */
        tasklet  = kmalloc(sizeof(struct tasklet_struct),GFP_KERNEL);
        if(tasklet == NULL) {
            printk(KERN_INFO "etx_device: cannot allocate Memory");
            goto irq;
        }
        tasklet_init(tasklet,tasklet_fn,0);
 
        printk(KERN_INFO "Device Driver Insert...Done!!!\n");
        return 0;
}

/*
** Module exit function
*/ 
static void __exit etx_driver_exit(void)
{
        /* Kill the Tasklet */ 
        tasklet_kill(tasklet);

        if(tasklet != NULL)
        {
          kfree(tasklet);
        }
        free_irq(IRQ_NO,(void *)(irq_handler));
}
##########################################################################################


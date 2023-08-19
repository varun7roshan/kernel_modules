##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################
Introduction

- Before getting to know about Mutex, let’s take an analogy first.

- You have one SPI connection. What if one thread wants to write something into that SPI 
device and another thread wants to read from that SPI device at the same time "?"
- You have one LED display. What if one thread is writing data at a different position of 
Display and another thread is writing different data at a different position of Display at 
the same time "?"
- You have one Linked List. What if one thread wants to insert something into the list and 
another one wants to delete something on the same Linked List at the same time "?"
- In all the scenarios above, the problem encountered is the same. At any given point 
"two threads are accessing a single resource".

Race Condition
- A race condition occurs when two or more threads can access shared data and they try to 
change it at the same time. 
- Because the thread scheduling algorithm can swap between threads at any time, we don’t 
know the order in which the threads will attempt to access the shared data. 
- Therefore, the result of the change in data is dependent on the thread scheduling 
algorithm, i.e. both threads are “racing” to access/change the data.

- To avoid race conditions, we have many ways like Semaphore, Spinlock, and Mutex. 
-------------------------------------------------------------------------------------------
Mutex
- A mutex is a mutual exclusion lock. Only one thread can hold the lock.

- A mutex can be used to prevent the simultaneous execution of a block of code by multiple 
threads that are running in single or multiple processes.

- Mutex is used as a synchronization primitive in situations where a resource has to be 
shared by multiple threads simultaneously.

- "A mutex has ownership. The thread which locks a Mutex must also unlock it"

- So whenever you are accessing a shared resource that time first we lock the mutex and 
then access the shared resource. 
- When we are finished with that shared resource then we unlock the Mutex.

- I hope you got some idea about Mutex. Now, let us look at Mutex in the Linux Kernel.
-------------------------------------------------------------------------------------------
Mutex in Linux Kernel

- Today most major operating systems employ multitasking. Multitasking is where multiple 
threads can execute in parallel and thereby utilizing the CPU in an optimum way. 
- Even though, multitasking is useful, if not implemented cautiously can lead to 
concurrency issues (Race condition), which can be very difficult to handle.

- The actual mutex type (minus debugging fields) is quite simple:

struct mutex 
{
    atomic_t        count;
    spinlock_t      wait_lock;
    struct list_head    wait_list;
};

- We will be using this structure for Mutex in the Linux kernel. 
- Refer to Linux/include/linux/mutex.h
-------------------------------------------------------------------------------------------
Initializing Mutex
We can initialize Mutex in two ways

Static Method
Dynamic Method
-------------------------------------------------------------------------------------------
Static Method
This method will be useful while using global Mutex. This macro is defined below.

DEFINE_MUTEX(name)

This call defines and initializes a mutex. Refer to Linux/include/linux/mutex.h
-------------------------------------------------------------------------------------------
Dynamic Method

- This method will be useful for per-object mutexes when the mutex is just a field in a 
heap-allocated object. This macro is defined below.

mutex_init(struct mutex *lock);

struct mutex *lock – the mutex to be initialized.

This call initializes already allocated mutex. Initialize the mutex to the unlocked state.

It is not allowed to initialize an already locked mutex.

Example
struct mutex etx_mutex; 
mutex_init(&etx_mutex);
-------------------------------------------------------------------------------------------
Mutex Lock

- Once a mutex has been initialized, it can be locked by any one of them explained below.

mutex_lock()
- This is used to lock/acquire the mutex exclusively for the current task. 
- If the mutex is not available, the current task will sleep until it acquires the Mutex.

- The mutex must, later on, be released by the same task that acquired it. 
- Recursive locking is not allowed. 
- The task may not exit without first unlocking the mutex. 
- Also, kernel memory where the mutex resides must not be freed with the mutex still locked
- The mutex must first be initialized (or statically defined) before it can be locked. 
- memset-ing the mutex to 0 is not allowed.

void mutex_lock(struct mutex *lock);

struct mutex *lock – the mutex to be acquired
-------------------------------------------------------------------------------------------
mutex_lock_interruptible()

- Locks the mutex like mutex_lock, and returns 0 if the mutex has been acquired or sleeps 
until the mutex becomes available. 
- If a signal arrives while waiting for the lock then this function returns -EINTR.

int mutex_lock_interruptible(struct mutex *lock);

struct mutex *lock – the mutex to be acquired
-------------------------------------------------------------------------------------------
mutex_trylock()

- This will try to acquire the mutex, without waiting (will attempt to obtain the lock, 
but will not sleep). 
- Returns 1 if the mutex has been acquired successfully, and 0 on contention.

int mutex_trylock(struct mutex *lock);

struct mutex *lock – the mutex to be acquired

- This function must not be used in an interrupt context. 
- The mutex must be released by the same task that acquired it.
>> So when its trying to unlock the mutex in interrupt context - it must not sleep
>> If we have tryunlock() then we can use in interrupt context - but it does not make sense
to have tryunlock() as - the mutex must be released before exiting the interrupt context
>> Hence we must not use mutex in interupt context
-------------------------------------------------------------------------------------------
Mutex Unlock

- This is used to unlock/release a mutex that has been locked by a task previously.

- This function must not be used in an interrupt context. 
- Unlocking of a not locked mutex is not allowed.

void mutex_unlock(struct mutex *lock);

struct mutex *lock – the mutex to be released
-------------------------------------------------------------------------------------------
Mutex Status

- This function is used to check whether mutex has been locked or not.

int mutex_is_locked(struct mutex *lock);

struct mutex *lock – the mutex to check the status.

Returns 1 if the mutex is locked, 0 if unlocked.

##########################################################################################
/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple Linux device driver (mutex)
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
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct 
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/err.h>

struct mutex etx_mutex;
unsigned long etx_global_variable = 0;

static struct task_struct *etx_thread1;
static struct task_struct *etx_thread2; 
 
int thread_function1(void *pv);
int thread_function2(void *pv);

/*
** Thread function 1
*/
int thread_function1(void *pv)
{
    
    while(!kthread_should_stop()) {
        mutex_lock(&etx_mutex);
        etx_global_variable++;
        pr_info("In EmbeTronicX Thread Function1 %lu\n", etx_global_variable);
        mutex_unlock(&etx_mutex);
        msleep(1000);
    }
    return 0;
}

/*
** Thread function 2
*/
int thread_function2(void *pv)
{
    while(!kthread_should_stop()) {
        mutex_lock(&etx_mutex);
        etx_global_variable++;
        pr_info("In EmbeTronicX Thread Function2 %lu\n", etx_global_variable);
        mutex_unlock(&etx_mutex);
        msleep(1000);
    }
    return 0;
}

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
 
        mutex_init(&etx_mutex);
        
        /* Creating Thread 1 */
        etx_thread1 = kthread_run(thread_function1,NULL,"eTx Thread1");
        if(etx_thread1) {
            pr_err("Kthread1 Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread1\n");
             goto r_device;
        }
 
         /* Creating Thread 2 */
        etx_thread2 = kthread_run(thread_function2,NULL,"eTx Thread2");
        if(etx_thread2) {
            pr_err("Kthread2 Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread2\n");
             goto r_device;
        }
        
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
}

/*
** Module exit function
*/ 
static void __exit etx_driver_exit(void)
{
        kthread_stop(etx_thread1);
        kthread_stop(etx_thread2);
}
 
##########################################################################################


##########################################################################################
- Seqlock - is similar to "Read Write Spinlocks"
>> In "Read Write Spinlocks" - Readers are giving more importance - so writer thread
when it tries to get the write_spinlock() - it will be starved / delayed. Either it has
to wait till all readers have released the read_loc()
>> ALSO it MEANS LIKE - READERS and WRITERS are treated EQUALLY

- In Seqlock - we give importnace to writer -

##########################################################################################


##########################################################################################


##########################################################################################
Seqlock in Linux Kernel

- In our previous tutorials, we have seen some locking methods like mutex, spinlock, etc. 
- In short, When you want to treat both "write and reader operations equally", then you 
have to use spinlock.
- In some situations, we may have to give "importance to readers"
- In such a case, we can use "read-write spinlock"

- Likewise, is there any mechanism that gives "importance to writers?" Yeah, it is there 
in Linux. 
- "Seqlock is the one that gives importance to writers"

- The 2.5.60 kernel added a new type of lock called a seqlock. 
- Seqlock is a short form of "sequential lock" 
- It is a "reader-writer consistent mechanism" which is giving "importance to the writer" 
- "So this avoids the problem of writer starvation" 
- You can read here how the writer is starving while using read-write spinlock. 
- So How this seqlock is giving importance to the writer "?" Is it really useful in all 
situations "?" We will see each one by one.
-------------------------------------------------------------------------------------------
Working of seqlock

- When no one is in a critical section then one writer can enter into a critical section 
by acquiring its lock. 
- Once it took its lock then the "writer will increment the sequence number by one"
- Currently, the "sequence number is an odd value" 

- "Once done with the writing, again it will increment the sequence number by one"
- "Now the number is an even value"

- So, when the sequence number is an odd value, writing is happening. 
- When the sequence number is an even value, writing has been done. 
- Only one writer thread will be allowed in the critical section. 
- So other writers will be waiting for the lock.

- When the reader wants to read the data, first it will read the sequence number. 
- If it is an even value, then it will go to a critical section and reads the data. 
- If it is an odd value (the writer is writing something), the reader will wait for the 
writer to finish (the sequence number becomes an even number). 
- The value of the sequence number while entering into the critical section is called an 
old sequence number.

- After reading the data, again it will check the sequence number. 
- If it is equal to the old sequence number, then everything is okay.
- Otherwise, it will repeat step 2 again. 
- In this case, readers simply retry (using a loop) until they read the same even 
sequence number before and after. 
- The reader never blocks, but it may have to retry if a write is in progress.
-------------------------------------------------------------------------------------------
- When only the reader is reading the data and no writer is in the critical section, 
any time one writer can enter into a critical section by taking lock without blocking. 

- This means the writer cannot be blocked for the reader and the reader has to re-read the 
data when the writer is writing. 
- This means seqlock is giving importance to a writer, not the reader (the reader may have 
to wait but not the writer).
-------------------------------------------------------------------------------------------
When we have to use seqlock

- We cannot use this seqlock in any situations like normal spinlock or mutex. 
- Because this will not be effective in such situations other than the situations 
mentioned below.

1. where read operations are more frequent than write.
2. where write access is rare but must be fast.
3. That data is simple (no pointers) that needs to be protected. 
Seqlocks generally cannot be used to protect data structures involving pointers, because 
the reader may be following a pointer that is invalid while the writer is changing the 
data structure.
-------------------------------------------------------------------------------------------
Seqlock in Linux Kernel – API

Init Seqlock
- This API is used to initialize the seqlock.

seqlock_init(seqlock_t *lock);
Example
#include <linux/seqlock.h>

seqlock_t lock;

seqlock_init(&lock);
-------------------------------------------------------------------------------------------
Write operation

- Before writing to the protected data, the writers must take exclusive access to enter 
the critical section. This write lock is implemented by using spinlock. 
Let’s see the API used for that.

Write Lock
write_seqlock

void write_seqlock(seqlock_t *lock);

- When you call this API, it locks the spinlock and increments the sequence number. 
- Now you can access the protected data. Once you are done with that, you can release the 
lock using the below API.
-------------------------------------------------------------------------------------------
write_tryseqlock

int write_tryseqlock(seqlock_t *lock);

- This API won’t wait for the lock. It will return non-zero if it took the lock. 
- Otherwise, it will return 0. That means some other writer is accessing the data.
-------------------------------------------------------------------------------------------
write_seqlock_irqsave

void write_seqlock_irqsave(seqlock_t *lock, long flags);

- This will save whether interrupts were ON or OFF in a flags word and grab the lock. 
- This API is used in an interrupt context.
-------------------------------------------------------------------------------------------
write_seqlock_irq

void write_seqlock_irq(seqlock_t *lock);

- This will disable interrupts on that CPU, and take the lock while writing. 
This API is used in an interrupt context.
-------------------------------------------------------------------------------------------
write_seqlock_bh

void write_seqlock_bh(seqlock_t *lock);

- This is similar to write_seqlock, but when you try to write from the bottom halves 
you can use this call.
-------------------------------------------------------------------------------------------
Write unlock

write_sequnlock
void write_sequnlock(seqlock_t *lock);

- This API will increments the sequence number again and release the spinlock.
-------------------------------------------------------------------------------------------
write_sequnlock_irqrestore

void write_sequnlock_irqrestore(seqlock_t *lock, long flags);

- This will release the lock and restores the interrupts using the flags argument. 
This API is used in an interrupt context.
-------------------------------------------------------------------------------------------
write_sequnlock_irq

void write_sequnlock_irq(seqlock_t *lock);

- This will release the lock and re-enable interrupts on that CPU, which is disabled by 
write_seqlock_irq call. This API is used in an interrupt context.
-------------------------------------------------------------------------------------------
write_sequnlock_bh

void write_sequnlock_bh(seqlock_t *lock);

- This will be used from the bottom halves while reading.
-------------------------------------------------------------------------------------------
Example write operation

- This example is for locking between user contexts. 
Use other variants based on the context(bottom half or IRQ).

write_seqlock(&lock);
/* Write data */
write_sequnlock(&lock);
-------------------------------------------------------------------------------------------
Read operation

- There is no locking needed for reading the protected data. 
- But we have to implement the below steps in our code.

1. Begin the read and get the initial sequence number.

2. Read the data.

3. Once the reading is done, compare the current sequence number with an initial sequence 
number. 
- If the current sequence number is an odd value or the current sequence number is not 
matching with the initial sequence number means writing is going on. 
- So the reader has to retry, which means the reader has to again go to step 1 and do 
the process again.
-------------------------------------------------------------------------------------------
Let’s see the APIs used for reading.

read_seqbegin
unsigned int read_seqbegin(seqlock_t *lock);

- This API will begin the read and return the sequence number. 
- This API is used for the above step 1.
-------------------------------------------------------------------------------------------
read_seqbegin_irqsave
unsigned int read_seqbegin_irqsave(seqlock_t *lock, long flags);

- This will save whether interrupts were ON or OFF in a flags word and return the 
sequence number.
-------------------------------------------------------------------------------------------
read_seqretry
int read_seqretry(seqlock_t *lock, unsigned int seq_no);

- This API will compare the current sequence number with the provided sequence number 
(argument 2). 
- If the current sequence number is an odd value or the current sequence number is not 
matching with the initial sequence number (argument 2) means writing is going on. 
- So it will return 1. Otherwise, it will return 0.
-------------------------------------------------------------------------------------------
read_seqretry_irqrestore
int read_seqretry_irqrestore(seqlock_t *lock, unsigned int seq_no, long flags);

- This will restore the interrupt using flags, and work like read_seqretry.
-------------------------------------------------------------------------------------------
Example read operation

- This example is for the user context. Use other variants based on the 
context(bottom half or IRQ). 

- An example reading snippet is given below.

unsigned int seq_no;

do {
    seq_no = read_seqbegin(&lock);
    /* Read the data */
} while ( read_seqretry(&lock, seq_no) );

##########################################################################################
Example Programming

- This code snippet explains how to create two threads that access a global variable 
(etx_gloabl_variable). 
- Thread 1 is for writing and Thread 2 is for reading. 
- Before writing to the variable, the writer should take the seqlock. 
- After that, it will release the seqlock.  
- The reader will check the sequence number. If it is not a valid sequence number, 
then again the reader will retry.

##########################################################################################
/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple Linux device driver (Seqlock)
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
#include <linux/seqlock.h>
#include <linux/err.h>
//Seqlock variable
seqlock_t etx_seq_lock;
 
unsigned long etx_global_variable = 0;

static struct task_struct *etx_thread1;
static struct task_struct *etx_thread2; 
 
int thread_function1(void *pv);
int thread_function2(void *pv);
 
//Thread used for writing
int thread_function1(void *pv)
{
    while(!kthread_should_stop()) {  
        write_seqlock(&etx_seq_lock);
        etx_global_variable++;
        write_sequnlock(&etx_seq_lock);
        msleep(1000);
    }
    return 0;
}
 
//Thread used for reading
int thread_function2(void *pv)
{
    unsigned int seq_no;
    unsigned long read_value;
    while(!kthread_should_stop()) {
        do {
            seq_no = read_seqbegin(&etx_seq_lock);
        read_value = etx_global_variable;
    } while (read_seqretry(&etx_seq_lock, seq_no));
        pr_info("In EmbeTronicX Thread Function2 : Read value %lu\n", read_value);
        msleep(1000);
    }
    return 0;
}

/*
** Module Init function
*/ 
static int __init etx_driver_init(void)
{       
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
 
        //Initialize the seqlock
        seqlock_init(&etx_seq_lock);
        
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


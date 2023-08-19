##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################
Introduction

- Before looking into atomic variables, we will see one example.

- I have one integer or long variable named "etx_global_variable" which is shared between 
two threads. 
- Two threads are just incrementing the variable like below.

Thread 1:
etx_global_variable++; //Accessing the variable
Thread 2:
etx_global_variable++; //Accessing the variable

- Now we will see how it is incrementing internally in each instruction when both 
threads are running concurrently. 
- Assume the initial value of etx_global_variable is 0.

- Now the value of etx_global_variable is 1 after the two threads are processed. 
- Are we expecting the same value which is 1"?" Nope. 
- We are expecting the value of etx_global_variable should be 2. 
- It is not running as expected because the global variable is shared between two 
concurrent threads. 
- So we need to implement synchronization because both the threads are accessing 
(writing/reading) the variable. 
- We can implement synchronization like below using locks.

Thread 1:
lock(); //spinlock or mutex 
etx_global_variable++; //Accessing the variable 
unlock();

Thread 2:
lock(); //spinlock or mutex 
etx_global_variable++; //Accessing the variable 
unlock();

- After this synchronization, we will see how it is incrementing internally when both 
threads are running concurrently. Assume the initial value of etx_global_variable is 0.

- Great. That’s all. Now we are getting 2 in the two methods mentioned above. 
- But has anyone thought anytime that, why these things are required for a single variable 

- Why don’t we have an alternate method for a single variable"?" Yes, obviously we have an 
alternate mechanism for integer and long variables. 

- That is the "atomic operation". If you use mutex/spinlock for just a single variable, 
it will add overhead. 
- In this tutorial, we gonna see the atomic variable, atomic operation, and its usage.

##########################################################################################

atomic variables in Linux

- The read, write and arithmetic operations on the atomic variables will be done in one 
instruction without interruption.

- So, again we will take the same example mentioned above to explain the atomic variable 
operations. When we use the atomic method, that will work like the below.

- So the extra locking mechanism is not required when we are using atomic variables since 
the operation is happening in one machine instruction.

- An "atomic_t" holds an int value and atomic64_t holds the long value on all 
supported architectures.

- In Linux Kernel Version 2.6, the atomic variable has defined below.

typedef struct 
{
	volatile int counter;
} atomic_t;

#ifdef CONFIG_64BIT
typedef struct 
{
	volatile long counter;
} atomic64_t;
#endif

- Then later, they have removed volatile and defined as below.

typedef struct 
{
	int counter;
} atomic_t;

#ifdef CONFIG_64BIT
typedef struct 
{
	long counter;
} atomic64_t;
#endif

- You can read here why they have removed volatile.

##########################################################################################

Why the "volatile" type class should not be used
------------------------------------------------

C programmers have often taken volatile to mean that the variable could be
changed outside of the current thread of execution; as a result, they are
sometimes tempted to use it in kernel code when shared data structures are
being used.  In other words, they have been known to treat volatile types
as a sort of easy atomic variable, which they are not.  The use of volatile in
kernel code is almost never correct; this document describes why.

The key point to understand with regard to volatile is that its purpose is
to suppress optimization, which is almost never what one really wants to
do.  In the kernel, one must protect shared data structures against
unwanted concurrent access, which is very much a different task.  The
process of protecting against unwanted concurrency will also avoid almost
all optimization-related problems in a more efficient way.

Like volatile, the kernel primitives which make concurrent access to data
safe (spinlocks, mutexes, memory barriers, etc.) are designed to prevent
unwanted optimization.  If they are being used properly, there will be no
need to use volatile as well.  If volatile is still necessary, there is
almost certainly a bug in the code somewhere.  In properly-written kernel
code, volatile can only serve to slow things down.

Consider a typical block of kernel code:

    spin_lock(&the_lock);
    do_something_on(&shared_data);
    do_something_else_with(&shared_data);
    spin_unlock(&the_lock);

If all the code follows the locking rules, the value of shared_data cannot
change unexpectedly while the_lock is held.  Any other code which might
want to play with that data will be waiting on the lock.  The spinlock
primitives act as memory barriers - they are explicitly written to do so -
meaning that data accesses will not be optimized across them.  So the
compiler might think it knows what will be in shared_data, but the
spin_lock() call, since it acts as a memory barrier, will force it to
forget anything it knows.  There will be no optimization problems with
accesses to that data.

If shared_data were declared volatile, the locking would still be
necessary.  But the compiler would also be prevented from optimizing access
to shared_data _within_ the critical section, when we know that nobody else
can be working with it.  While the lock is held, shared_data is not
volatile.  When dealing with shared data, proper locking makes volatile
unnecessary - and potentially harmful.

The volatile storage class was originally meant for memory-mapped I/O
registers.  Within the kernel, register accesses, too, should be protected
by locks, but one also does not want the compiler "optimizing" register
accesses within a critical section.  But, within the kernel, I/O memory
accesses are always done through accessor functions; accessing I/O memory
directly through pointers is frowned upon and does not work on all
architectures.  Those accessors are written to prevent unwanted
optimization, so, once again, volatile is unnecessary.

Another situation where one might be tempted to use volatile is
when the processor is busy-waiting on the value of a variable.  The right
way to perform a busy wait is:

    while (my_variable != what_i_want)
        cpu_relax();

The cpu_relax() call can lower CPU power consumption or yield to a
hyperthreaded twin processor; it also happens to serve as a memory barrier,
so, once again, volatile is unnecessary.  Of course, busy-waiting is
generally an anti-social act to begin with.

There are still a few rare situations where volatile makes sense in the
kernel:

  - The above-mentioned accessor functions might use volatile on
    architectures where direct I/O memory access does work.  Essentially,
    each accessor call becomes a little critical section on its own and
    ensures that the access happens as expected by the programmer.

  - Inline assembly code which changes memory, but which has no other
    visible side effects, risks being deleted by GCC.  Adding the volatile
    keyword to asm statements will prevent this removal.

  - The jiffies variable is special in that it can have a different value
    every time it is referenced, but it can be read without any special
    locking.  So jiffies can be volatile, but the addition of other
    variables of this type is strongly frowned upon.  Jiffies is considered
    to be a "stupid legacy" issue (Linus's words) in this regard; fixing it
    would be more trouble than it is worth.

  - Pointers to data structures in coherent memory which might be modified
    by I/O devices can, sometimes, legitimately be volatile.  A ring buffer
    used by a network adapter, where that adapter changes pointers to
    indicate which descriptors have been processed, is an example of this
    type of situation.

For most code, none of the above justifications for volatile apply.  As a
result, the use of volatile is likely to be seen as a bug and will bring
additional scrutiny to the code.  Developers who are tempted to use
volatile should take a step back and think about what they are truly trying
to accomplish.

##########################################################################################

Types of atomic variables

- Two different atomic variables are there.

Atomic variables that operate on Integers
Atomic variables that operate on Individual Bits
------------------------------------------------------------------------------------------
Atomic Integer Operations

- When we are doing atomic operations, that variable should be created using atomic_t or 
atomic64_t. 
- So we have separate special functions for reading, writing, and arithmetic operations, 
and those are explained below.

- The declarations are needed to use the atomic integer operations are in <asm/atomic.h>. 
- Some architectures provide additional methods that are unique to that architecture, 
but all architectures provide at least a minimum set of operations that are used 
throughout the kernel. 
- When you write kernel code, you can ensure that these operations are correctly 
implemented on all architectures.
------------------------------------------------------------------------------------------
Creating atomic variables

atomic_t etx_global_variable; /* define etx_global_variable */

or

/* define etx_global_variable,initialize it to zero */
atomic_t etx_global_variable = ATOMIC_INIT(0); 
------------------------------------------------------------------------------------------
Reading atomic variables

atomic_read
- This function atomically reads the value of the given atomic variable.

int atomic_read(atomic_t *v);

v:pointer of type atomic_t
Return: It returns the integer value.
------------------------------------------------------------------------------------------
Other operations on atomic variables

atomic_set()
- This function atomically sets the value to the atomic variable.

void atomic_set(atomic_t *v, int i);

v: the pointer of type atomic_t
i: the value to be set to v
------------------------------------------------------------------------------------------
atomic_add
- This function atomically adds value to the atomic variable.

void atomic_add(int i, atomic_t *v);

i: the value to be added to v
v: the pointer of type atomic_t
------------------------------------------------------------------------------------------
atomic_sub
This function atomically subtracts the value from the atomic variable.

void atomic_sub(int i, atomic_t *v);

where,
i – the value to be subtracted from v
v – the pointer of type atomic_t
------------------------------------------------------------------------------------------
atomic_inc
This function atomically increments the value of the atomic variable by 1.

void atomic_inc (atomic_t *v);

where,
v – the pointer of type atomic_t
------------------------------------------------------------------------------------------
atomic_dec
This function atomically decrements the value of the atomic variable by 1.

void atomic_dec (atomic_t *v);

where,
v – the pointer of type atomic_t
------------------------------------------------------------------------------------------
atomic_sub_and_test
- This function atomically subtracts the value from the atomic variable and test the 
result is zero or not.

void atomic_sub_and_test(int i, atomic_t *v);

where,
i – the value to be subtracted from v
v – the pointer of type atomic_t

Return: It returns true if the result is zero, or false for all other cases.
------------------------------------------------------------------------------------------
atomic_dec_and_test
- This function atomically decrements the value of the atomic variable by 1 and test the 
result is zero or not.

void atomic_dec_and_test(atomic_t *v);

where,
v – the pointer of type atomic_t

Return: It returns true if the result is zero, or false for all other cases.
------------------------------------------------------------------------------------------
atomic_inc_and_test
- This function atomically increments the value of the atomic variable by 1 and test the 
result is zero or not.

void atomic_inc_and_test(atomic_t *v);

where,
v – the pointer of type atomic_t

Return: It returns true if the result is zero, or false for all other cases.
------------------------------------------------------------------------------------------
atomic_add_negative
- This function atomically adds the value to the atomic variable and test the result is 
negative or not.

void atomic_add_negative(int i, atomic_t *v);

where,
i – the value to be added to v
v – the pointer of type atomic_t

Return: It returns true if the result is negative, or false for all other cases.
------------------------------------------------------------------------------------------
atomic_add_return
- This function atomically adds the value to the atomic variable and returns the value.

void atomic_add_return(int i, atomic_t *v);

where,
i – the value to be added to v
v – the pointer of type atomic_t

Return : It returns true if the result the value (i + v).
------------------------------------------------------------------------------------------
Like this, other functions are also there. Those are,

int atomic_sub_return(int i, atomic_t *v)
- Atomically subtract i from v and return the result
------------------------------------------------------------------------------------------
int atomic_inc_return(int i, atomic_t *v)	
- Atomically increments v by one and return the result
------------------------------------------------------------------------------------------
int atomic_dec_return(int i, atomic_t *v)	
- Atomically decrements v by one and return the result

##########################################################################################
atomic_add_unless

- This function atomically adds value to the atomic variable unless the number is a given 
value.

atomic_add_unless (atomic_t *v, int a, int u);

where,
v – the pointer of type atomic_t
a – the amount to add to v…
u – …unless v is equal to u.

Return: It returns non-zero if v was not u, and zero otherwise.

- There is a 64-bit version also available. Unlike atomic_t, that will operate on 64 bits. 
This 64-bit version also has a similar function like above, the only change is we have 
to use 64.

Example
atomic64_t etx_global_variable = ATOMIC64_INIT(0);
long atomic64_read(atomic64_t *v);
void atomic64_set(atomic64_t *v, int i);
void atomic64_add(int i, atomic64_t *v);
void atomic64_sub(int i, atomic64_t *v);
void atomic64_inc(atomic64_t *v);
void atomic64_dec(atomic64_t *v);
int atomic64_sub_and_test(int i, atomic64_t *v);
int atomic64_add_negative(int i, atomic64_t *v);
long atomic64_add_return(int i, atomic64_t *v);
long atomic64_sub_return(int i, atomic64_t *v);
long atomic64_inc_return(int i, atomic64_t *v);
long atomic64_dec_return(int i, atomic64_t *v);
int atomic64_dec_and_test(atomic64_t *v);
int atomic64_inc_and_test(atomic64_t *v);
But all the operations are the same as atomic_t.
------------------------------------------------------------------------------------------

Atomic Bitwise Operations

- Atomic_t is good when we are working on integer arithmetic. But when it comes to 
bitwise atomic operation, it doesn’t work well. 
- So kernel offers separate functions to achieve that. Atomic bit operations are very fast
- These functions are architecture-dependent and are declared in <asm/bitops.h>.

- These bitwise functions operate on a generic pointer. 
- So, atomic_t / atomic64_t is not required. 
- So we can work with a pointer to whatever data we want.
------------------------------------------------------------------------------------------
The below functions are available for atomic bit operations.

void set_bit(int nr, void *addr)	
- Atomically set the nr-th bit starting from addr
------------------------------------------------------------------------------------------
void clear_bit(int nr, void *addr)
- Atomically clear the nr-th bit starting from addr
------------------------------------------------------------------------------------------
void change_bit(int nr, void *addr)
- Atomically flip the value of the nr-th bit starting from addr
------------------------------------------------------------------------------------------
int test_and_set_bit(int nr, void *addr)
- Atomically set the nr-th bit starting from addr and return the previous value
------------------------------------------------------------------------------------------
int test_and_clear_bit(int nr, void *addr)
- Atomically clear the nr-th bit starting from addr and return the previous value
------------------------------------------------------------------------------------------
int test_and_change_bit(int nr, void *addr)
- Atomically flip the nr-th bit starting from addr and return the previous value
------------------------------------------------------------------------------------------
int test_bit(int nr, void *addr)
- Atomically return the value of the nr-th bit starting from addr
------------------------------------------------------------------------------------------
int find_first_zero_bit(unsigned long *addr, unsigned int size)
- Atomically returns the bit-number of the first zero bit, not the number of the byte 
containing a bit
------------------------------------------------------------------------------------------
int find_first_bit(unsigned long *addr, unsigned int size)
- Atomically returns the bit-number of the first set bit, not the number of the byte 
containing a bit

##########################################################################################

- And also non-atomic bit operations also available. What is the use of that when we have 
atomic bit operations "?" 

- When we have code that is already locked by mutex/spinlock then 
we can go for this non-atomic version. 
>>>>> Needed when - we already have locks in our code and we want faster bit operations

- This might be faster in that case. The below functions are available for non-atomic 
bit operations.
------------------------------------------------------------------------------------------
void _set_bit(int nr, void *addr)
- Non-atomically set the nr-th bit starting from addr
------------------------------------------------------------------------------------------
void _clear_bit(int nr, void *addr)
- Non-atomically clear the nr-th bit starting from addr
------------------------------------------------------------------------------------------
void _change_bit(int nr, void *addr)	
- Non-atomically flip the value of the nr-th bit starting from addr
------------------------------------------------------------------------------------------
int _test_and_set_bit(int nr, void *addr)	
- Non-atomically set the nr-th bit starting from addr and return the previous value
------------------------------------------------------------------------------------------
int _test_and_clear_bit(int nr, void *addr)	
- Non-atomically clear the nr-th bit starting from addr and return the previous value
------------------------------------------------------------------------------------------
int _test_and_change_bit(int nr, void *addr)
- Non-atomically flip the nr-th bit starting from addr and return the previous value
------------------------------------------------------------------------------------------
int _test_bit(int nr, void *addr)
- Non-atomically return the value of the nr-th bit starting from addr
##########################################################################################
/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple Linux device driver (Atomic Variables)
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
#include <linux/err.h>
atomic_t etx_global_variable = ATOMIC_INIT(0);      //Atomic integer variable
unsigned int etc_bit_check = 0;
  
static struct task_struct *etx_thread1;
static struct task_struct *etx_thread2; 
 
int thread_function1(void *pv);
int thread_function2(void *pv);

/*
** kernel thread function 2
*/
int thread_function1(void *pv)
{
    unsigned int prev_value = 0;
    
    while(!kthread_should_stop()) {
        atomic_inc(&etx_global_variable);
        prev_value = test_and_change_bit(1, (void*)&etc_bit_check);
        pr_info("Function1 [value : %u] [bit:%u]\n", atomic_read(&etx_global_variable), prev_value);
        msleep(1000);
    }
    return 0;
}
 
/*
** kernel thread function 2
*/
int thread_function2(void *pv)
{
    unsigned int prev_value = 0;
    while(!kthread_should_stop()) {
        atomic_inc(&etx_global_variable);
        prev_value = test_and_change_bit(1,(void*) &etc_bit_check);
        pr_info("Function2 [value : %u] [bit:%u]\n", atomic_read(&etx_global_variable), prev_value);
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


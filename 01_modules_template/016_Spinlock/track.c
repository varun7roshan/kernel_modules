##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################


##########################################################################################
Introduction

- In our previous tutorial, we have understood the use of Mutex and its Implementation. 
- If you have understood Mutex then Spinlock is also similar. 
- Both are used to protect a shared resource from being modified by two or more processes 
simultaneously.

SpinLock
- In the Mutex concept, when the thread is trying to lock or acquire the Mutex which is 
not available then that thread will go to sleep until that Mutex is available. 
- Whereas in Spinlock it is different. The spinlock is a very simple single-holder lock. 
- If a process attempts to acquire a spinlock and it is unavailable, the process will keep 
trying (spinning) until it can acquire the lock. 
- This simplicity creates a small and fast lock.

------------------------------------------------------------------------------------------
- Like Mutex, there are two possible states in Spinlock: Locked or Unlocked.

SpinLock in Linux Kernel Device Driver
- If the kernel is running on a uniprocessor and CONFIG_SMP, CONFIG_PREEMPT aren’t enabled 
while compiling the kernel then spinlock will not be available. 
- Because there is no reason to have a lock when no one else can run at the same time.

- "But if you have disabled CONFIG_SMP and enabled  CONFIG_PREEMPT then spinlock will"
"simply disable preemption, which is sufficient to prevent any races"
------------------------------------------------------------------------------------------
Initialize
- We can initialize Spinlock in Linux kernel in two ways.

Static Method
Dynamic Method
------------------------------------------------------------------------------------------
Static Method

- You can statically initialize a Spinlock using the macro given below.

DEFINE_SPINLOCK(etx_spinlock);

- The macro given above will create a spinlock_t variable in the name of etx_spinlock and 
initialize to UNLOCKED STATE. 
- Take a look at the expansion of DEFINE_SPINLOCK below.
#define DEFINE_SPINLOCK(x)      spinlock_t x = __SPIN_LOCK_UNLOCKED(x)
------------------------------------------------------------------------------------------
Dynamic Method

- If you want to initialize dynamically you can use the method as given below.

spinlock_t etx_spinlock;
spin_lock_init(&etx_spinlock);

You can use any one of the methods.
------------------------------------------------------------------------------------------

- After initializing the spinlock, there are several ways to use spinlock to lock or 
unlock, based on where the spinlock is used; either in user context or interrupt context. 
------------------------------------------------------------------------------------------
Approach 1 (Locking between User context)

- If you share data with user context (between Kernel Threads), then you can use this 
approach.

Lock:
spin_lock(spinlock_t *lock)

- This will take the lock if it is free, otherwise, it’ll spin until that lock is free 
(Keep trying).

Try_Lock:
spin_trylock(spinlock_t *lock)

- Locks the spinlock if it is not already locked. 
- If unable to obtain the lock it exits with an error and does not spin. 
- It returns non-zero if it obtains the lock otherwise returns zero.

Unlock:
spin_unlock(spinlock_t *lock)

- It does the reverse of the lock. It will unlock which is locked by the above call.

Checking_Lock:
spin_is_locked(spinlock_t *lock)

- This is used to check whether the lock is available or not. 
- It returns non-zero if the lock is currently acquired. otherwise returns zero.

Example
//Thread 1
int thread_function1(void *pv)
{
    while(!kthread_should_stop()) {
        spin_lock(&etx_spinlock);
        etx_global_variable++;
        printk(KERN_INFO "In EmbeTronicX Thread Function1 %lu\n", etx_global_variable);
        spin_unlock(&etx_spinlock);
        msleep(1000);
    }
    return 0;
}

//Thread 2
int thread_function2(void *pv)
{   
    while(!kthread_should_stop()) {
        spin_lock(&etx_spinlock);
        etx_global_variable++;
        printk(KERN_INFO "In EmbeTronicX Thread Function2 %lu\n", etx_global_variable);
        spin_unlock(&etx_spinlock);
        msleep(1000);
    }
    return 0;
}
------------------------------------------------------------------------------------------
Approach 2 (Locking between Bottom Halves)

- If you want to share data between two different Bottom halves or the same bottom halves, 
then you can use Approach 1.
------------------------------------------------------------------------------------------
Approach 3 (Locking between User context and Bottom Halves)

- If you share data with a bottom half and user context (like Kernel Thread), then this 
approach will be useful.

Lock:
spin_lock_bh(spinlock_t *lock)

- It disables soft interrupts on that CPU, then grabs the lock. 
- This has the effect of preventing softirqs, tasklets, and bottom halves from running on 
the local CPU. 
- Here the suffix ‘_bh‘ refers to “Bottom Halves“.

Unlock:
spin_unlock_bh(spinlock_t *lock)

- It will release the lock and re-enables the soft interrupts which are disabled by the 
above call.

Example
//Thread
int thread_function(void *pv)
{
    while(!kthread_should_stop()) {
        spin_lock_bh(&etx_spinlock);
        etx_global_variable++;
        printk(KERN_INFO "In EmbeTronicX Thread Function %lu\n", etx_global_variable);
        spin_unlock_bh(&etx_spinlock);
        msleep(1000);
    }
    return 0;
}
/*Tasklet Function*/
void tasklet_fn(unsigned long arg)
{
        spin_lock_bh(&etx_spinlock);
        etx_global_variable++;
        printk(KERN_INFO "Executing Tasklet Function : %lu\n", etx_global_variable);
        spin_unlock_bh(&etx_spinlock);
}
------------------------------------------------------------------------------------------
Approach 4 (Locking between Hard IRQ and Bottom Halves)

- If you share data between Hardware ISR and Bottom halves then you have to disable the 
IRQ before locking. 
- Because the bottom halves processing can be interrupted by a hardware interrupt. 
- So this will be used in that scenario.

Lock:
spin_lock_irq(spinlock_t *lock)

This will disable interrupts on that CPU, then grab the lock.

Unlock:
spin_unlock_irq(spinlock_t *lock)

- It will release the lock and re-enables the interrupts which are disabled by the 
above call.

Example
/*Tasklet Function*/
void tasklet_fn(unsigned long arg)
{
        spin_lock_irq(&etx_spinlock);
        etx_global_variable++;
        printk(KERN_INFO "Executing Tasklet Function : %lu\n", etx_global_variable);
        spin_unlock_irq(&etx_spinlock);
}

//Interrupt handler for IRQ 11. 
static irqreturn_t irq_handler(int irq,void *dev_id) {
        spin_lock_irq(&etx_spinlock); 
        etx_global_variable++;
        printk(KERN_INFO "Executing ISR Function : %lu\n", etx_global_variable);
        spin_unlock_irq(&etx_spinlock);
        /*Scheduling Task to Tasklet*/
        tasklet_schedule(tasklet); 
        return IRQ_HANDLED;
}
------------------------------------------------------------------------------------------
Approach 5 (Alternative way of Approach 4)

- If you want to use a different variant rather than using spin_lock_irq() and 
spin_unlock_irq() then you can use this approach.

Lock:
spin_lock_irqsave( spinlock_t *lock, unsigned long flags );

This will save whether interrupts were on or off in a flags word and grab the lock.

Unlock:
spin_unlock_irqrestore( spinlock_t *lock, unsigned long flags );

- This will release the spinlock and restores the interrupts using the flags argument.
------------------------------------------------------------------------------------------
Approach 6 (Locking between Hard IRQs)

- If you want to share data between two different IRQs, then you should use Approach 5.
------------------------------------------------------------------------------------------

Spinlock in Linux kernel example programming

- This code snippet explains how to create two threads that access a global variable 
(etx_gloabl_variable). 
- So before accessing the variable, it should lock the spinlock. 
- After that, it will release the spinlock. This example is using Approach 1.

##########################################################################################
										Read_Write_Spinlock:

- In our previous tutorial, we have understood the use of Common Spinlock and its 
Implementation. 
- If you have understood Spinlock then this "Read Write Spinlock" is also similar except 
some differences. We will cover those things below.

SpinLock
- The spinlock is a very simple single-holder lock. If a process attempts to acquire a 
spinlock and it is unavailable, the process will keep trying (spinning) until it can 
acquire the lock. 
- Read Write spinlock also does the same but it has separate locks for the read and the 
write operation.
------------------------------------------------------------------------------------------
Read Write Spinlocks

- I told that spinlock and read-write spinlock are similar in operation. 
- That means spinlock is enough. Then why do we need a read-write Spinlock "?"

- Okay, now we will take one scenario. I have five threads. 
- All those threads are accessing one global variable. So we can use spinlock over there. 
- Am I right "?" Well, yes it’s right.  In those threads, thread-1’s role is to write the 
data into that variable. 
- The other four thread’s roles are simply reading the data from that variable. 
- This the case. I hope you guys understand the scenario. Now I can ask you guys a 
question. If you implement spinlock,

Question: Now we forgot the thread-1(writer thread). So we have 4 reader threads 
(thread-2 to thread-5). Now those all four threads are want to read the data from the 
variable at the same time. What will happen in this situation if you use spinlock "?" 
What about the processing speed and performance "?"

- Let’s assume thread-2 got the lock while other reading threads are trying hard for the 
lock. 
- That variable won’t change even thread-2’s access. Because no one is writing. 
- But here only thread-2 is accessing. But other reading threads are simply wasting time to
take lock since the variable won’t change. 
- That means performance will be reduced. Isn’t it "?" Yes you are correct.

- In this case, if you implement read and write lock, thread-2 will take the read lock and 
read the data. 
- And other reading threads also will take the read lock without spinning (blocking) and 
read the data. Because no one is writing. So what about the performance now "?" 
- There is no waiting between reading operations. Right "?" Then, in this case, a 
read-write spinlock is useful.

- So, If multiple threads require read access to the same data, there is no reason why 
they should not be able to execute simultaneously. Spinlocks don’t differentiate between 
read and read/write access. Thus spinlocks do not exploit this potential parallelism. 
To do so, read-write locks are required.
------------------------------------------------------------------------------------------
Working of Read Write Spinlock

- When there is no thread in the critical section, any reader or writer thread can enter 
into a critical section by taking respective read or write lock. 
- But only one thread can enter into a critical section.

- If the reader thread is in the critical section, the new reader thread can enter 
arbitrarily, but the writer thread cannot enter. 
- The writer thread has to wait until all the reader thread finishes their process.
- If the writer thread is in the critical section, no reader thread or writer thread 
can enter.

- If one or more reader threads are in the critical section by taking its lock, the writer 
thread can of course not enter the critical section, but the writer thread cannot prevent 
the entry of the subsequent read thread. 
- He has to wait until the critical section has a reader thread. So this read-write 
spinlock is giving importance to the reader thread and not the writer thread. 
------------------------------------------------------------------------------------------
SEQLOCK: Gives importance to WRITERs
- "If you want to give importance to the writer thread than the reader thread, "
"then another lock is available in Linux which is seqlock."
------------------------------------------------------------------------------------------
Where to use Read Write Spinlock "?"

- If you are only reading the data then you take read lock
- If you are writing then go for a write lock

Note: Many people can hold a read lock, but a writer must be the sole holder. 
- Read-Write locks are more useful in scenarios where the architecture is clearly divided 
into the reader and the writers, "with more number of reads" 
>> With MORE NUMBER of READERS ------------ THIS is IMPORTANT
>> We "read" more often than we "write"
------------------------------------------------------------------------------------------

- In Read Write spinlock multiple readers are permitted at the same time but only one 
writer. (i.e) If a writer has the lock, no reader is allowed to enter the critical section 

- If only a reader has the lock, then multiple readers are permitted in the 
critical section.
------------------------------------------------------------------------------------------
Read-Write SpinLock in Linux Kernel Device Driver

Initialize
- We can initialize Read Write Spinlock in two ways.

Static Method
Dynamic Method
------------------------------------------------------------------------------------------
Static Method

- You can statically initialize a Read Write Spinlock using the macro given below.

DEFINE_RWLOCK(etx_rwlock);

The macro given above will create rwlock_t variable in the name of etx_rwlock.
------------------------------------------------------------------------------------------
Dynamic Method

- If you want to initialize dynamically you can use the method given below.

rwlock_t etx_rwlock; 
rwlock_init(&etx_rwlock);
You can use any one of the methods.
------------------------------------------------------------------------------------------
- After initializing the read-write spinlock, there are several ways to use 
read-write spinlock to lock or unlock, based on where the read/write spinlock is used; 
either in user context or interrupt context. 
Let’s look at the approaches to these situations.
------------------------------------------------------------------------------------------
Approach 1 (Locking between User context)

- If you share data with user context (between Kernel Threads), then you can use this 
approach.

Read Lock
Lock:
read_lock(rwlock_t *lock)
- This will take the lock if it is free, otherwise, it’ll spin until that lock is free 
(Keep trying).

Unlock:
read_unlock(rwlock_t *lock)
- It does the reverse of the lock. It will unlock which is locked by the above call.

Write Lock
Lock:
write_lock(rwlock_t *lock)
- This will take the lock if it is free, otherwise, it’ll spin until that lock is free 
(Keep trying).

Unlock:
write_unlock(rwlock_t *lock)
- It does the reverse of the lock. It will unlock which is locked by the above call.

Example
//Thread 1
int thread_function1(void *pv)
{
    while(!kthread_should_stop()) {  
        write_lock(&etx_rwlock);
        etx_global_variable++;
        write_unlock(&etx_rwlock);
        msleep(1000);
    }
    return 0;
}

//Thread 2
int thread_function2(void *pv)
{
    while(!kthread_should_stop()) {
        read_lock(&etx_rwlock);
        printk(KERN_INFO "In EmbeTronicX Thread Function2 : Read value %lu\n", etx_global_variable);
        read_unlock(&etx_rwlock);
        msleep(1000);
    }
    return 0;
}
------------------------------------------------------------------------------------------
Approach 2 (Locking between Bottom Halves)

- If you want to share data between two different Bottom halves or the same bottom halves, 
then you can use Approach 1.
------------------------------------------------------------------------------------------
Approach 3 (Locking between User context and Bottom Halves)

- If you share data with a bottom half and user context (like Kernel Thread), then this 
approach will be useful.

Read Lock
Lock:
read_lock_bh(rwlock_t *lock)

- It disables soft interrupts on that CPU, then grabs the lock. This has the effect of 
preventing softirqs, tasklets, and bottom halves from running on the local CPU. 
- Here the suffix ‘_bh‘ refers to “Bottom Halves“.

Unlock:
read_unlock_bh(rwlock_t *lock)

- It will release the lock and re-enables the soft interrupts which are disabled by the 
above call.

Write Lock
Lock:
write_lock_bh(rwlock_t *lock)

- It disables soft interrupts on that CPU, then grabs the lock. This has the effect of 
preventing softirqs, tasklets, and bottom halves from running on the local CPU. 
- Here the suffix ‘_bh‘ refers to “Bottom Halves“.

Unlock:
write_unlock_bh(rwlock_t *lock)

- It will release the lock and re-enables the soft interrupts which are disabled by the 
above call.

Example
//Thread
int thread_function(void *pv)
{
    while(!kthread_should_stop()) {  
        write_lock_bh(&etx_rwlock);
        etx_global_variable++;
        write_unlock_bh(&etx_rwlock);
        msleep(1000);
    }
    return 0;
}
/*Tasklet Function*/
void tasklet_fn(unsigned long arg)
{
        read_lock_bh(&etx_rwlock);
        printk(KERN_INFO "Executing Tasklet Function : %lu\n", etx_global_variable);
        read_unlock_bh(&etx_rwlock);
}
------------------------------------------------------------------------------------------
Approach 4 (Locking between Hard IRQ and Bottom Halves)

- If you share data between Hardware ISR and Bottom halves then you have to disable the 
IRQ before lock. 
- Because the bottom halves processing can be interrupted by a hardware interrupt. 
- So this will be used in that scenario.

Read Lock
Lock:
read_lock_irq(rwlock_t *lock)
- This will disable interrupts on that CPU, then grab the lock.

Unlock:
read_unlock_irq(rwlock_t *lock)
- It will release the lock and re-enables the interrupts which are disabled by the 
above call.

Write Lock
Lock:
write_lock_irq(rwlock_t *lock)
- This will disable interrupts on that CPU, then grab the lock.

Unlock:
write_unlock_irq(rwlock_t *lock)
- It will release the lock and re-enables the interrupts which are disabled by the 
above call.

Example
/*Tasklet Function*/
void tasklet_fn(unsigned long arg)
{
        write_lock_irq(&etx_rwlock);
        etx_global_variable++;
        write_unlock_irq(&etx_rwlock);
}
 
//Interrupt handler for IRQ 11. 
static irqreturn_t irq_handler(int irq,void *dev_id) {
        read_lock_irq(&etx_rwlock); 
        printk(KERN_INFO "Executing ISR Function : %lu\n", etx_global_variable);
        read_unlock_irq(&etx_rwlock);
        /*Scheduling Task to Tasklet*/
        tasklet_schedule(tasklet); 
        return IRQ_HANDLED;
}
------------------------------------------------------------------------------------------
Approach 5 (Alternative way of Approach 4)

- If you want to use a different variant rather than using read_lock_irq()/write_lock_irq()
and read_unlock_irq( )/write_unlock_irq() then you can use this approach.

Read Lock
Lock:
read_lock_irqsave( rwlock_t *lock, unsigned long flags );
- This will save whether interrupts were on or off in a flags word and grab the lock.

Unlock:
read_unlock_irqrestore( rwlock_t *lock, unsigned long flags );
- This will release the read/write spinlock and restores the interrupts using the flags 
argument.

Write Lock
Lock:
write_lock_irqsave( rwlock_t *lock, unsigned long flags );
- This will save whether interrupts were on or off in a flags word and grab the lock.

Unlock:
write_unlock_irqrestore( rwlock_t *lock, unsigned long flags );
- This will release the read-write spinlock and restores the interrupts using the flags argument.
------------------------------------------------------------------------------------------
Approach 6 (Locking between Hard IRQs)
If you want to share data between two different IRQs, then you should use Approach 5.

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




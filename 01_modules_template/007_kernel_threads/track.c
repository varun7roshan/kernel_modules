##########################################################################################

struct task_struct *thread_1;
const int kt_num = 1;

/*
  int (* threadfn(void *data)
*/
int my_thread(void *data)
{
  while(!kthread_should_stop())
  {
    msleep(5000);
  }
  pr_info("kthread_should_stop() returned TRUE : 1 - hence exiting\n");
  return 0; //or do_exit()
}

static int __init init_load(void)
{
  /*
    struct task_struct * kthread_create (int (* threadfn(void *data), void *data,
                                        const char namefmt[], ...);

				OR
		kthread_run (threadfn, data, namefmt, ...);
  */
  thread_1 = kthread_create(my_thread, NULL, "vaurn-kthread:%d", kt_num);
  if(thread_1)
  {
    /*int wake_up_process (struct task_struct * p);*/
    wake_up_process(thread_1);
  }
  return 0;
}

static void __exit exit_load(void)
{
  /*int kthread_stop ( struct task_struct *k);*/
  kthread_stop(thread_1);
}


##########################################################################################
ps -eaf 
root     28877     2  0 08:48 ?        00:00:00 [vaurn-kthread:1]

##########################################################################################


##########################################################################################
Process
- An "executing instance of a program" is called a "process". 
- Some operating systems use the term "task" to refer to a program that is being executed. 
- The "process is a heavyweight process". 
- The context switch between the process is time-consuming.

Threads
- A thread is an "independent flow of control" that operates within the same address 
space as other independent flows of control within a process.

- One process can have multiple threads, with each thread executing different code 
concurrently, while sharing data and synchronizing much more easily than cooperating 
processes. 
- Threads require fewer system resources than processes and can start more quickly. 
- Threads, also known as "lightweight processes."

- One of the advantages of the thread is that since all the threads within the processes 
share the same address space, the "communication between the threads" is far "easier" 
and less time-consuming as compared to processes. 

- This approach has one "disadvantage" also. It leads to several "concurrency issues" 
and "requires synchronization mechanisms" to handle the same.

Thread Management
- Whenever we are creating a thread, it has to manage by someone.

- A thread is a sequence of instructions.
- CPU can handle one instruction at a time.
- To switch between instructions on parallel threads, the execution state needs to be saved.
- Execution state in its simplest form is a program counter and CPU registers.
- The program counter tells us what instruction to execute next.
- CPU registers hold execution arguments, for example, addition operands.
- This alternation between threads requires management.
- Management includes saving state, restoring state, deciding what thread to pick next.

Types of Thread
There are two types of threads.

1. User Level Thread
2. Kernel Level Thread

User Level Thread
- In this type, the kernel is not aware of these threads. 
- Everything is maintained by the user thread library. That thread library contains 
code for creating and destroying threads, for passing messages and data between threads, 
for scheduling thread execution, and for saving and restoring thread contexts. 
- So all will be in User Space.

Kernel Level Thread
- Kernel level threads are managed by the OS, therefore, thread operations are 
implemented in the kernel code. 
- There is no thread management code in the application area.

Anyhow each type of thread has advantages and disadvantages too.

- Now we will move into Kernel Thread Programming. First, we will see the functions used in a kernel thread.

##########################################################################################
Kernel Thread Management Functions

- There are many functions used in Kernel Thread. We will see each one by one. 
- We can classify those functions based on functionalities.

1. Create Kernel Thread
2. Start Kernel Thread
3. Stop Kernel Thread

Other functions in Kernel Thread
- For use the below functions you should include "linux/kthread.h" header file.
-------------------------------------------------------------------------------------------
- Create Kernel Thread

kthread_create()
This API creates a kthread.

struct task_struct * kthread_create (int (* threadfn(void *data), void *data, const char namefmt[], ...);

threadfn – the function to run until signal_pending(current).
data – data ptr for threadfn.
namefmt[] – printf-style name for the thread.
... – variable arguments

- This helper function creates and names a kernel thread. 
- But we need to wake up that thread manually. 
- When woken, the thread will run threadfn() with data as its argument.

- threadfn can either call "do_exit()" directly if it is a "standalone thread" for 
which no one will call "kthread_stop", or return when "kthread_should_stop" is true 
(which means "kthread_stop" has been called). 
- The return value should be zero or a negative error number; 
it will be passed to "kthread_stop()"

- It Returns task_struct or ERR_PTR(-ENOMEM).
-------------------------------------------------------------------------------------------
Start Kernel Thread

wake_up_process
This is used to Wake up a specific process.

int wake_up_process (struct task_struct * p);

p – The process to be woken up.

- Attempt to wake up the nominated process and move it to the set of runnable processes.

It returns 1 if the process was woken up, 0 if it was already running.

- It may be assumed that this function implies a "write memory barrier" before changing 
the task state if and only if any tasks are woken up.
-------------------------------------------------------------------------------------------
Stop Kernel Thread

kthread_stop
It stops a thread created by kthread_create.

int kthread_stop ( struct task_struct *k);

k – thread created by kthread_create.

- Sets "kthread_should_stop()" for k to return true, wakes it and waits for it to exit. 
- Your "threadfn must not call do_exit itself", if you use this function! 
- This can also be called after kthread_create instead of calling wake_up_process: 
the thread will exit without calling threadfn.

It Returns the result of threadfn, or –EINTR if wake_up_process was never called.
-------------------------------------------------------------------------------------------
Other functions in Kernel Thread

kthread_should_stop
should this kthread return now "?"

int kthread_should_stop (void);

- When someone calls "kthread_stop()" on your kthread, it will be woken and this will 
return true. 
- You should then return, and your return value will be passed through to "kthread_stop()"

-------------------------------------------------------------------------------------------

kthread_bind
This is used to bind a just-created kthread to a CPU.

void kthread_bind (struct task_struct *k, unsigned int cpu);

k – thread created by kthread_create.
cpu – CPU (might not be online, must be possible) for k to run on.
-------------------------------------------------------------------------------------------
Implementation

Thread Function
- First, we have to create our thread that has the argument of void *  and should 
return int value.  
- We should follow some conditions in our thread function. It is advisable.

- If that thread is a long run thread, we need to check "kthread_should_stop()" 
every time, because any function may call "kthread_stop". 
- If any function called "kthread_stop()", that time "kthread_should_stop()" will 
return true. 
- We have to exit our thread function if true value been returned by "kthread_should_stop"

- But if your thread function is not running long, then let that thread finish its task 
and kill itself using "do_exit".

- In my thread function, let’s print something every minute and it is a continuous process.
- So let’s check the "kthread_should_stop()" every time. 
- See the below snippet to understand.

int thread_function(void *pv) 
{
 int i=0;
 while(!kthread_should_stop())
 {
   printk(KERN_INFO "In EmbeTronicX Thread Function %d\n", i++);
   msleep(1000);
 } 
 return 0; 
}
-------------------------------------------------------------------------------------------
Creating and Starting Kernel Thread

- So as of now, we have our thread function to run. Now, we will create a kernel thread 
using "kthread_create" and start the kernel thread using  "wake_up_process".

static struct task_struct *etx_thread; 

etx_thread = kthread_create(thread_function,NULL,"eTx Thread"); 
if(etx_thread) 
{
 wake_up_process(etx_thread); 
} 
else 
{
 printk(KERN_ERR "Cannot create kthread\n"); 
}

- There is another function that does both processes (create and start). 
- That is "kthread_run()". 
- You can replace both "kthread_create" and "wake_up_process" using this function.
-------------------------------------------------------------------------------------------
kthread_run
This is used to create and wake a thread.

kthread_run (threadfn, data, namefmt, ...);

threadfn – the function to run until signal_pending(current).
data – data ptr for threadfn.
namefmt – printf-style name for the thread.
... – variable arguments

Convenient wrapper for kthread_create followed by wake_up_process.

It returns the kthread or ERR_PTR(-ENOMEM).
-------------------------------------------------------------------------------------------
You can see the below snippet which is using kthread_run.

static struct task_struct *etx_thread;

etx_thread = kthread_run(thread_function,NULL,"eTx Thread"); 
if(etx_thread) 
{
 printk(KERN_ERR "Kthread Created Successfully...\n");
}
else 
{
 printk(KERN_ERR "Cannot create kthread\n"); 
}
-------------------------------------------------------------------------------------------
Stop Kernel Thread
You can stop the kernel thread using kthread_stop.  Use the below snippet to stop.

kthread_stop(etx_thread);

##########################################################################################


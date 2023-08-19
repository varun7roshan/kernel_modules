##########################################################################################
- Work Queue is not a thread - Though it is implemented on top of Kernel Threads
- We will call "schedule_work()" or "scheduled_delayed_work()" - which will call our
function - This function is executed only once - The Function by itself is not a Thread

- Same goes with - Dedicated work queues - our function will be called from a dedicated
kernel thread - Here if we want we can do while(1) processing if we want - as it is on
a dedicated kernel thread - but while(1) or sleep(100) cannot be done on a shared Work Queue
>> sleep(100) can be done in shared work queue as well - it will cause, other work handlers
to be starved - in this case its better to have dedicated work queues
>> while(1) must not be done in shared work handler at all

struct work_struct 
{
  atomic_long_t data;
  struct list_head entry;
  work_func_t func;
};
##########################################################################################

----------- DYNAMIC METHOD - INIT_WORK()-------------------

#include <linux/workqueue.h>

struct work_data 
{
	struct work_struct my_work;
	wait_queue_head_t my_wq;
	int the_data;
};

/*void (*work_func_t)(struct work_struct *work);*/
static void work_handler(struct work_struct *work)
{
	struct work_data *my_data = container_of(work, struct work_data, my_work);
}

static int __init my_init(void)
{
	struct work_data * my_data;

	my_data = kmalloc(sizeof(struct work_data), GFP_KERNEL);

	/* INIT_WORK(struct work_struct *work, void (*work_func_t)(struct work_struct *work);)
	*/	
	INIT_WORK(&my_data->my_work, work_handler); //DYNAMIC Method
	schedule_work(&my_data->my_work);
}

static void __exit exit_load(void)
{
#if 1
  /*Also flush_scheduled_work() gets blocked till wq is finished in rmmod*/
  //for gloabal wq to finish - better to call cancel_work_sync()- that will also block
  flush_scheduled_work();
#else
  /*so when sudo rmmod is invoked - rmmod only gets blocked - till wq finished*/

  cancel_work_sync(wq_malloc);
#endif
  kfree(wq_malloc);
}

##########################################################################################

------------- STATIC Method - DECLAR_WORK --------------------

1. This Code is using "Shared Work Queue" - So only we need to define "WORK" function
that is via : struct work_struct; which has a memeber to hold our "work" function
- We should not create a struct work_struct ourselves - we must used the MACRO :

DECLARE_WORK(Name_of_work_struct, callback_function);
- Internally the MACRO sets up more for us


void work_handler_fn(struct work_struct *work);

/*Creating work by Static Method - for Dynamic Method use INIT_WORK()
	DECLARE_WORK(variable_name, callback_function);
*/
DECLARE_WORK(my_work_struct, work_handler_fn); //------------------(1)

/*Workqueue Function*/
void work_handler_fn(struct work_struct *work) //----------------(2)
{
	/*This will run only once - and return - again when schedule_work() is called 
		This work handler is executed*/
  printk(KERN_INFO "Executing Workqueue Function\n");
}

//Interrupt handler for IRQ 11. 
static irqreturn_t irq_handler(int irq,void *dev_id) 
{
	printk(KERN_INFO "Shared IRQ: Interrupt Occurred");
	//static inline bool schedule_work(struct work_struct *work)
  schedule_work(&workqueue); //------------------------(3)
        
  return IRQ_HANDLED;
}

static int __init etx_driver_init(void)
{
	if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "etx_device", (void *)(irq_handler))) {
            printk(KERN_INFO "my_device: cannot register IRQ ");
                    goto irq;
}

##########################################################################################
----------- Dedicated Work Queue -----------

struct workqueue_struct *my_workqueue;

DECLARE_WORK(static_work_struct, my_work_handler);

void my_work_handler(struct work_struct *work)
{
	msleep(10000); //10 sec sleep
}

static int __init init_load(void)
{
  my_workqueue = create_workqueue("OWN_WQ");

  //int queue_work( struct workqueue_struct *wq, struct work_struct *work );
  queue_work(my_workqueue, &static_work_struct);

  return 0;
}

static void __exit exit_load(void)
{
  destroy_workqueue(my_workqueue);
}

##########################################################################################
Dedicated Work Queue:

- The core workqueue is represented by structure "struct workqueue_struct", which is the 
structure onto which work is placed. 
- This work is added to the queue in the top half (Interrupt context) and the execution of 
this work happened in the bottom half (Kernel context).

Create and destroy workqueue structure
- Workqueues are created through a macro called "create_workqueue()", which returns a 
"workqueue_struct" reference. You can remove this workqueue later(if needed)through a call
to the "destroy_workqueue()" function.

struct workqueue_struct *create_workqueue( name );

void destroy_workqueue( struct workqueue_struct * );

- You should use "create_singlethread_workqueue()" for creating a workqueue when you want 
to create only a "single thread for all the processors".

- Since "create_workqueue" and "create_singlethread_workqueue()" are "macros". 
Both are using the "alloc_workqueue" function in the background.
-------------------------------------------------------------------------------------------
"alloc_workqueue()"
Allocate a workqueue with the specified parameters.

alloc_workqueue ( fmt, flags, max_active );

fmt– printf format for the name of the workqueue
flags – WQ_* flags
max_active – max in-flight work items, 0 for default
This will return Pointer to the allocated workqueue on success, NULL on failure.

WQ_* flags
This is the second argument of alloc_workqueue.

WQ_UNBOUND:
Work items queued to an unbound wq are served by the special worker-pools which host workers who are not bound to any specific CPU. This makes the wq behave like a simple execution context provider without concurrency management. The unbound worker-pools try to start the execution of work items as soon as possible. Unbound wq sacrifices locality but is useful for the following cases.
Wide fluctuation in the concurrency level requirement is expected and using bound wq may end up creating a large number of mostly unused workers across different CPUs as the issuer hops through different CPUs.
Long-running CPU-intensive workloads which can be better managed by the system scheduler.

WQ_FREEZABLE:
A freezable wq participates in the freeze phase of the system suspend operations. Work items on the wq are drained and no new work item starts execution until thawed.

WQ_MEM_RECLAIM:
All wq which might be used in the memory reclaim paths MUST have this flag set. The wq is guaranteed to have at least one execution context regardless of memory pressure.

WQ_HIGHPRI:
Work items of a highpri wq are queued to the highpri worker-pool of the target CPU. Highpri worker-pools are served by worker threads with elevated nice levels.

Note that normal and highpri worker-pools don’t interact with each other. Each maintains its separate pool of workers and implements concurrency management among its workers.

WQ_CPU_INTENSIVE:
Work items of a CPU intensive wq do not contribute to the concurrency level. In other words, runnable CPU-intensive work items will not prevent other work items in the same worker pool from starting execution. This is useful for bound work items that are expected to hog CPU cycles so that their execution is regulated by the system scheduler.

Although CPU-intensive work items don’t contribute to the concurrency level, the start of their executions is still regulated by the concurrency management and runnable non-CPU-intensive work items can delay the execution of CPU-intensive work items.

This flag is meaningless for unbound wq.
-------------------------------------------------------------------------------------------
Queuing Work to workqueue
- With the work structure initialized, the next step is enqueuing the work on a workqueue. You can do this in a few ways.

queue_work
This will queue the work to the CPU on which it was submitted, but if the CPU dies it can be processed by another CPU.
int queue_work( struct workqueue_struct *wq, struct work_struct *work );

wq – workqueue to use
work – work to queue
It returns false if work was already on a queue, true otherwise.
-------------------------------------------------------------------------------------------
queue_work_on
This puts work on a specific CPU.
int queue_work_on( int cpu, struct workqueue_struct *wq, struct work_struct *work );

cpu– cpu to put the work task on
wq – workqueue to use
work– job to be done
-------------------------------------------------------------------------------------------
queue_delayed_work
After waiting for a given time this function puts work in the workqueue.

int queue_delayed_work( struct workqueue_struct *wq,struct delayed_work *dwork, unsigned long delay );
Where,

wq – workqueue to use
dwork – work to queue
delay– number of jiffies to wait before queueing or 0 for immediate execution
-------------------------------------------------------------------------------------------
queue_delayed_work_on
After waiting for a given time this puts a job in the workqueue on the specified CPU.
int queue_delayed_work_on( int cpu, struct workqueue_struct *wq,struct delayed_work *dwork, unsigned long delay );

cpu– CPU to put the work task on
wq – workqueue to use
dwork – work to queue
delay– number of jiffies to wait before queueing or 0 for immediate execution
-------------------------------------------------------------------------------------------
- We can able to see the print “Shared IRQ: Interrupt Occurred“ and “Executing Workqueue 
Function“
- Use “ps -aef” command to see our workqueue. You can able to see our workqueue which is 
“own_wq“

pi@pi:~ $ ps -aef | grep -i own
root      8217     2  0 04:20 ?        00:00:00 [kworker/0:1+OWN_WQ]
root      8574     2  0 04:24 ?        00:00:00 [OWN_WQ]
pi        8632  1857  0 04:24 pts/0    00:00:00 grep --color=auto -i own

pi@pi:~ $ ps -aef | grep -i own
root        70     2  0 02:45 ?        00:00:00 [kworker/3:1+OWN_WQ]
root      8717     2  0 04:25 ?        00:00:00 [OWN_WQ]
pi        8724  1857  0 04:25 pts/0    00:00:00 grep --color=auto -i own
-------------------------------------------------------------------------------------------
Difference between "schedule_work()" and "queue_work()"

- If you want to use your "own dedicated workqueue" you should "create a workqueue" using 
"create_workqueue()". At that time you need to "put work on your workqueue" by using 
"queue_work()" function.

- If you don’t want to create any own workqueue, you can use "kernel global workqueue". 
- In that condition, you can use "schedule_work()" function to put your work to global 
workqueue.

##########################################################################################
- The below call creates a "workqueue" by the "name" and the function that we are passing 
in the second argument gets scheduled in the queue.

DECLARE_WORK(name, void (*func)(void *))

name: The name of the “work_struct” structure that has to be created.
func: The function to be scheduled in this workqueue.

Example:
DECLARE_WORK(workqueue,workqueue_fn);
-----------------------------------------------------------------------------------------
- "Schedule_work()"
- This function puts a "job" in the "kernel-global workqueue" if it was not already 
queued and leaves it in the same position on the kernel-global workqueue otherwise.

int schedule_work( struct work_struct *work );

work:job to be done

Returns :zero if work was already on the kernel-global workqueue and non-zero otherwise.
-----------------------------------------------------------------------------------------
Scheduled_delayed_work()
- After waiting for a given time this function puts a job in the kernel-global workqueue.

int scheduled_delayed_work( struct delayed_work *dwork, unsigned long delay );

dwork:job to be done
delay: number of jiffies to wait or 0 for immediate execution
-----------------------------------------------------------------------------------------
"Schedule_work_on()"
- This puts a job on a specific CPU.

int schedule_work_on( int cpu, struct work_struct *work );

cpu: CPU to put the work task on
work: job to be done
-----------------------------------------------------------------------------------------
Scheduled_delayed_work_on()
- After waiting for a given time this puts a job in the kernel-global workqueue on the specified CPU.

int scheduled_delayed_work_on(int cpu, struct delayed_work *dwork, unsigned long delay );
cpu:CPU to put the work task on
dwork:job to be done
delay:number of jiffies to wait or 0 for immediate execution
-----------------------------------------------------------------------------------------
Flush / wait / block till work queue is finished executing

- There are also a number of helper functions that you can use to flush or cancel work on 
work queues. 
- To flush a particular work item and block until the work is complete, you can make a 
call to "flush_work()". 
- All work on a given work queue can be completed using a call to "flush_work()". 
- In both cases, the "caller blocks until the operation are complete". 
- To flush the "kernel-global work queue", call "flush_scheduled_work()".

int flush_work(struct work_struct *work); // for our dedicated WQ to finish

void flush_scheduled_work( void ); // for global work queue to finish
-----------------------------------------------------------------------------------------
Cancel Work from workqueue
- You can cancel work if it is not already executing in a handler. 
- A call to "cancel_work_sync()" will terminate the work in the queue or block until the 
callback has finished (if the work is already in progress in the handler). 
- If the work is delayed, you can use a call to "cancel_delayed_work_sync()".

int cancel_work_sync( struct work_struct *work );
int cancel_delayed_work_sync( struct delayed_work *dwork );

-----------------------------------------------------------------------------------------
Check the workqueue
- Finally, you can find out whether a work item is pending (not yet executed by the 
handler) with a call to work_pending() or delayed_work_pending().

work_pending( work );
delayed_work_pending( work );

-----------------------------------------------------------------------------------------

##########################################################################################


##########################################################################################


##########################################################################################
There are 4 bottom half mechanisms are available in Linux:

1. Workqueue in Linux Kernel – Executed in a process context.
2. Threaded IRQ
3. Softirq – Executed in an atomic context.
4. Tasklet – Executed in an atomic context.

##########################################################################################
Work queues - Simple Work Deferring Mechanism - can be used as Bottom Half
-----------
- Added since Linux kernel 2.6, the most used and "simple deferring mechanism" is the 
"work queue". 

- As a deferring mechanism, it takes an opposite approach to the others we've seen, running only in a pre-emptible context. 
>> So WORK QUEUES - run in pre-emptible context - like Threads and Processes
>> Moreover "Work Queues" - are itself Threads - internally

- It is the only choice when you need to "sleep" in your "bottom half" 
- By sleep, I mean process I/O data, hold mutexes, delay, and all the other tasks that 
may lead to sleep or move the task off the run queue.
>> Notice the work queue can be moved off the run queue - that means it must be a Thread

- Keep in mind that "work queues are built on top of kernel threads", and this is the 
reason why I decided not to talk about the kernel thread as a deferring mechanism at all. 
>> So "Work Queue" itlsef a "Kernel Thread"
>> Why a seperate "Work Queue" concept "?", we already have kernel threads right
- The reason to have "work queues" is that - we can have one thread for each CPU -which can
take "work queues" and execute them - Shared Work Queue
- Without this "WQ" concept - we would have to create "kernel threads" ourselves for
each of the work deffering mechanisms - Dedicated Work Queues

- However, there are two ways to deal with work queues in the kernel. 
- First, there is a 
1. default shared work queue, handled by a set of kernel threads, each running on a CPU.
- Once you have work to schedule, you queue that work into the global work queue, which 
will be executed at the appropriate moment. 

2. The other method is to "run the work queue" in a "dedicated kernel thread". 
- It means whenever your work queue handler needs to be executed, your kernel thread is 
woken up to handle it, instead of one of the default predefined threads.
- Structures and functions to call are different, depending on whether you chose a shared
work queue or dedicated ones.
##########################################################################################

Kernel-global work queue – the shared queue
-------------------------------------------
- Unless you have no choice, you need critical performance, or you need to control
everything from "work queue initialization to work scheduling", and if you 
"only submit tasks occasionally", you should "use the shared work queue" provided by the 
kernel. 
- With that queue being shared over the system, you should be nice, and should not 
monopolize the queue for a long time.

- Since the "execution of the pending task on the queue is serialized on each CPU", 
you should not sleep for a long time because "no other task on the queue will run" until 
you "wake up".
>>>>>>>> THIS IS IMPORTANT POINT :
>> Once we register our work queue with the shared queue - The kernel thread which calls
our work queue function - must do quick processing and not hog on to CPU - by sleeping
as it will effect the execution of other work queues - The function calls to the
registered work queues are "Serialized" 

- "You won't even know who you share the work queue with", so don't be surprised if your
task takes longer to get the CPU. 

- "Work in shared work queues" is executed in a per-CPU thread called "events/n", created 
by the kernel.

- In this case, the work must also be initialized with the "INIT_WORK" macro. 
- Since we are going to use the "shared work queue", there is "no need to create" a 
"work queue structure". 

- We only need the "work_struct structure" that will be passed as an argument. 
- There are three functions to schedule work on the shared work queue:

1. struct workqueue_struct
2. struct work_struct

- The version that ties the work on the "current CPU:"
int schedule_work(struct work_struct *work);

The same, but "delayed, function":
static inline bool schedule_delayed_work(struct delayed_work *dwork,unsigned long delay)

- The function that actually "schedules the work on a given CPU":
int schedule_work_on(int cpu, struct work_struct *work);

- The same as shown previously, but with a "delay":
int scheduled_delayed_work_on(int cpu, struct delayed_work *dwork, unsigned long delay);

- All of these functions schedule the work given as an argument on the system's shared work
queue, "system_wq", defined in kernel/workqueue.c:

struct workqueue_struct *system_wq __read_mostly;
EXPORT_SYMBOL(system_wq);

- Work already submitted to the shared queue can be canceled with the 
"cancel_delayed_work()" function. You can flush the shared work queue with:
void flush_scheduled_work(void);

- In order to pass data to my work queue handler, you may have noticed that, in both 
examples, I've embedded my "work_struct" structure inside
my custom data structure and used container_of to retrieve it. This is a common way to 
pass data to the work queue handler.

##########################################################################################

Dedicated work queue
--------------------
- Here, the "work queue" is represented as an instance of struct workqueue_struct. 
- The work to be queued into the work queue is represented as an instance of 
struct work_struct. 

- There are four steps involved prior to scheduling your work in your own kernel thread:
1. Declare/initialize a struct workqueue_struct
2. Create your work function
3. Create a struct work_struct so that your work function will be embedded into it
4. Embed your work function in the work_struct

Programming syntax
- The following functions are defined in include/linux/workqueue.h:
- 
Declare the work and work queue:
struct workqueue_struct *myqueue;
struct work_struct thework;

- Define the worker function (the handler):
void dowork(void *data) { /* Code goes here */ };

- Initialize our work queue and embed our work into it:
myqueue = create_singlethread_workqueue( "mywork" );

INIT_WORK( &thework, dowork, <data-pointer> );

We could have also created our work queues through a macro called "create_workqueue()". 

- The difference between "create_workqueue()" and "create_singlethread_workqueue()" 
is that the former will create a work queue that in turn will create a separate kernel 
thread on each and every processor available.

- Scheduling work:
queue_work(myqueue, &thework);

- Queue after the given delay to the given worker thread:
queue_dalayed_work(myqueue, &thework, <delay>);
These functions return false if the work was already on a queue and true if otherwise. 

- delay represents the number of jiffies to wait before queueing. 

- You may use the helper function "msecs_to_jiffies()" in order to convert the standard
ms delay into jiffies. For example, to queue work after 5 ms, you can use

queue_delayed_work(myqueue, &thework, msecs_to_jiffies(5));.

- Wait on all pending work on the given work queue:
void flush_workqueue(struct workqueue_struct *wq)
- flush_workqueue() "sleeps until all queued work has finished its execution". 
- "New incoming (enqueued) work does not affect the sleep". 

- You may typically use this in driver shutdown handlers.

Cleanup: Use "cancel_work_sync()" or "cancel_delayed_work_sync()" for synchronous 
cancellation, which will cancel the work if it is not already running,
or block until the work has completed. 
- The work will be cancelled even if it requeues itself. You must also ensure that the 
work queue on which the work was last queued can't be destroyed before the handler returns

- These functions are to be used for nondelayed or delayed work, respectively:
int cancel_work_sync(struct work_struct *work);
int cancel_delayed_work_sync(struct delayed_work *dwork);

Since Linux kernel v4.8, it has been possible to use cancel_work or cancel_delayed_work, 
which are asynchronous forms of cancellation. 
You must check whether the function returns true or not, and makes sure the work does not 
requeue itself.

- You must then explicitly flush the work queue:
if ( !cancel_delayed_work( &thework) )
{
	flush_workqueue(myqueue);
	destroy_workqueue(myqueue);
}

- The other is a different version of the same method and will create only a 
single thread for all the processors. 

- If you need a delay before the work is enqueued, feel free to use the following work 
initialization macro:

INIT_DELAYED_WORK(_work, _func);
INIT_DELAYED_WORK_DEFERRABLE(_work, _func);

- Using the preceding macros would imply that you should use the following functions to
queue or schedule the work in the work queue:

int queue_delayed_work(struct workqueue_struct *wq, 
											 struct delayed_work *dwork, unsigned long delay)

- queue_work ties the work to the current CPU. 

- You can specify the CPU on which the handler should run using the queue_work_on function:
int queue_work_on(int cpu, struct workqueue_struct *wq, struct work_struct *work);

- For delayed work, you can use:
int queue_delayed_work_on(int cpu, struct workqueue_struct *wq,
struct delayed_work *dwork, unsigned long delay);

##########################################################################################
Predefined work queue function
------------------------------
schedule_work(w)
schedule_delayed_work(w,d)
schedule_delayed_work_on(cpu,w,d)
flush_scheduled_work()

Equivalent standard work queue function
---------------------------------------
queue_work(keventd_wq,w)
queue_delayed_work(keventd_wq,w,d)(on any CPU)
queue_delayed_work(keventd_wq,w,d) (on a given CPU)
flush_workqueue(keventd_wq)

##########################################################################################


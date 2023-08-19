- Basic Template for Linux Kernel Module
-------------------------------------------------
#include <linux/module.h>

static int __init init_load(void)
{

}

static void __exit exit_load(void)
{

}

module_init(init_load);
module_exit(exit_load);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Varun");
MODULE_DESCRIPTION("Linked List and Containerof");
MODULE_INFO(board, "Raspberry_PI_3_Model_B");
###########################################################################################

LIST_HEAD(HEAD_my_list_static); //HEAD of the List - Static Creation

INIT_LIST_HEAD(&car_1->node); //initialize the new node

list_add(&car_1->node, &HEAD_my_list_static); //add the list to the begining

list_add_tail(&car_3->node, &HEAD_my_list_static); //add the list to tail

list_del(&car_1->node); //delete the node from the list, HEAD is not needed

list_for_each_entry(tmp_car_loop_counter, pHead, node)
{
	//To iterate over each list entry - we get back the main struct, containerof()
	//will be used 'node' --> member
}

###########################################################################################
#include <linux/slab.h>
kmalloc() and kzalloc()

#define GFP_ATOMIC	(__GFP_HIGH|__GFP_ATOMIC|__GFP_KSWAPD_RECLAIM)
#define GFP_KERNEL	(__GFP_RECLAIM | __GFP_IO | __GFP_FS)
#define GFP_KERNEL_ACCOUNT (GFP_KERNEL | __GFP_ACCOUNT)
#define GFP_NOWAIT	(__GFP_KSWAPD_RECLAIM)
#define GFP_NOIO	(__GFP_RECLAIM)
#define GFP_NOFS	(__GFP_RECLAIM | __GFP_IO)
#define GFP_USER	(__GFP_RECLAIM | __GFP_IO | __GFP_FS | __GFP_HARDWALL)
#define GFP_DMA		__GFP_DMA
#define GFP_DMA32	__GFP_DMA32
#define GFP_HIGHUSER	(GFP_USER | __GFP_HIGHMEM)
#define GFP_HIGHUSER_MOVABLE	(GFP_HIGHUSER | __GFP_MOVABLE | \
			 __GFP_SKIP_KASAN_POISON | __GFP_SKIP_KASAN_UNPOISON)
#define GFP_TRANSHUGE_LIGHT	((GFP_HIGHUSER_MOVABLE | __GFP_COMP | \
			 __GFP_NOMEMALLOC | __GFP_NOWARN) & ~__GFP_RECLAIM)
#define GFP_TRANSHUGE	(GFP_TRANSHUGE_LIGHT | __GFP_DIRECT_RECLAIM)

###########################################################################################
Check in : linux/include/linux/list.h
-------------------------------------------------------------------------------------------
#define LIST_HEAD_INIT(name) { &(name), &(name) }
>> Basically this initializes the members "*prev" and "*next" to point it to self
We can call this when:
1. New Node is created - either statically : 
	struct list_head my_node_1;
	LIST_HEAD_INIT(&my_node_1);
>> Not only for nodes - we call this for HEAD variable as well
2. Dynamically kmalloc() Node - we can call "LIST_HEAD_INIT()"
-------------------------------------------------------------------------------------------
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)
>> This is used to create a variable of "struct list_head" - usually to create a HEAD
statically - better to use this, than creating a variable ourselves with "struct list_head"
since it is internally calling "LIST_HEAD_INIT()" which will initialize the members

-------------------------------------------------------------------------------------------
/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void list_add(struct list_head *new, struct list_head *head)
>> When we call list_add() - it will insert at the begining
-------------------------------------------------------------------------------------------
/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *new, struct list_head *head)
-------------------------------------------------------------------------------------------
static inline void list_del(struct list_head *entry);
-------------------------------------------------------------------------------------------
/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void list_replace(struct list_head *old, struct list_head *new)
-------------------------------------------------------------------------------------------
/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int list_empty(const struct list_head *head)

-------------------------------------------------------------------------------------------
/**
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline int list_is_singular(const struct list_head *head)
{
  return !list_empty(head) && (head->next == head->prev);
}
-------------------------------------------------------------------------------------------
/**
 * list_entry - get the struct for this entry
 * @ptr:  the &struct list_head pointer.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
  container_of(ptr, type, member)

-------------------------------------------------------------------------------------------
/**
 * list_for_each  - iterate over a list
 * @pos:  the &struct list_head to use as a loop cursor.
 * @head: the head for your list.
 */
#define list_for_each(pos, head) \
  for (pos = (head)->next; pos != (head); pos = pos->next)
-------------------------------------------------------------------------------------------
/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:  the &struct list_head to use as a loop cursor.
 * @n:    another &struct list_head to use as temporary storage
 * @head: the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
  for (pos = (head)->next, n = pos->next; pos != (head); \
    pos = n, n = pos->next)
-------------------------------------------------------------------------------------------
/**
 * list_for_each_entry  - iterate over list of given type
 * @pos:  the type * to use as a loop cursor.
 * @head: the head for your list.
 * @member: the name of the list_head within the struct.
 */
#define list_for_each_entry(pos, head, member)        \
  for (pos = list_first_entry(head, typeof(*pos), member);  \
       &pos->member != (head);          \
       pos = list_next_entry(pos, member))
###########################################################################################
Kernel Panic when "rmmod"

[  250.612974] Unable to handle kernel paging request at virtual address 7f6811f0
[  250.613005] pgd = 0b7afe7b
[  250.613017] [7f6811f0] *pgd=3314b811, *pte=00000000, *ppte=00000000
[  250.613045] Internal error: Oops: 80000007 [#1] SMP ARM
[  250.613058] Modules linked in: 03_ll_load(O-) fuse rfcomm bnep hci_uart btbcm serdev bluetooth ecdh_generic 8021q garp stp llc brcmfmac brcmutil cfg80211 rfkill raspberrypi_hwmon hwmon snd_bcm2835(C) bcm2835_codec(C) bcm2835_v4l2(C) snd_pcm v4l2_mem2mem bcm2835_mmal_vchiq(C) snd_timer videobuf2_dma_contig v4l2_common videobuf2_vmalloc snd videobuf2_memops videobuf2_v4l2 videobuf2_common videodev media vc_sm_cma(C) uio_pdrv_genirq uio fixed i2c_dev ip_tables x_tables ipv6
[  250.613186] CPU: 2 PID: 1298 Comm: rmmod Tainted: G         C O      4.19.127-v7+ #1
[  250.613196] Hardware name: BCM2835
[  250.613211] PC is at 0x7f6811f0
[  250.613232] LR is at sys_delete_module+0x190/0x208
[  250.613243] pc : [<7f6811f0>]    lr : [<801ada40>]    psr: 20000013
[  250.613254] sp : aa321f48  ip : 00000000  fp : aa321fa4
[  250.613265] r10: 00000081  r9 : aa320000  r8 : 801011c4
[  250.613277] r7 : 00000081  r6 : 01ec82e4  r5 : 7f67e040  r4 : 80d04d48
[  250.613289] r3 : 7f6811f0  r2 : 00000000  r1 : 00000000  r0 : 80d12884
[  250.613301] Flags: nzCv  IRQs on  FIQs on  Mode SVC_32  ISA ARM  Segment user
[  250.613313] Control: 10c5383d  Table: 2944c06a  DAC: 00000055
[  250.613326] Process rmmod (pid: 1298, stack limit = 0xfb3ce5bd)
[  250.613338] Stack: (0xaa321f48 to 0xaa322000)
[  250.613355] 1f40:                   6c5f3330 6f6c5f6c 00006461 801011c4 aa320000 00000006
[  250.613373] 1f60: aa321f8c aa321f70 80c90ad8 80101068 7eec65b8 00000006 801011c4 aa320000
[  250.613391] 1f80: 00321fac ca74fd61 801f7460 01ec82a8 7eec7600 00000002 00000000 aa321fa8
[  250.613409] 1fa0: 80101000 801ad8bc 01ec82a8 7eec7600 01ec82e4 00000800 66144500 66144500
[  250.613426] 1fc0: 01ec82a8 7eec7600 00000002 00000081 7eec78ec 01ec82a8 00000000 0003fce8
[  250.613443] 1fe0: 0003ff88 7eec75b4 00022bb8 76c37fcc 20000010 01ec82e4 00000000 00000000
[  250.613476] [<801ada40>] (sys_delete_module) from [<80101000>] (ret_fast_syscall+0x0/0x28)
[  250.613488] Exception stack(0xaa321fa8 to 0xaa321ff0)
[  250.613503] 1fa0:                   01ec82a8 7eec7600 01ec82e4 00000800 66144500 66144500
[  250.613521] 1fc0: 01ec82a8 7eec7600 00000002 00000081 7eec78ec 01ec82a8 00000000 0003fce8
[  250.613534] 1fe0: 0003ff88 7eec75b4 00022bb8 76c37fcc
[  250.613549] Code: bad PC value
[  250.613566] ---[ end trace 445efd379fb5c7aa ]---

BUG : 
static void __init exit_load(void)
{

}
>> "__init" is added so the kernel panic occured - it was trying to find the exit_load
in the __init section

Fix : __exit
###########################################################################################
Another Panic:

[  144.391599] ------------TOP of EXIT----------
[  144.391610] In Exit - Cars to free
[  144.391615] Priting the List HEAD
[  144.391621] Car Model Number : 2
[  144.391627] Car Color        : Black
[  144.391633] Car Name         : Vento
[  144.391638] Car Model Number : 3
[  144.391643] Car Color        : White
[  144.391648] Car Name         : Bolero
[  144.391653] ------END of Print----------------
[  144.391658] calling free_cars()
[  144.391663] Removing this car from list
[  144.391668] Car Model Number : 2
[  144.391673] Car Color        : Black
[  144.391678] Car Name         : Vento
[  144.391682] calling free() for above car
>> Here this above car_2 is kfree() is successful
[  144.391689] Removing this car from list

[  144.391707] Unable to handle kernel NULL pointer dereference at virtual address 000000f4
[  144.391729] pgd = a86f59ac
[  144.391741] [000000f4] *pgd=29419835, *pte=00000000, *ppte=00000000

>> Looks like kfree() with NULL is passed - check in free_cars() if linked list traversal
is correct 


###########################################################################################
[  403.943125] car_1:8de8acb5
[  403.943132] car_2:cd5fd7bc
[  403.943138] car_3:76ba72d4

[  410.902118] In Exit - Cars to free
[  410.902124] Priting the List HEAD
[  410.902136] Car kzalloc      : cd5fd7bc
[  410.902142] Car Model Number : 2
[  410.902147] Car Color        : Black
[  410.902153] Car Name         : Vento
[  410.902159] Car kzalloc      : 76ba72d4
[  410.902165] Car Model Number : 3
[  410.902170] Car Color        : White
[  410.902175] Car Name         : Bolero
[  410.902180] ------END of Print----------------
[  410.902185] calling free_cars()
[  410.902192] Removing this car from list : tmp_car_loop_counter:cd5fd7bc
[  410.902197] Car Model Number : 2
[  410.902202] Car Color        : Black
[  410.902207] Car Name         : Vento
[  410.902212] calling free() for above car
[  410.902221] Removing this car from list : tmp_car_loop_counter:7f482529
[  410.902238] Unable to handle kernel NULL pointer dereference at virtual address 000000f4

Bug : Got to know the issue :
- When we are in list_for_each_entry() - and we kfree() the node inside the loop
then, the loop_counter node - does not exists in the LIST HEAD - and a defrence into
the next will cause a panic

Fix : When travesing the Linked list we have to save each node in an array and then
free one by one - Else the list_for_each_entry() tmp_loop_counter will be effected

###########################################################################################
[  416.601540] car_1:78399cdd
[  416.601547] car_2:c2c72b83
[  416.601553] car_3:6a9f75a6

[  440.141178] ------------TOP of EXIT----------
[  440.141189] In Exit - Cars to free
[  440.141194] Priting the List HEAD
[  440.141206] Car kzalloc      : c2c72b83
[  440.141212] Car Model Number : 2
[  440.141217] Car Color        : Black
[  440.141222] Car Name         : Vento
[  440.141229] Car kzalloc      : 6a9f75a6
[  440.141234] Car Model Number : 3
[  440.141239] Car Color        : White
[  440.141244] Car Name         : Bolero
[  440.141249] ------END of Print----------------
[  440.141254] calling free_cars()
[  440.141263] Removing this car from list : tmp_car_loop_counter:c2c72b83
[  440.141268] Car Model Number : 2
[  440.141273] Car Color        : Black
[  440.141278] Car Name         : Vento
[  440.141285] Removing this car from list : tmp_car_loop_counter:6a9f75a6
[  440.141290] Car Model Number : 3
[  440.141295] Car Color        : White
[  440.141300] Car Name         : Bolero
[  440.141308] Got ptr_free_array[0]:c2c72b83
[  440.141315] Got ptr_free_array[1]:6a9f75a6
[  440.141323] calling list_del(&(ptr_free_array[0]->node:c2c72b83));
[  440.141328] calling free() for above car
[  440.141337] calling list_del(&(ptr_free_array[1]->node:6a9f75a6));
[  440.141342] calling free() for above car
[  440.141349] Freeing the array pointer : ptr_free_array : ea66a2bc
[  440.141354] Endof free_cars
[  440.141360] All cars freed - again printing the List HEAD to check
[  440.141364] Priting the List HEAD
[  440.141369] ------END of Print----------------
[  440.141374] ------------END of EXIT----------

###########################################################################################


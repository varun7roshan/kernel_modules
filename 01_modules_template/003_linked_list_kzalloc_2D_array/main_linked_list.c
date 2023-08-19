#include <linux/module.h>
#include <linux/slab.h> //kmalloc() kzalloc()
#include <linux/list.h> //for linked list

#define STR_BALENO "Baleno"
#define STR_VENTO	 "Vento"
#define STR_BOLERO "Bolero"

#define STR_SILVER	"Silver"
#define STR_BLACK		"Black"
#define STR_WHITE		"White"

/*
struct list_head
{
	struct list_head *prev;
	struct list_head *next;
}
>> So this "struct list_head" is of a Basic NODE
So the HEAD will also be a type of "struct list_head" and the NODE in the
embedded struct. So both HEAD and NODE of same type "struct list_head"
*/

struct car
{
	int model_num;
	char *color;
	char *name;
	struct list_head node;
};


void print_list(struct list_head *pHead)
{
	/* We need to Traverse the List and print
		list_for_each_entry(struct car *pos, struct list_head *head, struct list_head *member)
		pos : loop counter - but needs to be type of "struct car"
		head : head of the List
		member : The "NODE" name in the "struct car" of the type "struct list_head"
	*/

	struct car *tmp_car_loop_counter;
	
	list_for_each_entry(tmp_car_loop_counter, pHead, node)
	{
		/*Now the "tmp_car_loop_counter" will point to the First "struct car"*/
		pr_info("Car kzalloc      : %p\n", tmp_car_loop_counter);
		pr_info("Car Model Number : %d\n", tmp_car_loop_counter->model_num);
		pr_info("Car Color        : %s\n", tmp_car_loop_counter->color);
		pr_info("Car Name         : %s\n", tmp_car_loop_counter->name);
	}
}

/*
	For Dynamic Freeing:
	1. We traverse the linked list and count the number of node
     Dynamically allocate an arrya of pointer
		 Again traverse the LL to save the node pointers
		 call kfree() for the array elements
	2. We can have another LL to save the node pointers - than a array of pointers
		 - By this method we need to traverse the list only once and in each traverse
       we add the node pointer address to LL of pointers to free
		 - We need to traverse the new LL to free the pointers in them
		 - This is a headache - better is to save in a Dynamic Array - we can realloc, if we want
		 - creating another LL is a problem
	3. Have a Garbage collector - either my_kzalloc() and my_free()
		- In my_kzalloc() - we can have Array or LL of poniters
			>> Linked list of pointers is good - NO read below - Array of pointers is better
		- In my_kfree() - we can traverse the LL and free all the pointers in them
		- and also delete the node after freeing - Again this is same problem
			>> Better to have Array of pointer - Have a large Limit - realloc if needed
			- or malloc() again and copy and then free() old
*/
void free_cars(struct list_head *pHead)
{
	/*traverse the list and get the Node and free them*/
	struct car *tmp_car_loop_counter;
	struct car **ptr_free_array;
	int list_len = 0;	
	int i = 0;

	list_for_each_entry(tmp_car_loop_counter, pHead, node)
	{
		pr_info("Removing this car from list : tmp_car_loop_counter:%p\n", tmp_car_loop_counter);
		pr_info("Car Model Number : %d\n", tmp_car_loop_counter->model_num);
		pr_info("Car Color        : %s\n", tmp_car_loop_counter->color);
		pr_info("Car Name         : %s\n", tmp_car_loop_counter->name);
		list_len++;
	}

	if(list_len)
	{
		/*allocate an array of pointers - number of elements in the array x (void *)
			we can use kzalloc(sizeof(void *)*list_len, GFP_KERNEL) as well*/
		ptr_free_array = (struct car **)kzalloc(sizeof(struct car *)*list_len, GFP_KERNEL);	
	}	
	list_for_each_entry(tmp_car_loop_counter, pHead, node)
	{
		ptr_free_array[i++] = tmp_car_loop_counter;
		pr_info("Got ptr_free_array[%d]:%p\n", i-1, ptr_free_array[i-1]);
	}

	for(i = 0; i < list_len; i++)
	{
		pr_info("calling list_del(&(ptr_free_array[%d]->node:%p));\n", i, ptr_free_array[i]);
		list_del(&(ptr_free_array[i]->node));
		pr_info("calling free() for above car\n");
		kfree(ptr_free_array[i]);
	}

	pr_info("Freeing the array pointer : ptr_free_array : %p\n", ptr_free_array);
	kfree(ptr_free_array);
	pr_info("Endof %s\n", __FUNCTION__);
}


/*
	Static Method
#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)
>> So notice here that the HEAD is also a type of "struct list_head"
#define LIST_HEAD_INIT(name) { &(name), &(name) }
*/
LIST_HEAD(HEAD_my_list_static); //this HEAD will point to first NODE

/*
	Dynamic Method
	struct list_head HEAD_my_list_dynamic;
	INIT_LIST_HEAD(&HEAD_my_list_dynamic);
		OR
	struct list_head *HEAD_ptr_my_list_dynamic;
	HEAD_ptr_my_list_dynamic = (struct list_head *)kzalloc(sizeof(struct list_head), GFP_KERNEL);
	INIT_LIST_HEAD(HEAD_ptr_my_list_dynamic);
*/

static int __init init_load(void)
{
	struct car *car_1;
	struct car *car_2;
	struct car *car_3;
	
	pr_info("------------TOP of INIT----------\n");
	car_1 = (struct car*)kzalloc(sizeof(struct car), GFP_KERNEL);	
	car_2 = (struct car*)kzalloc(sizeof(struct car), GFP_KERNEL);
	car_3 = (struct car*)kzalloc(sizeof(struct car), GFP_KERNEL);

	pr_info("calling INIT_LIST_HEAD() for each node\n");
	INIT_LIST_HEAD(&car_1->node);
	INIT_LIST_HEAD(&car_2->node);
	INIT_LIST_HEAD(&car_3->node);

	pr_info("car_1:%p\n", car_1);
	pr_info("car_2:%p\n", car_2);
	pr_info("car_3:%p\n", car_3);

	car_1->model_num = 1;
	car_1->color = STR_SILVER;
	car_1->name = STR_BALENO;
	
	car_2->model_num = 2;
	car_2->color = STR_BLACK;
	car_2->name = STR_VENTO;

	car_3->model_num = 3;
	car_3->color = STR_WHITE;
	car_3->name = STR_BOLERO;

	/*to add a node to the list HEAD
	void list_add(struct list_head *new, struct list_head *head);
	This will add to the HEAD - we can change it to add to TAIL:
	void list_add_tail(struct list_head *new, struct list_head *head);
	*/
	pr_info("Adding car_1, car_2 to List HEAD, using list_add()\n");
	list_add(&car_1->node, &HEAD_my_list_static);
	list_add(&car_2->node, &HEAD_my_list_static);

	pr_info("-------Priting the List HEAD------\n");
	print_list(&HEAD_my_list_static);
	pr_info("------END of Print----------------\n");

	pr_info("Adding car_3 to Tail - Bolero to Tail\n");
	list_add_tail(&car_3->node, &HEAD_my_list_static);	
	pr_info("------END of Print----------------\n");

	pr_info("Priting the List HEAD\n");
	print_list(&HEAD_my_list_static);
	pr_info("------END of Print----------------\n");

	pr_info("Deleting car_1\n");
	/*void list_del(struct list_head *entry);*/
	list_del(&car_1->node); //Notice the HEAD is not needed - as its a Double Linked list
	pr_info("car_1 is deleted - Now free the car_1\n");
	kfree(car_1);
	pr_info("------END of Print----------------\n");

	pr_info("Priting the List HEAD\n");
	print_list(&HEAD_my_list_static);
	pr_info("------END of Print----------------\n");

	pr_info("------------END of INIT----------\n");
	return 0;
}


static void __exit exit_load(void)
{
	pr_info("------------TOP of EXIT----------\n");
	pr_info("In Exit - Cars to free\n");
	pr_info("Priting the List HEAD\n");
	print_list(&HEAD_my_list_static);
	pr_info("------END of Print----------------\n");

	pr_info("calling free_cars()\n");
	free_cars(&HEAD_my_list_static);
	pr_info("All cars freed - again printing the List HEAD to check\n");
	
	pr_info("Priting the List HEAD\n");
	print_list(&HEAD_my_list_static);
	pr_info("------END of Print----------------\n");
	
	pr_info("------------END of EXIT----------\n");
}

module_init(init_load);
module_exit(exit_load);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Varun");
MODULE_DESCRIPTION("Linked List and Containerof");
MODULE_INFO(board, "Raspberry_PI_3_Model_B");


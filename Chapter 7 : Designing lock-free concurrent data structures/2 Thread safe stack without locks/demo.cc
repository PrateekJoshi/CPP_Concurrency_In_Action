/*
 * demo.cc
 *
 *  Created on: 01-Mar-2021
 *      Author: prateek
 *      Pg 184

 Push :
This code neatly matches the three-point plan from above: create a new node c, set the
node’s next pointer to the current head d, and set the head pointer to the new
node e . By populating the data in the node structure itself from the node constructor B ,
you’ve ensured that the node is ready to roll as soon as it’s constructed, so that’s the easy
problem down. Then you use compare_exchange_weak() to ensure that the head
pointer still has the same value as you stored in new_node->next d , and you set it to
new_node if so. This bit of code also uses a nifty part of the compare/exchange function-
ality: if it returns false to indicate that the comparison failed (for example, because
head was modified by another thread), the value supplied as the first parameter new_node->next )
is updated to the current value of head. You therefore don’t have to
reload head each time through the loop, because the compiler does that for you. Also,
because you’re just looping directly on failure, you can use compare_exchange_weak,
which can result in more optimal code than compare_exchange_strong on some archi-
tectures (see chapter 5).

 */
#include <atomic>

template<typename T>
class lock_free_stack
{
private:
	/* Node data type */
	struct node
	{
		T data;
		node* next;

		/* node constructor */
		node( T const& data_) : data(data_), next(nullptr)
		{}
	};

	/* Make head as atomic */
	std::atomic<node*> head;

public:
	void push(T const& data)
	{
		/* Create node to be pushed */
		node* const new_node = new node(data);

		/* Set node next to head */
		new_node->next = head.load();

		/* Use compare_exchange_weak() to ensure that the head pointer still has the same value
		 * as you stored in new_node->next d , and you set it to new_node if so. This bit of code also uses
		 * a nifty part of the compare/exchange functionality: if it returns false to indicate that
		 *  the comparison failed (for example, because head was modified by another thread), the
		 *  value supplied as the first parameter new_node->next ) is updated to the current value of head.
		 */
		while( !head.compare_exchange_weak(new_node->next, new_node));
	}
};





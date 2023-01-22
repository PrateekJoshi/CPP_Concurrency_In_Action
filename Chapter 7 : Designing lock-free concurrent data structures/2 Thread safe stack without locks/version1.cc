// Pg - 184
// The simplest stack is just a linked list; the head pointer identifies the first node
// (which will be the next to retrieve), and each node then points to the next node in turn.
// Under such a scheme, adding a node is relatively simple:
// 1	Create a new node.
// 2	Set its next pointer to the current head node.
// 3	Set the head node to point to it.

// This works fine in a single-threaded context, but if other threads are also modifying
// the stack, it’s not enough. Crucially, if two threads are adding nodes, there’s a race
// condition between steps 2 and 3: a second thread could modify the value of head
// between when your thread reads it in step 2 and you update it in step 3. This would
// then result in the changes made by that other thread being discarded or even worse
// consequences. Before we look at addressing this race condition, it’s also important to
// note that once head has been updated to point to your new node, another thread
// could read that node. It’s therefore vital that your new node is thoroughly prepared
// before head is set to point to it; you can’t modify the node afterward.

// OK, so what can you do about this nasty race condition? The answer is to use an
// atomic compare/exchange operation at step 3 to ensure that head hasn’t been modi-
// fied since you read it in step 2. If it has, you can loop and try again. The following list-
// ing shows how you can implement a thread-safe push() without locks.

#include <atomic>
template<typename T>
class lock_free_stack
{
private:
	struct node
	{
		T data;
		node* next;

		node(T const& data_) : data(data_)
		{}
	};
	std::atomic<node*> head;
public:
	void push(T const& data)
	{
		node* const new_node = new node(data);
		new_node->next = head.load();
		while( !head.compare_exchange_weak(new_node->next, new_node) );
	}

	void pop(T& result)
	{
		node* old_head = head.load();
		while( !head.compare_exchange_weak(old_head, old_head->next));
		result = old_head->data;
	}


};

// Push : 
// You use compare_exchange_weak() to ensure that the head
// pointer still has the same value as you stored in new_node->next d , and you set it to
// new_node if so. This bit of code also uses a nifty part of the compare/exchange function-
// ality: if it returns false to indicate that the comparison failed (for example, because
// head was modified by another thread), the value supplied as the first parameter
// (new_node->next ) is updated to the current value of head. You therefore don’t have to
// reload head each time through the loop, because the compiler does that for you. Also,
// because you’re just looping directly on failure, you can use compare_exchange_weak,
// which can result in more optimal code than compare_exchange_strong on some archi-
// tectures 

// Pop :
// Of course, now that you have a means of adding data to the stack, you need a way
// of getting it off again. On the face of it, this is quite simple:
// 1) Read the current value of head.
// 2) Read head->next.
// 3) Set head to head->next .
// 4) Return the data from the retrieved node.
// 5) Delete the retrieved node.

// However, in the presence of multiple threads, this isn’t so simple. If there are two
// threads removing items from the stack, they both might read the same value of head at
// step 1. If one thread then proceeds all the way through to step 5 before the other gets
// to step 2, the second thread will be dereferencing a dangling pointer. This is one of
// the biggest issues in writing lock-free code, so for now you’ll just leave out step 5 and
// leak the nodes.

// This doesn’t resolve all the problems, though. There’s another problem: if two
// threads read the same value of head, they’ll return the same node. This violates the
// intent of the stack data structure, so you need to avoid this. You can resolve this the same
// way you resolved the race in push(): use compare/exchange to update head. If the
// compare/exchange fails, either a new node has been pushed on or another thread
// just popped the node you were trying to pop. Either way, you need to return to step 1
// (although the compare/exchange call rereads head for you).

// Once the compare/exchange call succeeds, you know you’re the only thread that’s
// popping the given node off the stack, so you can safely execute step 4. 
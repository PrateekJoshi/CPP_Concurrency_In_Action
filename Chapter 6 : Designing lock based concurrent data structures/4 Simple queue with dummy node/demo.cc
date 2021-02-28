/*
 * demo.cc
 *
 *  Created on: 27-Feb-2021
 *      Author: prateek
 *      Pg 160
The changes to try_pop() are fairly minimal. First, you’re comparing head against
tail d rather than checking for NULL, because the dummy node means that head is
never NULL. Because head is a std::unique_ptr<node>, you need to call head.get()
to do the comparison. Second, because the node now stores the data by pointer B,
you can retrieve the pointer directly e rather than having to construct a new instance
of T . The big changes are in push(): you must first create a new instance of T on the
heap and take ownership of it in a std::shared_ptr<> h (note the use of
std::make_shared to avoid the overhead of a second memory allocation for the refer-
ence count). The new node you create is going to be the new dummy node, so you
don’t need to supply the new_value to the constructor i. Instead, you set the data on
the old dummy node to your newly allocated copy of the new_value j. Finally, in
order to have a dummy node, you have to create it in the constructor c.

By now, I’m sure you’re wondering what these changes buy you and how they help
with making the queue thread-safe. Well, push() now accesses only tail, not head,
which is an improvement. try_pop() accesses both head and tail, but tail is
needed only for the initial comparison, so the lock is short-lived. The big gain is that
the dummy node means try_pop() and push() are never operating on the same
node, so you no longer need an overarching mutex. So, you can have one mutex for
head and one for tail. Where do you put the locks?

You’re aiming for the maximum opportunities for concurrency, so you want to
hold the locks for the smallest possible length of time. push() is easy: the mutex needs
to be locked across all accesses to tail, which means you lock the mutex after the new
node is allocated i and before you assign the data to the current tail node j. The
lock then needs to be held until the end of the function.

try_pop() isn’t so easy. First off, you need to lock the mutex on head and hold it
until you’re finished with head. In essence, this is the mutex to determine which
thread does the popping, so you want to do that first. Once head is changed f, you
can unlock the mutex; it doesn’t need to be locked when you return the result g.
That leaves the access to tail needing a lock on the tail mutex. Because you need to
access tail only once, you can just acquire the mutex for the time it takes to do the
read. This is best done by wrapping it in a function. In fact, because the code that
needs the head mutex locked is only a subset of the member, it’s clearer to wrap that in
a function too. The final code is shown here.
*/
#include <memory>
template<typename T>
class queue
{
	struct node
	{
		std::shared_ptr<T> data;
		std::unique_ptr<node> next;
	};

	std::unique_ptr<node> head;
	node* tail;

public:
	queue() : head(new node), tail(head.get())
	{}

	queue(const queue& other) = delete;
	queue& operator=(const queue& other) = delete;

	std::shared_ptr<T> try_pop()
	{
		// can't compared unique_ptr directly with raw pointer , so use get()
		if( head.get() == tail )
		{
			return std::shared_ptr<T>();
		}

		std::shared_ptr<T> const res(head->data);
		std::unique_ptr<node> const old_head = std::move(head);
		head = std::move(old_head->next);
		return res;
	}

	void push(T new_value)
	{
		std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
		std::unique_ptr<node> p(new node);
		tail->data = new_data;
		node* const new_tail = p.get();
		tail->next = std::move(p);
		tail = new_tail;
	}

};





/*
 * demo.cc
 *
 *  Created on: 27-Feb-2021
 *      Author: prateek
 *      Pg 158
First off, note that listing 6.4 uses std::unique_ptr<node> to manage the nodes,
because this ensures that they (and the data they refer to) get deleted when they’re no
longer needed, without having to write an explicit delete. This ownership chain is
managed from head, with tail being a raw pointer to the last node.
Although this implementation works fine in a single-threaded context, a couple of
things will cause you problems if you try to use fine-grained locking in a multi-
threaded context. Given that you have two data items (head B and tail c), you
could in principle use two mutexes, one to protect head and one to protect tail, but
there are a couple of problems with that.

The most obvious problem is that push() can modify both head f and tail g,
so it would have to lock both mutexes. This isn’t too much of a problem, although
it’s unfortunate, because locking both mutexes would be possible. The critical prob-
lem is that both push() and pop() access the next pointer of a node: push() updates
tail->next e, and try_pop() reads head->next d. If there’s a single item in the
queue, then head==tail, so both head->next and tail->next are the same object,
which therefore requires protection. Because you can’t tell if it’s the same object with-
out reading both head and tail, you now have to lock the same mutex in both push()
and try_pop(), so you’re no better off than before. Is there a way out of this dilemma?
 */
#include <memory>
template<typename T>
class queue
{
private:
	struct node
	{
		T data;
		std::unique_ptr<node> next;

		node(T data_) : data(std::move(data_))
		{}
	};

	std::unique_ptr<node> head;
	node *tail;


public:
	queue() : tail(nullptr)
	{}

	queue(const queue& other) = delete;
	queue& operator= (const queue& other) = delete;

	std::shared_ptr<T> try_pop()
	{
		if( !head )
		{
			return std::shared_ptr<T>();
		}

		std::shared_ptr<T> const res(std::make_shared<T>(std::move(head->data)));
		std::unique_ptr<node> const old_head = std::move(head);

		head = std::move(old_head->next);
		if( !head )
		{
			tail = nullptr;
		}

		return res;
	}

	void push(T new_value)
	{
		std::unique_ptr<node> p(new node(std::move(new_value)));
		node* const new_tail = p.get();

		if(tail)
		{
			tail->next = std::move(p);
		}
		else
		{
			head = std::move(p);
		}

		tail = new_tail;
	}
};





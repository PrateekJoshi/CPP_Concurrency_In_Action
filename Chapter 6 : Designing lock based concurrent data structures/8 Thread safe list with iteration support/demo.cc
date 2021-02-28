/*
 * demo.cc
 *
 *  Created on: 28-Feb-2021
 *      Author: prateek
 *      Pg 176
 The basic idea with fine-grained locking for a linked list is to have one mutex per
node. If the list gets big, that’s a lot of mutexes! The benefit here is that operations on
separate parts of the list are truly concurrent: each operation holds only the locks on
the nodes it’s actually interested in and unlocks each node as it moves on to the next.
The next listing shows an implementation of just such a list.
 */
#include <memory>
#include <mutex>

template<typename T>
class threadsafe_list
{
	/* A node type */
	struct node
	{
		std::mutex m;
		std::shared_ptr<T> data;		// data is a pointer
		std::unique_ptr<node> next;		// pointer to next node

		/* Constructor */
		node() : next()
		{}

		/* Constructor */
		node(T const& value) : data(std::make_shared<T>(value))
		{}
	};

	//Head node
	node head;

public:
	/* Constructor */
	threadsafe_list()
	{}

	/* Destructor */
	~threadsafe_list()
	{
		remove_if([](T const&) { return true; } );
	}

	/* Delete copy constructor and assigment operator */
	threadsafe_list(threadsafe_list const& other) = delete;
	threadsafe_list& operator = (threadsafe_list const& other) = delete;

	/* Push an item to front of list */
	void push_front(T const& value)
	{
		/* Allocate memory on heap and create a node
		 * Done before taking a lock ( as it is a time consuming operation )
		 */
		std::unique_ptr<node> new_node(new node(value));

		/* Take lock on head node */
		std::lock_guard<std::mutex> lk(head.m);

		/* Make new_node as new head
		   std::move() is kind of a ownership transfer of pointer
		 */
		new_node->next = std::move(head.next);

		/* Set new head */
		head.next = std::move(new_node);
	}

	/*
	 * Apply a function for each node in linked list
	 */
	template<typename Function>
	void for_each(Function f)
	{
		// Store address of head in current
		node* current = &head;

		/* Acquire lock on head */
		std::unique_lock<std::mutex> lk(head.m);

		/* While next is not null
		 * NOTE: Here we have used .get() on unique_ptr as we dont want to transfer ownership of pointer
		 * */
		while( node *next = current->next.get() )
		{
			/* Acquire lock on next node to current */
			std::unique_lock<std::mutex> next_lk(next->m);

			/* Release previous node lock */
			lk.unlock();

			/* Call the function on next node */
			f(*next->data);

			/* Make next as current */
			current = next;

			/* Transfer ownership of next_lk to lk ( so that we can unlock it on next iteration once its next node lock is acquired ) */
			lk = std::move(next_lk);
		}
	}

	/* Find and return a data if a predicate holds true for a node */
	template<typename Predicate>
	std::shared_ptr<T> find_first_if(Predicate p)
	{
		/* Get address of head */
		node* current = &head;

		/* Acquire lock on head */
		std::unique_lock<std::mutex> lk(head.m);

		/* While next is not null
		 * NOTE: Here we have used .get() on unique_ptr as we dont want to transfer ownership of pointer
		 */
		while( node* const next = current->next.get() )
		{
			/* Acquire lock on next */
			std::unique_lock<std::mutex> next_lk(next->m);

			/* Remove lock on current */
			lk.unlock();

			/* If predicate is true */
			if( p(*next->data) )
			{
				/* return the data */
				return next->data;
			}

			/* Make current as next */
			current = next;

			/* Transfer ownership of next_lk to lk ( so that we can unlock it on next iteration once its next node lock is acquired ) */
			lk = std::move(next_lk);
		}

		// If predicate could not succeed, return nullptr
		return std::shared_ptr<T>();
	}

	/*
	 * Remove node if predicate holds true
	 */
	template<typename Predicate>
	void remove_if(Predicate p)
	{
		/* Get address of head */
		node* current = &head;

		/* Acquire lock on head */
		std::unique_lock<std::mutex> lk(head.m);

		/* While next is not null
		 * NOTE: Here we have used .get() on unique_ptr as we dont want to transfer ownership of pointer
		 */
		while (node *const next = current->next.get())
		{
			/* Acquire lock on next */
			std::unique_lock<std::mutex> next_lk(next->m);

			/* If predicate is true . NOTE we have not removed the lock till now on current node */
			if( p(*next->data) )
			{
				// Get ownership of old next ( node to be deleted )
				std::unique_ptr<node> old_next = std::move(current->next);

				// Make current->next pointer as owner of next->next pointer (skipped next node) */
				current->next = std::move(next->next);

				// Now we successfuly update all pointers  ( unlock next node lock )
				next_lk.unlock();
			}
			else
			{
				// Unlock next node lock
				lk.unlock();

				// Make current as next
				current = next;

				//Transfer ownership of next_lk to lk
				lk = std::move(next_lk);
			}
		}
	}

};





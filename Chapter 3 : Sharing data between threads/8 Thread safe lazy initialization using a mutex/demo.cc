/*
 * demo.cc
 *
 *  Created on: 08-Feb-2021
 *      Author: prateek
 *      Pg : 60
 *
Suppose you have a shared resource that’s so expensive to construct that you want to
do so only if it’s actually required; maybe it opens a database connection or allocates a
lot of memory. Lazy initialization such as this is common in single-threaded code—
each operation that requires the resource first checks to see if it has been initialized
and then initializes it before use if not:
 *
 */
#include <memory>
#include <mutex>

struct some_resource
{
	void do_something(){}
};

std::shared_ptr<some_resource> resource_ptr;
std::mutex resource_mutex;

void foo()
{
	std::unique_lock<std::mutex> unq_lock(resource_mutex);		// All threads are serialized here
	if( !resource_ptr )
	{
		// Only initialization needs protection
		resource_ptr.reset(new some_resource);
	}
	unq_lock.unlock();
	resource_ptr->do_something();
}




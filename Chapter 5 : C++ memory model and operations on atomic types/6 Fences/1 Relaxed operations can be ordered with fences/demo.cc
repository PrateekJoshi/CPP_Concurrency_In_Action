/*
 * demo.cc
 *
 *  Created on: 14-Feb-2021
 *      Author: prateek
 *      Pg 143
 An atomic operations library wouldn’t be complete without a set of fences. These are
operations that enforce memory-ordering constraints without modifying any data and
are typically combined with atomic operations that use the memory_order_relaxed
ordering constraints. Fences are global operations and affect the ordering of other
atomic operations in the thread that executed the fence. Fences are also commonly
called memory barriers, and they get their name because they put a line in the code that
certain operations can’t cross. As you may recall from section 5.3.3, relaxed operations
on separate variables can usually be freely reordered by the compiler or the hardware.
Fences restrict this freedom and introduce happens-before and synchronizes-with
relationships that weren’t present before.

The release fence c synchronizes-with the acquire fence f, because the load from y
at e reads the value stored at d. This means that the store to x at B happens-before
the load from x at g, so the value read must be true and the assert at h won’t fire.
This is in contrast to the original case without the fences where the store to and load
from x weren’t ordered, and so the assert could fire. Note that both fences are neces-
sary: you need a release in one thread and an acquire in another to get a synchronizes-
with relationship.
In this case, the release fence c has the same effect as if the store to y d was
tagged with memory_order_release rather than memory_order_relaxed. Likewise, the
acquire fence f makes it as if the load from y e was tagged with memory_order_
acquire. This is the general idea with fences: if an acquire operation sees the result of
a store that takes place after a release fence, the fence synchronizes-with that acquire
operation; and if a load that takes place before an acquire fence sees the result of a
release operation, the release operation synchronizes-with the acquire fence. Of
course, you can have fences on both sides, as in the example here, in which case if a
load that takes place before the acquire fence sees a value written by a store that takes
place after the release fence, the release fence synchronizes-with the acquire fence.
 */
#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x,y;
std::atomic<int> z;


void write_x_then_y()
{
	x.store(true, std::memory_order_relaxed);
	std::atomic_thread_fence(std::memory_order_release);
	y.store(true, std::memory_order_relaxed);
}
void read_y_then_x()
{
	while( !y.load(std::memory_order_relaxed) );
	std::atomic_thread_fence(std::memory_order_acquire);
	if( x.load(std::memory_order_relaxed) )
	{
		++z;
	}
}

int main(int argc, char **argv) {
	x = false;
	y = false;
	z = 0;

	std::thread a(write_x_then_y);
	std::thread b(read_y_then_x);

	a.join();
	b.join();

	assert(z.load() != 0 );
}






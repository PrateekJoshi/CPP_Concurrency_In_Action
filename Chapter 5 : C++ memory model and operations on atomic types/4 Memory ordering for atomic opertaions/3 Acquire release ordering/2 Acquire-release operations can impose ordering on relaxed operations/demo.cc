/*
 * demo.cc
 *
 *  Created on: 14-Feb-2021
 *      Author: prateek
 *      Pg 134
 In order to see the benefit of acquire-release ordering, you need to consider two
stores from the same thread, like in listing 5.5. If you change the store to y to use
memory_order_release and the load from y to use memory_order_acquire like in the
following listing, then you actually impose an ordering on the operations on x.

 Eventually, the load from y d will see true as written by the store c. Because the
store uses memory_order_release and the load uses memory_order_acquire, the store
synchronizes-with the load. The store to x B happens-before the store to y c,
because they’re in the same thread. Because the store to y synchronizes-with the load
from y , the store to x also happens-before the load from y and by extension happens-
before the load from x e. Thus the load from x must read true, and the assert f can’t
fire. If the load from y wasn’t in a while loop, this wouldn’t necessarily be the case; the
load from y might read false, in which case there’d be no requirement on the value
read from x. In order to provide any synchronization, acquire and release operations
must be paired up. The value stored by a release operation must be seen by an acquire
operation for either to have any effect. If either the store at c or the load at d was a
relaxed operation, there’d be no ordering on the accesses to x, so there’d be no guar-
antee that the load at e would read true, and the assert could fire.
 */
#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x,y;
std::atomic<int> z;

void write_x_then_y()
{
	x.store(true, std::memory_order_relaxed);
	y.store(true, std::memory_order_release);
}

void read_y_then_x()
{
	while( !y.load(std::memory_order_acquire) );
	if( x.load(std::memory_order_relaxed) )
	{
		++z;
	}
}

int main(int argc, char **argv) {
	x = false;
	y = false;
	z = 0 ;
	std::thread a(write_x_then_y);
	std::thread b(read_y_then_x);
	a.join();
	b.join();
	assert( z.load() != 0 );
}
































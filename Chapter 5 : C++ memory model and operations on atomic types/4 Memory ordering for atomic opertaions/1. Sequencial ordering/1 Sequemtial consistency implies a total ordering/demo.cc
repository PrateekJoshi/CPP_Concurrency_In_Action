/*
 * demo.cc
 *
 *  Created on: 13-Feb-2021
 *      Author: prateek
 *      Pg 125
The assert f can never fire, because either the store to x B or the store to y c must
happen first, even though it’s not specified which. If the load of y in read_x_then_y d
returns false , the store to x must occur before the store to y , in which case the load of x
in read_y_then_x e must return true, because the while loop ensures that the y is true
at this point. Because the semantics of memory_order_seq_cst require a single total
ordering over all operations tagged memory_order_seq_cst, there’s an implied ordering
relationship between a load of y that returns false d and the store to y B . For there to
be a single total order, if one thread sees x==true and then subsequently sees y==false,
this implies that the store to x occurs before the store to y in this total order.
Of course, because everything is symmetrical, it could also happen the other way
around, with the load of x e returning false, forcing the load of y d to return true.
In both cases, z is equal to 1. Both loads can return true, leading to z being 2, but
under no circumstances can z be zero.

Sequential consistency is the most straightforward and intuitive ordering, but it’s
also the most expensive memory ordering because it requires global synchronization
between all threads. On a multiprocessor system this may require quite extensive and
time-consuming communication between processors.

In order to avoid this synchronization cost, you need to step outside the world of
sequential consistency and consider using other memory orderings.
 */
#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x,y;
std::atomic<int> z;

void write_x()
{
	x.store(true, std::memory_order_seq_cst);
}

void write_y()
{
	y.store(true, std::memory_order_seq_cst);
}

void read_x_then_y()
{
	while(!x.load(std::memory_order_seq_cst) );

	if( y.load(std::memory_order_seq_cst) )
	{
		++z;
	}
}

void read_y_then_x()
{
	while(!y.load(std::memory_order_seq_cst));
	if( x.load(std::memory_order_seq_cst) )
	{
		++z;
	}
}


int main(int argc, char **argv) {
	x = false;
	y = false;
	z = 0 ;

	std::thread a(write_x);
	std::thread b(write_y);
	std::thread c(read_x_then_y);
	std::thread d(read_y_then_x);

	a.join();
	b.join();
	c.join();
	d.join();

	assert( z.load() != 0 );		// never fail : z can be 1 or 2 never be 0

	return 0;
}































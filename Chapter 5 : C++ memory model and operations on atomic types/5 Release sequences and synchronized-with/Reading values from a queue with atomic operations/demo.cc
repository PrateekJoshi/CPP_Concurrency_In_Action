/*
 * demo.cc
 *
 *  Created on: 14-Feb-2021
 *      Author: prateek
 *      Pg 141
If the store is tagged with memory_order_release, memory_order_acq_rel, or memory_order_
seq_cst , and the load is tagged with memory_order_consume, memory_order_acquire,
or memory_order_seq_cst, and each operation in the chain loads the value written
by the previous operation, then the chain of operations constitutes a release sequence
and the initial store synchronizes-with (for memory_order_acquire or memory_order_
seq_cst ) or is dependency-ordered-before (for memory_order_consume) the final
load. Any atomic read-modify-write operations in the chain can have any memory
ordering (even memory_order_relaxed ).
To see what this means and why it’s important, consider an atomic<int> being
used as a count of the number of items in a shared queue, as in the following listing.

One way to handle things would be to have the thread that’s producing the data
store the items in a shared buffer and then do count.store(number_of_items,
memory_order_release) B to let the other threads know that data is available. The
threads consuming the queue items might then do count.fetch_sub(1,memory_
order_acquire) c to claim an item from the queue, prior to actually reading the
shared buffer e. Once the count becomes zero, there are no more items, and the thread
must wait d.

 */

#include <atomic>
#include <thread>
#include <vector>
std::vector<int> queue_data;
std::atomic<int> count;

void wait_for_more_items() {}
void process(int data){}

void populate_queue()
{
    unsigned const number_of_items=20;
    queue_data.clear();
    for(unsigned i=0;i<number_of_items;++i)
    {
        queue_data.push_back(i);
    }

    count.store(number_of_items,std::memory_order_release);
}

void consume_queue_items()
{
    while(true)
    {
        int item_index;
        if((item_index=count.fetch_sub(1,std::memory_order_acquire))<=0)
        {
            wait_for_more_items();
            continue;
        }
        process(queue_data[item_index-1]);
    }
}

int main()
{
    std::thread a(populate_queue);
    std::thread b(consume_queue_items);
    std::thread c(consume_queue_items);
    a.join();
    b.join();
    c.join();
}



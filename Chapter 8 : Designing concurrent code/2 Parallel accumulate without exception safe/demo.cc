/*
 * demo.cc
 *
 *  Created on: 06-Mar-2021
 *      Author: prateek
 *      Pg 244
First up, you have the call to distance c, which performs operations on the user-
supplied iterator type. Because you haven’t yet done any work, and this is on the call-
ing thread, it’s fine. Next up, you have the allocation of the results vector d and the
threads vector e. Again, these are on the calling thread, and you haven’t done any
work or spawned any threads, so this is fine. Of course, if the construction of threads
throws, the memory allocated for results will have to be cleaned up, but the destruc-
tor will take care of that for you.

Skipping over the initialization of block_start f because that’s similarly safe, you
come to the operations in the thread-spawning loop g, h, i. Once you’ve been
through the creation of the first thread at h, you’re in trouble if you throw any excep-
tions; the destructors of your new std::thread objects will call std::terminate and
abort your program. This isn’t a good place to be.

The call to accumulate_block j can potentially throw, with similar consequences;
your thread objects will be destroyed and call std::terminate. On the other hand,
the final call to std::accumulate 1) can throw without causing any hardship, because
all the threads have been joined by this point.

That’s it for the main thread, but there’s more: the calls to accumulate_block on the
new threads might throw at B. There aren’t any catch blocks, so this exception will be
left unhandled and cause the library to call std::terminate() to abort the application.
In case it’s not glaringly obvious, this code isn’t exception-safe.
 */

#include <algorithm>
#include <functional>
#include <thread>
#include <vector>

template<typename Iterator,typename T>

struct accumulate_block
{
    void operator()(Iterator first,Iterator last,T& result)
    {
        result=std::accumulate(first,last,result);
    }
};

template<typename Iterator,typename T>
T parallel_accumulate(Iterator first,Iterator last,T init)
{
    unsigned long const length=std::distance(first,last);

    if(!length)
        return init;

    unsigned long const min_per_thread=25;
    unsigned long const max_threads=
        (length+min_per_thread-1)/min_per_thread;

    unsigned long const hardware_threads=
        std::thread::hardware_concurrency();

    unsigned long const num_threads=
        std::min(hardware_threads!=0?hardware_threads:2,max_threads);

    unsigned long const block_size=length/num_threads;

    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads-1);

    Iterator block_start=first;
    for(unsigned long i=0;i<(num_threads-1);++i)
    {
        Iterator block_end=block_start;
        std::advance(block_end,block_size);
        threads[i]=std::thread(
            accumulate_block<Iterator,T>(),
            block_start,block_end,std::ref(results[i]));
        block_start=block_end;
    }
    accumulate_block<Iterator,T>()(block_start,last,results[num_threads-1]);

    std::for_each(threads.begin(),threads.end(),
                  std::mem_fn(&std::thread::join));

    return std::accumulate(results.begin(),results.end(),init);
}



/*
 * demo.cc
 *
 *  Created on: 14-Mar-2021
 *      Author: prateek
 *      Pg 279
 */

#include <vector>
#include <future>
#include <thread>
#include "thread_pool.cc"

template<typename Iterator,typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    unsigned long const length=std::distance(first,last);

    if(!length)
    {
        return init;
    }

    unsigned long const block_size=25;

    /*
     * 1. You are working on no of blocks to use rather than no of threads.
     * 2. NOTE : If you choose too small a task size, the code may run more slowly than with one thread.
     */
    unsigned long const num_blocks=(length+block_size-1)/block_size;
    std::vector<std::future<T>> futures(num_blocks-1);
    thread_pool pool;

    Iterator block_start = first;
    for( unsigned long i = 0 ; i < ( num_blocks-1) ; i++ )
    {
    	Iterator block_end = block_start;
    	std::advance(block_end, block_size);
    	futures[i] = pool.submit(accumulate_block<Iterator,T>());
    	block_start = block_end;
    }

    T last_result = accumulate_block<Iterator,T>()(block_size,last);
    T result = init;

    for( unsigned long i = 0 ; i < ( num_blocks-1) ; i++ )
    {
    	result += futures[i].get();
    }

    result += last_result;
    return result;
}





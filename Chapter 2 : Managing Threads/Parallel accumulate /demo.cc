/*
 * Pg -29
 */
#include <thread>
#include <numeric>
#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>

template<typename Iterator, typename T>
struct accumulate_block
{
	void operator ()(Iterator first, Iterator last, T &result)
	{
		result = std::accumulate(first, last, result);
	}
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
	unsigned long const length = std::distance(first, last);

	if( !length )
	{
		return init;
	}

	unsigned long const min_per_thread = 25;
	unsigned long const max_threads = (length + min_per_thread-1)/min_per_thread;
	unsigned long const hardware_threads = std::thread::hardware_concurrency();

	std::cout<<"Debug no of hardware threads : "<< hardware_threads<<std::endl;

	unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

	unsigned long const block_size = length / num_threads;

	std::vector<T> results(num_threads);
	std::vector<std::thread> threads(num_threads-1);		// 1 is main thread

	Iterator block_start = first;
	for( unsigned long i = 0 ; i < num_threads-1 ; i++ )
	{
		Iterator block_end = block_start;
		std::advance(block_end, block_size);	// adnavce block_end itr by block_size


		/* Start thread by running acccumulate in diff data range to get intermediate results */
		threads[i] = std::thread(accumulate_block<Iterator,T>(), block_start, block_end, std::ref(results[i]));

		block_start = block_end;
	}

	/* Do accumulate of last one in case , data length is not a multiple of num_threeads
	 * block_size = length / num_threads;
	 */
	accumulate_block<Iterator,T>()(block_start,last, results[num_threads-1]);

	/* Wait for each parallel thread to finish */
	std::for_each( threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

	return std::accumulate(results.begin(), results.end(), init);
}

int main(int argc, char **argv) {
	std::vector<int> vi;
	for( int i = 0 ; i < 100 ; i++ )
	{
		vi.push_back(10);
	}

	int sum = parallel_accumulate(vi.begin(), vi.end(), 0);

	std::cout<<"sum="<<sum<<std::endl;
}

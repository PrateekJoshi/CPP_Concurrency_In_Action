/*
 * demo.cc
 *
 *  Created on: 14-Mar-2021
 *      Author: prateek
 *      Pg 283
 */

#include <algorithm>
#include <deque>
#include <future>
#include <memory>
#include <queue>
#include <thread>
#include <type_traits>

class thread_pool
{
	thread_safe_queue<funtion_wrapper> pool_work_queue;

	/* local queue can be a plain std::queue<> because it's only ever accessed by one thread */
	typedef std::queue<function_wrapper> local_queue_type;

	/*
	 * std::unique_ptr<> to hold the thread-local work queue
	 */
	static thread_local std::unique_ptr<local_queue_type> local_work_queue;

	void worker_thread()
	{
		// Local thread queue is initialized before processing step
		local_work_queue.reset(new local_queue_type);

		while( !done )
		{
			run_pending_task();
		}
	}

public:
	template<typename FunctionType>
	std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f)
	{
		typedef typename std::result_of<FunctionType()>::type result_type;
		std::packaged_task<result_type()> task(f);
		std::future<result_type> result(task.get_future());
		if(local_work_queue)
		{
			local_work_queue->push(std::move(task));
		}
		else
		{
			pool_work_queue.push(std::move(task));
		}

		return result;
	}

	void run_pending_task()
	{
		function_wrapper task;

		/*
		 * Check to see if there are any items on the local queue. If there are , you can take the
		 * front one and process it.
		 */
		if( local_work_queue && !local_work_queue->empty )
		{
			task = std::move(local_work_queue->front());
			local_work_queue->pop();
			task();
		}
		else if( pool_work_queue.try_pop(task))
		{
			/*
			 * If there are no tasks on the local queue, you try the pool queue as before
			 */
			task();
		}
		else
		{
			std::this_thread::yield();
		}
	}

	// NOTE : rest as before thread pool implementation
};





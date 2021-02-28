/*
 * demo.cc
 *
 *  Created on: 28-Feb-2021
 *      Author: prateek
 *      Pg 171
Assuming a fixed number of buckets, which bucket a
key belongs to is purely a property of the key and its hash function. This means you
can safely have a separate lock per bucket. If you again use a mutex that supports mul-
tiple readers or a single writer, you increase the opportunities for concurrency N-fold,
where N is the number of buckets. The downside is that you need a good hash func-
tion for the key. The C++ Standard Library provides the std::hash<> template, which
you can use for this purpose. It’s already specialized for the fundamental types such as
int and common library types such as std::string , and the user can easily specialize
it for other key types. If you follow the lead of the standard unordered containers and
take the type of the function object to use for doing the hashing as a template param-
eter, the user can choose whether to specialize std::hash<> for their key type or pro-
vide a separate hash function.

 This implementation uses a std::vector<std::unique_ptr<bucket_type>> g to
hold the buckets, which allows the number of buckets to be specified in the construc-
tor. The default is 19, which is an arbitrary prime number; hash tables work best with a
prime number of buckets. Each bucket is protected with an instance of boost::
shared_mutex B to allow many concurrent reads or a single call to either of the mod-
ification functions per bucket.

Because the number of buckets is fixed, the get_bucket() function h can be
called without any locking i, j, 1), and then the bucket mutex can be locked either
for shared (read-only) ownership d or unique (read/write) ownership e, f as
appropriate for each function.

All three functions make use of the find_entry_for() member function c on the
bucket to determine whether the entry is in the bucket. Each bucket contains just a
std::list<> of key/value pairs, so adding and removing entries is easy.
I’ve already covered the concurrency angle, and everything is suitably protected
with mutex locks, so what about exception safety? value_for doesn’t modify anything,
so that’s fine; if it throws an exception, it won’t affect the data structure.
remove_mapping modifies the list with the call to erase, but this is guaranteed not to
throw, so that’s safe. This leaves add_or_update_mapping, which might throw in either
of the two branches of the if. push_back is exception-safe and will leave the list in the
original state if it throws, so that branch is fine. The only problem is with the assign-
ment in the case where you’re replacing an existing value; if the assignment throws,
you’re relying on it leaving the original unchanged. However, this doesn’t affect the
data structure as a whole and is entirely a property of the user-supplied type, so you
can safely leave it up to the user to handle this.
*/

#include <cstddef>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <utility>
#include <vector>


template<typename Key, typename Value, typename Hash=std::hash<Key>>
class threadsafe_lookup_table
{
private:
	class bucket_type
	{
	private:
		/* Define some typedef to shorter naming of types afterwards */
		typedef std::pair<Key,Value> bucket_value;
		typedef std::list<bucket_value> bucket_data;
		typedef typename bucket_data::iterator bucket_itertaor;

		/* A list of <key,value> pairs */
		bucket_data data;

		/* Shared mutex for data */
		mutable std::shared_mutex mutex;

		bucket_itertaor find_entry_for(Key const& key) const
		{
			return std::find_if(data.begin(), data.end(),
					[&](bucket_value const& item)
					{
						return item.first == key;
					});
		}

	public:
		/* Return the value for key in data . If not present then return default value */
		Value value_for(Key const& key, Value const& default_value) const
		{
			//Get the shared lock
			std::shared_lock<std::shared_mutex> lock(mutex);

			/* Find the entryt in data for key */
			bucket_itertaor const found_entry = find_entry_for(key);

			/* If found then return value else return default_value */
			return ( found_entry == data.end() ) ? default_value : found_entry->second;
 		}

		/* Add or Update entry in data for specified key with specified value */
		void add_or_update_mapping(Key const& key , Value const& value )
		{
			/* Get the shared lock before updating data*/
			std::unique_lock<std::shared_mutex> lock(mutex);

			/* Get the iterator entry to update */
			bucket_itertaor const found_entry = find_entry_for(key);

			/* If key is already not present */
			if( found_entry == data.end() )
			{
				data.push_back(bucket_value(key,value));
			}
			else
			{
				/* Update the value for the key */
				found_entry->second = value;
			}
		}

		/* Remove a key from data */
		void remove_mapping(Key const& key)
		{
			/* Get the shared lock before updating data*/
			std::unique_lock<std::shared_mutex> lock(mutex);

			/* Get the iterator entry to remove */
			bucket_itertaor const found_entry = find_entry_for(key);

			/* If key is present then delete */
			if( found_entry != data.end() )
			{
				data.erase(found_entry);
			}
		}
	};

	/* Define a list of buckets
	 *
	 *  | ptr	|  ptr	|  ptr  |  ptr 	|
	 *     |
	 *     V
	 *   bucket
	 *    []
	 *    []
	 *    []
	 *
	 *
	 */
	std::vector<std::unique_ptr<bucket_type>> buckets;

	/* Define a hasher */
	Hash hasher;

	/* Get the index of the bucket from the key
	 * A hash function to get index of bucket based on key
	 */
	bucket_type& get_bucket(Key const& key ) const
	{
		std::size_t const bucket_index = hasher(key) % buckets.size();
		return *buckets[bucket_index];

	}

public:
	/* Declare some typedefs */
	typedef Key key_type;
	typedef Value mapped_type;
	typedef Hash hash_type;

	/* Constructor */
	threadsafe_lookup_table(unsigned int num_buckets = 19, Hash const& hasher_ = Hash() )
		: buckets(num_buckets) , hasher(hasher_)
	{
		for( unsigned int i = 0 ; i < num_buckets ; i++  )
		{
			/* Initialize bucket_type pointer (ptr) for each entry in buckets vector */
			buckets[i].reset(new bucket_type);
		}
	}

	/* Delete copy constructor */
	threadsafe_lookup_table(threadsafe_lookup_table const& other ) = delete;

	/* Delete assignment operator */
	threadsafe_lookup_table& operator = (threadsafe_lookup_table const& other) = delete;

	/* Return the value for a key , if key not present then return default value */
	Value value_for(Key const& key, Value const& default_value = Value() ) const
	{
		return get_bucket(key).value_for(key, default_value);
	}

	/* Add or Update entry in data for specified key with specified value */
	void add_or_update_mapping(Key const& key, Value const& value)
	{
		get_bucket(key).add_or_update_mapping(key,value);
	}

	/* Remove a key from lookup table */
	void remove_mapping(Key const& key )
	{
		get_bucket(key).remove_mapping(key);
	}

	/* Get snapshot of the all the key,value in the buckets
	 *1.This would require locking the entire container in order to ensure that a consistent copy
	 *  of the state is retrieved, which requires locking all the buckets.
	 *2.The lookup table implementation from listing 6.11 increases the opportunity for con-
        currency of the lookup table as a whole by locking each bucket separately and by
        using a boost::shared_mutex to allow reader concurrency on each bucket. But what
	 */
	std::map<Key,Value> get_map() const
	{
		/* Create a vector to hold lock on each bucket */
		std::vector<std::unique_lock<std::shared_mutex>> locks;

		/* Acquire lock on each bucket */
		for( unsigned int i = 0 ; i < buckets.size() ; i++ )
		{
			locks.push_back(std::unique_lock<std::shared_mutex>( buckets[i].mutex ));
		}

		/* Create result variable to store all <Key,Value> pair from all the buckets */
		std::map<Key,Value> result;
		for( unsigned int i = 0 ; i < buckets.size() ; i++ )
		{
			for( bucket_iterator it = buckets[i].data.begin() ; it != buckets[i].data.end() ; it++ )
			{
				result.insert(*it);
			}
		}

		return result;
	}


};







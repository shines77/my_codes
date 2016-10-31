#pragma once

#include <numeric>
#include <mutex>
#include <condition_variable>

template <>
class simple_semaphore<void, false>
{
	using locker_t = std::unique_lock<std::mutex>;
public:
	~simple_semaphore() = default;
	simple_semaphore(simple_semaphore const&) = delete;
	simple_semaphore& operator= (simple_semaphore const&) = delete;
	simple_semaphore(simple_semaphore&&) = default;
	simple_semaphore& operator= (simple_semaphore&&) = default;

	simple_semaphore(uint32_t max_count)
		: count_(0)
		, max_(max_count)
	{

	}

	void wait()
	{
		locker_t locker{ mutex_ };
		cond_var_.wait(locker, [this]() { return count_ != 0; });
		--count_;
	}

	void signal(uint32_t count = 1)
	{
		locker_t locker{ mutex_ };
		count_ += count;
		if (count_ > max_)
			count_ = max_;
		cond_var_.notify_one();
	}

private:
	std::mutex					mutex_;
	std::condition_variable		cond_var_;
	uint32_t					count_;
	uint32_t const				max_;
};

using semaphore = simple_semaphore<void>;

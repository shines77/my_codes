#pragma once

#include <stdint.h>

template <typename SemaphoreT>
struct is_platform_semaphore
{
	static constexpr bool value() noexcept
	{
		return false;
    }
};

template <typename SemaT = void,
	bool = is_platform_semaphore<SemaT>::value()>
class simple_semaphore;

template <typename SemaT>
class simple_semaphore<SemaT, true>
{
	using sema_type = SemaT;
public:
	~simple_semaphore() = default;
	simple_semaphore(const simple_semaphore&) = delete;
	simple_semaphore& operator=(simple_semaphore const&) = delete;
	simple_semaphore(simple_semaphore&&) = default;
	simple_semaphore& operator=(simple_semaphore&&) = default;

	simple_semaphore(uint32_t max_count = 0)
		: semaphore_(max_count)
	{

	}

	void wait()
	{
		semaphore_.wait();
	}

	void signal(uint32_t count = 1)
	{
		semaphore_.signal(count);
	}

private:
	sema_type semaphore_;
};

#ifdef _MSC_VER
#include "windows_semaphore.hpp"
#else
#include "simple_semaphore.hpp"
#endif
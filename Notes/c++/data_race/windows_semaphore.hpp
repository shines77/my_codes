#pragma once

#include <stdint.h>
#include <Windows.h>

namespace win32
{
	class semaphore
	{
	public:
		semaphore(semaphore const&) = delete;
		semaphore& operator=(semaphore const&) = delete;
		semaphore(semaphore&&) = default;
		semaphore& operator=(semaphore&&) = default;

		explicit semaphore(uint32_t max_count)
			: sema_handle_(CreateSemaphore(NULL, 0, max_count, NULL))
		{

		}

		~semaphore()
		{
			CloseHandle(sema_handle_);
		}

		void wait()
		{
			WaitForSingleObject(sema_handle_, INFINITE);
		}

		void signal(uint32_t count = 1)
		{
			ReleaseSemaphore(sema_handle_, count, NULL);
		}

	private:
		HANDLE	sema_handle_;
	};
}

template <>
struct is_platform_semaphore<win32::semaphore>
{
	static constexpr bool value() noexcept
	{
		return true;
	}
};

using semaphore = simple_semaphore<win32::semaphore>;

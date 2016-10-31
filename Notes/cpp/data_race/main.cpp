
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "semaphore.hpp"

void test_atomic()
{
	using semaphore_type = semaphore;

	semaphore_type sema_1{ 1 };
	semaphore_type sema_2{ 1 };
	semaphore_type end_sema{ 2 };

	//int x, y, r1, r2;
	std::atomic<int> x, y, r1, r2;
    std::atomic<bool> exit_flag = true;

#if 1
    x = 0; y = 0; r1 = 0; r2 = 0;
    std::atomic_thread_fence(std::memory_order_seq_cst);
#else
    x.store(0, std::memory_order_seq_cst);
    y.store(0, std::memory_order_seq_cst);
    r1.store(0, std::memory_order_seq_cst);
    r2.store(0, std::memory_order_seq_cst);
#endif

    printf("--------------------------------------------------------------------\n");
    printf("test_atomic():\n\n");

    std::memory_order load_memory_order, store_memory_order;
    // You can switch the atomic::load() memory_order and atomic::store() memory_order,
    // default value both is std::memory_order_seq_cst,
#if 0
    // std::memory_order_seq_cst is a instruction level memory barrier.
    // On x86, it's a InterlockedExchange() instruction.
    // On arm, it's a instruction level memory barrier - "mfence" or other like this.
    load_memory_order = std::memory_order_seq_cst;
    store_memory_order = std::memory_order_seq_cst;
#else
    // std::memory_order_acquire and std::memory_order_release
    // is a compiler level memory barrier or a instruction level memory barrier.
    // On x86, it's a compiler level memory barrier.
    // On arm, it's a instruction level memory barrier.
    load_memory_order = std::memory_order_acquire;
    store_memory_order = std::memory_order_release;
#endif

	std::thread t1
	{
		[&]()
		{
			while (exit_flag.load())
			{
				//sema_1.wait();
				while (std::rand() % 8 != 0);

				// transaction thread 1
				x.store(1, store_memory_order);
				r1.store(y.load(load_memory_order), store_memory_order);
				///////////////////////

				//end_sema.signal();
			}
		}
	};

    std::thread t2
    {
	    [&]()
	    {
		    while (exit_flag.load())
		    {
			    //sema_2.wait();
			    while (std::rand() % 8 != 0);

			    // transaction thread 2
			    y.store(1, store_memory_order);
			    r2.store(x.load(load_memory_order), store_memory_order);
			    ///////////////////////

			    //end_sema.signal();
		    }
	    }
    };

    static const size_t max_iterations = 500000;
	size_t detected_0_0 = 0, detected_0_1 = 0, detected_1_0 = 0, detected_1_1 = 0;
    size_t detected_other = 0, detected_total = 0;
	for (auto iterations = 0; iterations < max_iterations; ++iterations)
	{
		x = 0; y = 0; r1 = 0; r2 = 0;
		//sema_1.signal();
		//sema_2.signal();
		//end_sema.wait();
		//end_sema.wait();

        detected_total++;
		if (r1 == 0 && r2 == 0)
			detected_0_0++;
		else if (r1 == 0 && r2 == 1)
			detected_0_1++;
		else if (r1 == 1 && r2 == 0)
			detected_1_0++;
		else if (r1 == 1 && r2 == 1)
			detected_1_1++;
        else
            detected_other++;   // Other unknown result.
	}

    exit_flag.store(false);
	//sema_1.signal();
	//sema_2.signal();
	//end_sema.wait();
	//end_sema.wait();

    printf("[r1, r2] order result [0, 0]: %8zu / %zu, %5.1f %% detected.\n",
        detected_0_0, detected_total, ((double)detected_0_0  * 100.0/ detected_total));
    printf("[r1, r2] order result [0, 1]: %8zu / %zu, %5.1f %% detected.\n",
        detected_0_1, detected_total, ((double)detected_0_1  * 100.0/ detected_total));
    printf("[r1, r2] order result [1, 0]: %8zu / %zu, %5.1f %% detected.\n",
        detected_1_0, detected_total, ((double)detected_1_0  * 100.0/ detected_total));
    printf("[r1, r2] order result [1, 1]: %8zu / %zu, %5.1f %% detected.\n",
        detected_1_1, detected_total, ((double)detected_1_1  * 100.0/ detected_total));
    printf("\n");
    printf("[r1, r2] order result [x, x]: %8zu / %zu, %5.1f %% detected.\n",
        detected_other, detected_total, ((double)detected_other  * 100.0/ detected_total));
    printf("\n");

    if (t1.joinable())
        t1.join();
    if (t2.joinable())
        t2.join();
}

void test_semaphore()
{
	using semaphore_type = semaphore;

	semaphore_type sema_1{ 1 };
	semaphore_type sema_2{ 1 };
	semaphore_type end_sema{ 2 };

	//int x, y, r1, r2;
	std::atomic<int> x, y, r1, r2;
    std::atomic<bool> exit_flag = true;

    x.store(0, std::memory_order_seq_cst);
    y.store(0, std::memory_order_seq_cst);
    r1.store(0, std::memory_order_seq_cst);
    r2.store(0, std::memory_order_seq_cst);

    printf("--------------------------------------------------------------------\n");
    printf("test_semaphore():\n\n");

    std::memory_order load_memory_order, store_memory_order;
    // You can switch the atomic::load() memory_order and atomic::store() memory_order,
    // default value both is std::memory_order_seq_cst,
#if 0
    // std::memory_order_seq_cst is a instruction level memory barrier.
    // On x86, it's a InterlockedExchange() instruction.
    // On arm, it's a instruction level memory barrier - "mfence" or other like this.
    load_memory_order = std::memory_order_seq_cst;
    store_memory_order = std::memory_order_seq_cst;
#else
    // std::memory_order_acquire and std::memory_order_release
    // is a compiler level memory barrier or a instruction level memory barrier.
    // On x86, it's a compiler level memory barrier.
    // On arm, it's a instruction level memory barrier.
    load_memory_order = std::memory_order_acquire;
    store_memory_order = std::memory_order_release;
#endif

	std::thread t1
	{
		[&]()
		{
			while (exit_flag.load())
			{
				sema_1.wait();
				while (std::rand() % 8 != 0);

				// transaction thread 1
				x.store(1);
				r1.store(y.load());
				///////////////////////

				end_sema.signal();
			}
		}
	};

    std::thread t2
    {
	    [&]()
	    {
		    while (exit_flag.load())
		    {
			    sema_2.wait();
			    while (std::rand() % 8 != 0);

			    // transaction thread 2
			    y.store(1);
			    r2.store(x.load());
			    ///////////////////////

			    end_sema.signal();
		    }
	    }
    };

    static const size_t max_iterations = 500000;
	size_t detected_0_0 = 0, detected_0_1 = 0, detected_1_0 = 0, detected_1_1 = 0;
    size_t detected_other = 0, detected_total = 0;
	for (auto iterations = 0; iterations < max_iterations; ++iterations)
	{
		x = 0; y = 0; r1 = 0; r2 = 0;
		sema_1.signal();
		sema_2.signal();
		end_sema.wait();
		end_sema.wait();

        detected_total++;
		if (r1 == 0 && r2 == 0)
			detected_0_0++;
		else if (r1 == 0 && r2 == 1)
			detected_0_1++;
		else if (r1 == 1 && r2 == 0)
			detected_1_0++;
		else if (r1 == 1 && r2 == 1)
			detected_1_1++;
        else
            detected_other++;   // Other unknown result.
	}

    exit_flag.store(false);
	sema_1.signal();
	sema_2.signal();
	end_sema.wait();
	end_sema.wait();

    printf("[r1, r2] order result [0, 0]: %8zu / %zu, %5.1f %% detected.\n",
        detected_0_0, detected_total, ((double)detected_0_0  * 100.0/ detected_total));
    printf("[r1, r2] order result [0, 1]: %8zu / %zu, %5.1f %% detected.\n",
        detected_0_1, detected_total, ((double)detected_0_1  * 100.0/ detected_total));
    printf("[r1, r2] order result [1, 0]: %8zu / %zu, %5.1f %% detected.\n",
        detected_1_0, detected_total, ((double)detected_1_0  * 100.0/ detected_total));
    printf("[r1, r2] order result [1, 1]: %8zu / %zu, %5.1f %% detected.\n",
        detected_1_1, detected_total, ((double)detected_1_1  * 100.0/ detected_total));
    printf("\n");
    printf("[r1, r2] order result [x, x]: %8zu / %zu, %5.1f %% detected.\n",
        detected_other, detected_total, ((double)detected_other  * 100.0/ detected_total));
    printf("\n");

    if (t1.joinable())
        t1.join();
    if (t2.joinable())
        t2.join();
}

int main(int argn, char * argv[])
{
    test_atomic();
    test_semaphore();

#if defined(_WIN32) && (defined(NDEBUG) || !defined(NDEBUG))
    ::system("pause");
#endif
    return 0;
}

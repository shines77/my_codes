
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <atomic>
#include <thread>
#include <memory>

#include <stdint.h>
#include <string.h>
#include <assert.h>

static const uint32_t max_thread_cnt = 8;

std::atomic<uint32_t> g_counter = 0;

struct Task {
    int index;
    bool canceled;

    Task() : index(-1), canceled(false) {}
    Task(int _index) : index(_index), canceled(false) {}
    ~Task() {}

    bool isCanceled() const { return canceled; }
};

void test_thread_proc(const Task & task, uint32_t hardware_concurrency)
{
    uint32_t tls_counter;
    if (!task.isCanceled()) {
        std::cout << ">> [" << task.index << "]" << "send control command." << std::endl;

        //tls_counter = g_counter.fetch_add(1) + 1;
        tls_counter = ++g_counter;
        std::ostringstream oss;
        oss << ">> index: " << task.index << ", counter: " << tls_counter << std::endl;
        std::cout << oss.str().c_str() << std::endl;

        // std::thread::hardware_concurrency() * 2
        if (tls_counter >= hardware_concurrency) {
            std::cout << std::endl;
            std::cout << ">> All thread(s) finished." << std::endl;
            std::cout << std::endl;
        }
    }
}

void atomic_test(uint32_t thread_num)
{
    std::unique_ptr<Task[]> tasks(new Task[thread_num]());
    std::unique_ptr<std::unique_ptr<std::thread>[]>
        threads(new std::unique_ptr<std::thread>[thread_num]());

    std::cout << "--------------------- atomic_test() -----------------------" << std::endl << std::endl;

    {
        for (int i = 0; i < (int)thread_num; ++i) {
            Task * pTask1 = &(tasks.get()[i]);
            Task * pTask2 = &tasks[i];
            assert(pTask1 == pTask2);
#if 1
            tasks[i].index = i + 1;
            threads[i].reset(new std::thread(test_thread_proc, tasks[i], thread_num));
#else
            tasks.get()[i].index = i + 1;
            threads.get()[i].reset(new std::thread(test_thread_proc, tasks.get()[i], thread_num));
#endif
        }

        std::cout << ">>>> All " << thread_num << " thread(s) create done." << std::endl << std::endl;

        for (int i = 0; i < (int)thread_num; ++i) {
            if (threads[i].get() && threads[i].get()->joinable())
                threads[i].get()->join();
        }
    }

    std::cout << ">>>> All " << thread_num << " thread(s) run finished." << std::endl << std::endl;

    std::cout << "-----------------------------------------------------------" << std::endl << std::endl;
}

template <typename T>
std::shared_ptr<T> make_shared_array(size_t size)
{
    // std::default_delete<T> 是 STL 中的默认删除器.
    return std::shared_ptr<T>(new T[size](), std::default_delete<T[]>());
}

void atomic_test2(uint32_t thread_num)
{
#if 1
    std::shared_ptr<Task> tasks = make_shared_array<Task>(thread_num);
    std::shared_ptr<std::shared_ptr<std::thread>> threads =
        make_shared_array<std::shared_ptr<std::thread>>(thread_num);
#else
    std::shared_ptr<Task> tasks(new Task[thread_num](), [](Task * p) { delete[] p; });
    std::shared_ptr<std::shared_ptr<std::thread>>
        threads(new std::shared_ptr<std::thread>[thread_num](),
                [](std::shared_ptr<std::thread> * p) { delete[] p; });
#endif

    std::cout << "--------------------- atomic_test2() ----------------------" << std::endl << std::endl;

    {
        for (int i = 0; i < (int)thread_num; ++i) {
            tasks.get()[i].index = i + 1;
            threads.get()[i].reset(new std::thread(test_thread_proc, tasks.get()[i], thread_num));
        }

        std::cout << ">>>> All " << thread_num << " thread(s) create done." << std::endl << std::endl;

        for (int i = 0; i < (int)thread_num; ++i) {
            if (threads.get()[i] && threads.get()[i]->joinable())
                threads.get()[i]->join();
        }
    }

    std::cout << ">>>> All " << thread_num << " thread(s) run finished." << std::endl << std::endl;

    std::cout << "-----------------------------------------------------------" << std::endl << std::endl;
}

int main(int argc, char * argv[])
{
    atomic_test(max_thread_cnt);
    atomic_test2(max_thread_cnt);

    ::system("pause");
    return 0;
}

#include <iostream>
#include <thread>
using namespace std;

#include "thread_pool.h"


void test_thread_pool()
{
    Thread_Pool pool;

    std::thread thd1([&pool]{
        for(int i = 0; i < 10; ++i)
        {
            auto thd_id = this_thread::get_id();
            pool.add_task( [thd_id]{cout << "sync thread 1 id " << thd_id << endl;} );
        }
    });

    std::thread thd2([&pool]{
        for(int i = 0; i < 10; ++i)
        {
            auto thd_id = this_thread::get_id();
            pool.add_task( [thd_id]{cout << "sync thread 2 id " << thd_id << endl;} );
        }
    });

    this_thread::sleep_for(std::chrono::seconds(2));
    getchar();
    pool.stop();
    thd1.join();
    thd2.join();
}


int main()
{
    test_thread_pool();
    return 0;
}

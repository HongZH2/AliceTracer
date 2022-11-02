//
// Created by Hong Zhang on 2022/10/28.
//

#ifndef ALICE_TRACER_ALICE_THREADS_H
#define ALICE_TRACER_ALICE_THREADS_H

#include "alice_common.h"

namespace ALICE_UTILS {
    /*
     * TheadPool
     */
    class ThreadPool{
    public:
        static ThreadPool & getInstance(){
            static ThreadPool instance;
            return instance;
        }
        ThreadPool(const ThreadPool &) = delete;
        ThreadPool & operator=(const ThreadPool &) = delete;

        template<typename Func, typename ... Args>
        static void submitTask(Func && func, Args && ... args){
            auto & thread_pool = ThreadPool::getInstance();
            auto && task = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            std::lock_guard<std::mutex> lock(thread_pool.mtx_);
            thread_pool.task_queue_.emplace(task);
            thread_pool.is_empty_.notify_one();
        }

    private:
        ThreadPool(){
            for(int32_t i = 0; i < MAX_NUM_THREADS; ++i){
                thread_pool_[i] = std::thread([&](){
                    while(true) {
                        if(is_release_)
                            break;
                        std::unique_lock<std::mutex> lock(mtx_);
                        if(task_queue_.empty()){
                            is_empty_.wait(lock);
                        }
                        else{
                            auto cur_task = std::move(task_queue_.front());
                            task_queue_.pop();
                            lock.unlock();
                            cur_task();
                        }
                    }
                });

                thread_pool_[i].detach();
            }
        }

        ~ThreadPool(){
            { // 为了把锁释放
                std::lock_guard<std::mutex> locker(mtx_);
                is_release_ = true;
            }
            is_empty_.notify_all();
        }
    private:
        std::mutex mtx_;
        std::condition_variable is_empty_;
        bool is_release_ = false;
        std::queue<std::function<void(void)>> task_queue_;
        std::array<std::thread, MAX_NUM_THREADS> thread_pool_;
    };
}

#endif //ALICE_TRACER_ALICE_THREADS_H

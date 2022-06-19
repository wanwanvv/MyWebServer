/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-03 17:56:32
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-06 16:06:43
 */
#ifndef THREADPOLL_H
#define THREADPOLL_H

#include<thread>
#include<condition_variable>
#include<mutex>
#include<vector>
#include<queue>
#include<future>
#include <iostream>

class ThreadPool{
private:
    bool m_stop;
    std::vector<std::thread> m_thread;
    std::queue<std::function<void()>> tasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
public:
    explicit ThreadPool(size_t threadNumber):m_stop(false){
        for(size_t i=0;i<threadNumber;++i){
            std::cout<<"Thread "<<i<<" initialized!"<<std::endl;
            m_thread.emplace_back(
                [this](){
                    for (;;)
                    {
                        std::function<void()>task;
                        {
                            std::unique_lock<std::mutex> lk(m_mutex);
                            m_cv.wait(lk,[this](){return m_stop||!tasks.empty();});
                            if(m_stop&&tasks.empty()) return;
                            task=std::move(tasks.front());
                            tasks.pop();
                        }
                        task();
                    }
                }
            );
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;//&&右值引用

    ThreadPool& operator=(const ThreadPool&)=delete;
    ThreadPool& operator=(ThreadPool&&)=delete;

    ~ThreadPool(){
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_stop=true;
        }
        m_cv.notify_all();
        for(auto& thread:m_thread)
        {
            thread.join();
        }
    }
    
    template<typename F,typename... Args>
    auto submit(F&& f,Args&&... args)->std::future<decltype(f(args...))>{
        auto taskPtr=std::make_shared<std::packaged_task<decltype(f(args...))()>>(
            std::bind(std::forward<F>(f),std::forward<Args>(args)...)
        );
        {
            std::unique_lock<std::mutex>lk(m_mutex);
            if(m_stop) throw std::runtime_error("submit on stopped ThreadPool");
            tasks.emplace([taskPtr](){ (*taskPtr)(); });
        }
        m_cv.notify_one();
        return taskPtr->get_future();

    }
};

#endif
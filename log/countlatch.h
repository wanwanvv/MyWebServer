/*
 * @Descripttion: 作用是确保Thread中传进去的func真的启动了以后外层的start才返回
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-28 10:50:24
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-28 10:55:00
 */

#ifndef COUNT_LATCH_H
#define COUNT_LATCH_H
#include<thread>
#include<condition_variable>
#include <mutex>

class CountLatch{
public:
    explicit CountLatch(int count);
    void wait();
    void countDown();

private:
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    int count_;
};


#endif
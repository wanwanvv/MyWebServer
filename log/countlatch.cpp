/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-28 10:50:39
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-07-07 21:04:17
 */

#include "countlatch.h"

CountLatch::CountLatch(int count):count_(count){}

void CountLatch::wait(){
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock,[this](){return count_<=0;});
}

void CountLatch::countDown(){
    std::unique_lock<std::mutex> lock(mutex_);
    --count_;
    if(count_==0) condition_.notify_all();
}
/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-05-22 11:00:52
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-05 09:11:40
 */
#ifndef TIMER_H
#define TIMER_H

#include <functional>
#include <chrono>
#include <ctime>
#include <memory>
#include <unordered_map>
#include <vector>
#include<ctime>
#include <assert.h>

typedef std::function<void()> TimeoutCallBack;
//high_resolution的精度是1纳秒
typedef std::chrono::high_resolution_clock Clock;

typedef std::chrono::milliseconds MS;
//表示一个具体时间,chrono库中采用的是Unix的时间戳1970年1月1日 00:00。所以time_point也就是距离时间戳(epoch)的时间长度（duration）
typedef Clock::time_point TimeStamp;

//聚合类
class TimerNode{
public:
    int id;//定时器id
    TimeStamp expire;//设置过期时间
    TimeoutCallBack cb;//设置一个回调函数方便删除定时器时将对应的HTTP连接关闭
    bool operator<(const TimerNode& t)
    {
        return expire<t.expire;
    }
};

class TimerManager{
    typedef std::shared_ptr<TimerNode> SP_TimerNode;
    public:
        TimerManager(){heap_.reserve(64);}
        ~TimerManager(){clear();}
        //设置定时器
        void addTimer(int id,int timeout,const TimeoutCallBack& cb);
        //处理过期的定时器
        void handle_expired_event();
        //下一次处理过期定时器的时间
        int getNextHandle();

        void update(int id,int timeout);
        void work(int id);
        void pop();
        void clear();
        
    private:
        void del_(size_t i);
        void siftup_(size_t i);
        bool siftdown_(size_t index,size_t n);
        void swapNode_(size_t i,size_t j);
        std::vector<TimerNode> heap_;
        std::unordered_map<int,size_t> ref_;//映射一个fd对应的定时器在heap_中的位置
};

#endif //TIMER_H
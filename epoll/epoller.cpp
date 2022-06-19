/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-05-22 10:18:38
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-03 10:10:19
 */

#include "epoller.h"

/**
 * epoll_create(int size)，epoll把关心的文件描述符上的事件放在内核堆里的一个事件列表中
 * @size: 并不起作用，只是高树事件表需要多大
 * @return: 额外的文件描述符来唯一标识内核中的这个事件表
 * 
 * */
Epoller::Epoller(int maxEvent):epollerFd_(epoll_create(512)),events_(maxEvent)
{
    assert(epollerFd_>=0&&events_.size()>0);
}

Epoller::~Epoller()
{
    close(epollerFd_);
}

bool Epoller::addFd(int fd,uint32_t events){
    if(fd<0) return false;
    //     struct epoll_event
    // {
    //   uint32_t events;	/* 描述事件类型，读事件,写事件,水平触发事件,边沿触发事件...*/ 
    //   epoll_data_t data;	/*用户数据*/
    // }
    //
    //     typedef union epoll_data
    // {
    // void *ptr;/*不能和fd同时使用*/
    // int fd; /*指定事件所从属的目标文件描述符*/
    // uint32_t u32;
    // uint64_t u64;
    // } epoll_data_t;
    //epoll在一个文件描述符上只能呢个有一个事件
    epoll_event ev={0};
    ev.data.fd=fd;
    ev.events=events;
    //EPOLL_CTL_ADD:添加事件表中注册fd上的事件，删除epoll事件只需删除fd就够了
    return 0==epoll_ctl(epollerFd_,EPOLL_CTL_ADD,fd,&ev);//成功返回0，失败返回-1并设置errno
}

bool Epoller::modFd(int fd,uint32_t events)
{
    if(fd<0) return false;
    epoll_event ev={0};
    ev.data.fd=fd;
    ev.events=events;
    return 0==epoll_ctl(epollerFd_,EPOLL_CTL_MOD,fd,&ev);
}

bool Epoller::delFd(int fd)
{
    if(fd<0) return false;
    epoll_event ev = {0};
    return 0==epoll_ctl(epollerFd_,EPOLL_CTL_DEL,fd,&ev);
}

int Epoller::wait(int timeoutMs)
{
    //extern int epoll_wait (int __epfd, struct epoll_event *__events,int __maxevents, int __timeout);
    /**
     * epoll系用调用的主要接口是epoll_wait函数，在一段超时时间内等待一组文件描述符上的事件
     * @return: 就绪的文件描述符的个数，失败返回-1并设置errno
     * @_maxevents: 最多监听事件的个数，必须大于0 
     * @__events: 如果检测到事件，就将所有就绪的事件从内核事件表中复制到__envents参数指向的数组中
     */
    return epoll_wait(epollerFd_,&events_[0],static_cast<int>(events_.size()),timeoutMs);
}

int Epoller::getEventFd(size_t i) const
{
    assert(i<events_.size()&&i>=0);
    return events_[i].data.fd;
}

uint32_t Epoller::getEvents(size_t i) const
{
    assert(i < events_.size() && i >= 0);
    return events_[i].events;
}

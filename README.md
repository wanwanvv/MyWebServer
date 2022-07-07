# MyWebServer
A high performance web server implemented in C++.

## Introduction
C++实现的基于Linux的高性能服务器，经过webbench压力测试能达到上万的QPS

## Environment
- Linux
- C++11
- openssl
- g++>=4.8.5
- make

## Install
make clean
make
./bin/server_run

## Functions
+ 利用IO复用技术Epoll与线程池实现的多线程的Reactor高并发模型；
+ 利用正则与状态机解析HTTP请求，实现处理静态资源的要求；
+ 基于堆实现的定时器，关闭超时的非活动连接；
+ 利用openssl库支持ssl协议，允许HTTPS连接请求；
+ 实现LFUCache，加快访问速度；
+ 参考muduo，基于双缓冲机制的异步日志系统，记录服务器运行状态；

## Optimized method


## Directory tree

## Codes statistics

## Performance Test
使用linux压测工具webbench，分别测试10k连接下，keepAlive和non-keepAlive请求。

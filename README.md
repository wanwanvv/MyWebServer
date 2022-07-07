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
+ 参考muduo，基于双缓冲机制实现的单例模式的异步日志系统，记录服务器运行状态；

## Optimized method


## Directory tree

## Codes statistics
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
CSS                              5             647             59           3377
C++                             17            216            343           1510
C/C++ Header                    17            171            186            709
HTML                            10            131            101            645
C                                2             36             68            409
make                             4             31              9             96
JSON                             2              0              0             74
JavaScript                       7             18             45             60
Markdown                         1              0              0              2
-------------------------------------------------------------------------------
SUM:                            65           1250            811           6882
-------------------------------------------------------------------------------

## Performance Test
使用linux压测工具webbench，命令如下：
./webbench-1.5/webbench -c 10 -t 10 http://ip:port/
./webbench-1.5/webbench -c 100 -t 10 http://ip:port/
./webbench-1.5/webbench -c 1000 -t 10 http://ip:port/

对比参照的C++服务器：[Aged-cat](https://github.com/Aged-cat/WebServer)和[markparticle](https://github.com/markparticle/WebServer/)(表格中的为QPS的值)：

|      |  10   |  100  | 1000  | 5000 |
| :--: | :---: | :---: | :---: | :---: | 
| Aged-cat  | 17609 | 18256 | 18117 | 126 |
| markparticle  | 20394  | 21592  | 22640  | 152 |
| wanwanvv  | 21478 | 23578 | 23375 | 154 |

* 测试环境: Ubuntu:19.10 cpu:i5-8400 内存64G 
* QPS 10000+

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

## Functions and optimized method
+ 利用IO复用技术Epoll与线程池实现的多线程的Reactor高并发模型；
+ 利用正则与状态机解析HTTP请求，实现处理静态资源的要求；
+ 基于堆实现的定时器，关闭超时的非活动连接；
+ 利用openssl库支持ssl协议，允许HTTPS连接请求；
+ 实现LFUCache，加快访问速度；
+ 参考muduo，基于双缓冲机制实现的单例模式的异步日志系统，记录服务器运行状态；
+ 使用了智能指针，bind和function等c++11的新特性
+ 利用RAII机制实现了数据库连接池，减少数据库连接建立与关闭的开销，同时实现了用户注册登录功能

## Directory tree
```
.
├── buffer 实现读写网络数据缓存
│   ├── buffer.cpp
│   └── buffer.h
├── cache LFU缓存
│   ├── cache_test_run
│   ├── LFCache_test.cpp
│   ├── LFUCache.cpp
│   ├── LFUCache.h
│   └── makefile
├── config
├── epoll
│   ├── epoller.cpp
│   └── epoller.h
├── http 连接处理
│   ├── HTTPconnection.cpp
│   ├── HTTPconnection.h
│   ├── HTTPrequest.cpp
│   ├── HTTPrequest.h
│   ├── HTTPresponse.cpp
│   └── HTTPresponse.h
├── img
│   └── cloc.png
├── log 双缓冲实现日志
│   ├── asynclogging.cpp
│   ├── asynclogging.h
│   ├── countlatch.cpp
│   ├── countlatch.h
│   ├── fileutil.cpp
│   ├── fileutil.h
│   ├── fixedbuffer.h
│   ├── logfile.cpp
│   ├── logfile.h
│   ├── logging.cpp
│   ├── logging.h
│   ├── logging_test.cpp
│   ├── logstream.cpp
│   ├── logstream.h
│   ├── makefile
│   └── noncopyable.h
├── sql 数据库连接
│   ├── sqlconnpool.cpp
│   ├── sqlconnpool.h
│   └── sqlconnRAII.h
├── main.cpp
├── makefile
├── pool
│   └── threadpool.h
├── README.md
├── resources 存储静态文件
├── server
│   ├── webserver.cpp
│   └── webserver.h
├── ssl ssl证书和密钥
│   ├── ca.crt
│   ├── ca.csr
│   ├── ca.key
│   ├── certificate.config
│   └── openssl.cnf
├── timer 小根堆实现的定时器
│   ├── timer.cpp
│   └── timer.h
└── webbench-1.5
    ├── Makefile
    ├── socket.c
    ├── webbench
    └── webbench.c

```

## Codes statistics
<p align="center">
<img src=".\img\cloc.png" height = "220" alt="" align=center />
<br><br>
<b>Figure 1.</b> Code cloc.
</p>

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

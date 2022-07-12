/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-05-28 10:56:53
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-07-07 09:44:11
 */
#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H
#include<arpa/inet.h> //sockaddr_in
#include<iostream>
#include<sys/uio.h> //readv/writev
#include<sys/types.h>
#include "buffer.h"
#include "HTTPrequest.h"
#include "HTTPresponse.h"
#include "LFUCache.h"

//OpenSSL 库的核心数据结构是SSL对象和SSL_CTX对象
typedef std::shared_ptr<SSL_CTX> SP_SSL_CTX;
typedef std::shared_ptr<SSL> SP_SSL;

class HTTPconnection{
public:
    HTTPconnection();
    ~HTTPconnection();

    void initHTTPConn(int socketFd,const sockaddr_in& addr);

    //每个连接中定义的对缓冲区的读写接口
    ssize_t readBuffer(int* saveErrno);
    ssize_t writeBuffer(int* saveErrno);

    //关闭连接的接口
    void closeHTTPConn();
    //定义处理HTTP连接的接口，分为解析request和生成response
    bool handleHTTPConn();
    //处理SSL握手
    bool sslHandShake();

    //其他方法
    const char* getIP() const;
    int getPort() const;
    int getFd() const;
    sockaddr_in getAddr() const;

    int writeBytes(){
        return iov_[1].iov_len+iov_[0].iov_len;
    }

    bool isKeepAlive() const
    {
        return request_.isKeepAlive();
    }

    bool getSSLConnect(){
        return isSSLConnect;
    }

    bool setSSLConnect(bool flag){
        isSSLConnect=flag;
    }

    SP_SSL getSSL(){
        return ssl_;
    }

    bool setSSL(bool flag){
        isSSLConnect=flag;
    }

    static bool isET;
    static SP_SSL_CTX ctx;//ssl 变量
    static bool isSSL;
    static const char* srcDir;
    
    static std::atomic<int>userCount;//原子类型，允许无锁并行编程

private:
    int fd_; //HTTP连接对应的连接描述符
    struct sockaddr_in addr_;
    bool isClose_;//标记是否关闭连接

    int iovCnt_;
    struct iovec iov_[2];

    Buffer readBuffer_;//读缓冲区
    Buffer writeBuffer_;//写缓冲区

    HTTPrequest request_;
    HTTPresponse response_;
    
    SP_SSL ssl_;
    bool isSSLConnect;
};

#endif //HTTP_CONNECTION_H
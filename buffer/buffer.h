/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-05-29 09:18:28
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-27 15:20:52
 */
#ifndef BUFFER_H
#define BUFFER_H

#include<vector>
#include<iostream>
#include<cstring>
#include<atomic>
#include<unistd.h> //read() write()
#include<sys/uio.h> //readv() writev()
#include<assert.h>

#include <openssl/bio.h> //处理包括文件和套接字在内的各种类型的通信
#include <openssl/ssl.h>//ssl的主要数据结构
#include <openssl/err.h>

class Buffer{
public:
    Buffer(int initBufferSize=1024);

    ~Buffer()=default;

    //缓冲区中可以读取的字节数
    size_t writeableBytes() const;
    //缓冲区中可以写入的字节数
    size_t readableBytes() const;
    //缓冲区中已经读取的字节数
    size_t readBytes() const;

    //获取当前读指针
    const char* curReadPtr() const;
    //获取当前写指针
    const char* curWritePtrConst() const;
    char* curWritePtr();
    //更新读指针
    void updateReadPtr(size_t len);
    void updateReadPtrUntilEnd(const char* end);//将读指针直接更新到指定位置
    //更新写指针
    void updateWritePtr(size_t len);
    //将读指针和写指针初始化
    void initPtr();

    //保证将数据写入缓冲区
    void ensureWriteable(size_t len);
    //将数据写入到缓冲区
    void append(const char* str,size_t len);
    void append(const std::string& str);
    void append(const void* data,size_t len);
    void append(const Buffer& buffer);

    //I/O操作的读写接口
    ssize_t readFd(int fd,int* Errno);
    ssize_t writeFd(int fd,int* Errno);

    ssize_t readFd_ssl(SSL* ssl,int* Errno);
    ssize_t writeFd_ssl(SSL* ssl,int* Errno);

    //将缓冲区的数据转化为字符串
    std::string AlltoStr();

    //test
    void printContent(){
        std::cout<<"pointer location info:"<<readPos_<<" "<<writePos_<<std::endl;
        for(int i=readPos_;i<=writePos_;++i)
        {
            std::cout<<buffer_[i]<<" ";
        }
        std::cout<<std::endl;
    }

private:
    //指向缓冲区初始位置的指针
    char* BeginPtr_();
    const char* BeginPtr_() const;
    //用于缓冲区空间不够时的扩容
    void allocateSpace(size_t len);

    std::vector<char> buffer_;
    std::atomic<std::size_t> readPos_;//指示读指针
    std::atomic<std::size_t> writePos_;//指示写指针

};

#endif
/*
 * @Descripttion: 用来格式化输出，重载了<<运算符，同时也有自己的一块缓冲区，这里缓冲区的存在是为了缓存一行，把多个<<的结果连成一块
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-21 10:46:09
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-28 10:29:38
 */
#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include <assert.h>
#include <string>
#include <string.h>
#include "fixedbuffer.h"
#include "noncopyable.h"

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

class LogStream:noncopyable{
public:
	
	typedef FixedBuffer<kSmallBuffer> Buffer;

    LogStream& operator<<(bool);
	LogStream& operator<<(short);
	LogStream& operator<<(unsigned short);
	LogStream& operator<<(int);
	LogStream& operator<<(unsigned int);
	LogStream& operator<<(long);
	LogStream& operator<<(unsigned long);
	LogStream& operator<<(long long);
	LogStream& operator<<(unsigned long long);
	LogStream& operator<<(float);
	LogStream& operator<<(double);
	LogStream& operator<<(long double);
	LogStream& operator<<(char);
	LogStream& operator<<(const char*);
	LogStream& operator<<(const std::string&);

    void append(const char* data,size_t len){buffer_.append(data, len); }
    const Buffer& buffer() const{return buffer_;}
    void resetBuffer(){buffer_.reset();}

private:
    template<typename T> void formatInteger(T);
    template<typename T> void formatDecimal(T);
	template <typename T> size_t convert(char buf[], T value);
    static const int kMaxNumbericSize=32;
    Buffer buffer_;

};

#endif
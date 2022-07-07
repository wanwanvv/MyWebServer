/*
 * @Descripttion: 它负责启动一个log线程，专门用来将log写入LogFile，应用了“双缓冲技术”，其实有4个以上的缓冲区
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-22 14:38:47
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-29 09:51:55
 */
#ifndef ASCYNC_LOGGING_H
#define ASCYNC_LOGGING_H

#include <functional>
#include <string>
#include <vector>
#include "logstream.h"
#include "countlatch.h"
#include "logfile.h"

class AsyncLogging:noncopyable{
public:
    AsyncLogging(const std::string basename, int flushInterval=2);
    ~AsyncLogging(){}
    void append(const char *logfile,int len);
	void start();
	void stop();	
    
private:
    void threadFunc();
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::shared_ptr<Buffer>> BufferVector;
    typedef std::shared_ptr<Buffer> BufferPtr;
    const int flushInterval_;
    bool running_;
    std::string basename_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable condition_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
    CountLatch latch_;    
};

typedef std::unique_ptr<AsyncLogging> UP_AsyncLogging;
#endif
/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-27 16:06:40
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-29 09:51:36
 */
#include "asynclogging.h"

AsyncLogging::AsyncLogging(std::string logFileName, int flushInterval)
:flushInterval_(flushInterval),
running_(false),
basename_(logFileName),
currentBuffer_(new Buffer),
nextBuffer_(new Buffer),
latch_(1)
{
    assert(logFileName.length()>1);
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

AsyncLogging::~AsyncLogging(){
    if(running_) stop();
}

void AsyncLogging::start(){
    running_=true;
    thread_=std::thread(threadFunc);
    latch_.wait();
}

void AsyncLogging::stop(){
    running_=false;
    condition_.notify_all();
    thread_.join();
}

void AsyncLogging::append(const char *logline,int len){
    std::unique_lock<std::mutex> lock(mutex_);
    if(currentBuffer_->avail()>len){
        currentBuffer_->append(logline,len);
    }else{
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();
        if(nextBuffer_) currentBuffer_=std::move(nextBuffer_);
        else currentBuffer_.reset(new Buffer);
        currentBuffer_->append(logline,len);
        condition_.notify_all();
    }
}

void AsyncLogging::threadFunc(){
    assert(running_==true);
    latch_.countDown();
    //每个线程初始化一个LogFile对象往文件写
    LogFile output(basename_);
    //两个新的Buffer指针
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    //初始化
    newBuffer1->bzero();
    newBuffer2->bzero();
    //预留大小为16的Buffer vector，此时为空
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while(running_){
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(buffers_.empty()) condition_.wait_for(lock,std::chrono::seconds(flushInterval_));
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();
            currentBuffer_=std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if(!nextBuffer_){
                nextBuffer_=std::move(newBuffer2);
            }
        }
        assert(!buffersToWrite.empty());
        if(buffersToWrite.size()>25){
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }
        for (size_t i = 0; i < buffersToWrite.size(); ++i) {
            // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }
        if (buffersToWrite.size() > 2) {
            // drop non-bzero-ed buffers, avoid trashing
            buffersToWrite.resize(2);
        }

        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}

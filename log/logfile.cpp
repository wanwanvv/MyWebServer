/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-28 08:54:33
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-29 08:59:18
 */

#include "logfile.h"

LogFile::LogFile(const std::string& basename,int flushEveryN)
:basename_(basename),
flushEveryN_(flushEveryN),
count_(0),
file_(new AppendFile(basename)),
mutex_(new std::mutex())
{
    // file_.reset(new AppendFile(basename));
    // mutex_.reset(new std::mutex);
}

LogFile::~LogFile(){}

void LogFile::flush(){
    std::unique_lock<std::mutex> lock(*mutex_);
    file_->flush();
}

void LogFile::append(const char* logline, int len){
    std::unique_lock<std::mutex> lock(*mutex_);
    this->append_unlocked(logline,len);
}

void LogFile::append_unlocked(const char* logline, int len){
    file_->append(logline,len);
    ++count_;
    if(count_>=flushEveryN_){
        count_=0;
        file_->flush();
    }
}
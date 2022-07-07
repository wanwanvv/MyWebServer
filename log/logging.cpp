/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-28 10:33:20
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-29 08:58:36
 */

#include "logging.h"

std::string Logger::logFileName_ = "../WebServer.log";
std::once_flag flag1;
static UP_AsyncLogging AsyncLogger_;

void once_init()
{
    AsyncLogger_.reset(new AsyncLogging(Logger::getLogFileName()));
    AsyncLogger_->start(); 
}

void output(const char* msg, int len)
{
    //pthread_once(&once_control_, once_init);
    std::call_once(flag1, once_init);
    AsyncLogger_->append(msg, len);
}

//*******************Logger class***************
Logger::Impl::Impl(const char* filename, int line):stream_(),line_(line),basename_(filename){
    formatTime();
}

void Logger::Impl::formatTime(){
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);   
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}

Logger::Logger(const char* fileName,int line):impl_(fileName,line)
{

}

Logger::~Logger(){
    impl_.stream()<<"--"<<impl_.getBaseName()<<':'<<impl_.getLine()<<'\n';
    const LogStream::Buffer& buf(impl_.stream().buffer());
    output(buf.data(),buf.length());
}


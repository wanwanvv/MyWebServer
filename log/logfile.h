/*
 * @Descripttion: 每append N次，就会flush一下并利用fileutil往文件写
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-27 16:27:13
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-29 08:16:42
 */
#ifndef LOG_FILE_H
#define LOG_FILE_H

#include <memory>
#include <string>
#include <mutex>
#include "noncopyable.h"
#include "fileutil.h"

class LogFile:noncopyable{
public:
    LogFile(const std::string& basename,int flushEveryN=1024);
    ~LogFile();

    void append(const char* logline,int len);
    void flush();
    //bool rollFile();

private:

    const std::string basename_;
    const int flushEveryN_;

    int count_;
    std::unique_ptr<std::mutex> mutex_;
    std::unique_ptr<AppendFile> file_;

    void append_unlocked(const char* logline, int len);
};

#endif
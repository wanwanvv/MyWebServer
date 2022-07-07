/*
 * @Descripttion: 最底层的文件类，封装了Log文件的打开，写入并在类析构的时候关闭文件，底层使用了标准IO，该append函数直接向文件写
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-27 16:33:28
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-28 08:52:35
 */
#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <string>
#include <iostream>
#include "noncopyable.h"

class AppendFile:noncopyable{
public:
    explicit AppendFile(std::string filenam);
    ~AppendFile();

    void append(const char* logfile,const size_t len);//直接写入文件
    void flush();

private:
    size_t write(const char* logfile,const size_t len);
    FILE* fp_;
    char buff_[64*1024];
};

#endif
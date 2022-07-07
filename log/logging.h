/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-21 10:43:04
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-29 08:36:42
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <sys/time.h>
#include <string>
#include "logstream.h"
#include "asynclogging.h"

class Logger{
public:
    Logger(const char* fileName,int line);
    ~Logger();
    LogStream &stream();
    static std::string getLogFileName();

private:
    class Impl{
        public:
            Impl(const char* filename, int line);
            void formatTime();//当前时间格式
            std::string getBaseName(){return basename_;}
            LogStream& stream(){return stream_;}
            int getLine(){return line_;}	
            
        private:
            LogStream stream_;
            int line_;//所在行数
            std::string basename_;//文件名
    };

    Impl impl_;
    static std::string logFileName_;

};

//__FILE__本行语句所在源文件
//__LINE__本行语句在源文件中的行数
#define LOG Logger(__FILE__, __LINE__).stream()

#endif
/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-03 11:44:15
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-22 20:58:20
 */
#ifndef RESPONSE_H
#define RESPONSE_H

#include <unordered_map>
#include <fcntl.h>  //open
#include <unistd.h> //close
#include <sys/stat.h> //stat
#include <sys/mman.h> //mmap,munmap
#include <assert.h>

#include "buffer.h"

class HTTPresponse{
public:
    HTTPresponse();
    ~HTTPresponse();

     void init(const std::string& srcDir,std::string& path,bool isKeepAlive=false,int code=-1);
    void makeResponse(Buffer& buffer);
    void unmapFile_();
    char* file();
    size_t fileLen() const;
    void errorContent(Buffer& buffer,std::string message);
    int code() const {return code_;}

private:
    void addStateLine_(Buffer& buffer);
    void addResponseHeader_(Buffer& buffer);
    void addResponseContent_(Buffer& buffer);

    //void addResponseEntity_();

    void errorHTML_();
    std::string getFileType_();

    int code_;
    bool isKeepAlive_;

    std::string path_;
    std::string srcDir_;

    char* mmFile_;
    struct  stat mmFileStat_;//stat结构存储文件的信息（元数据）

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    static const std::unordered_map<int, std::string> CODE_PATH;

};

#endif
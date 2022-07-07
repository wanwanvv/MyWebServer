/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-27 16:33:35
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-28 08:53:42
 */

#include "fileutil.h"


AppendFile::AppendFile(std::string filename):fp_(fopen(filename.c_str(),"a+")){
    // 用户提供缓冲区
  setbuffer(fp_, buff_, sizeof buff_);
}

AppendFile::~AppendFile(){fclose(fp_);}

void AppendFile::append(const char* logline,size_t len){
    size_t n=this->write(logline,len);
    size_t remain=len-n;
    while(remain>0){
        size_t new_n=this->write(logline+n,remain);
        if(new_n==0){
            int err=ferror(fp_);
            if(err) std::cout<<"AppendFile::append() failed !"<<std::endl;
            break;
        }
        n+=new_n;
        remain-=new_n;
    }
}

void AppendFile::flush(){
    fflush(fp_);
}

size_t AppendFile::write(const char* logline,size_t len){
    return fwrite_unlocked(logline,1,len,fp_);
}


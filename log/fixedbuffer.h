/*
 * @Descripttion: 一块固定大小的内存缓冲区域
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-28 09:37:26
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-29 08:05:58
 */
#ifndef FIXED_BUFFER_H
#define FIXED_BUFFER_H
#include <assert.h>
#include <string.h>
#include <string>
#include "noncopyable.h"

//使用模板非类型参数
template<int SIZE>
class FixedBuffer:noncopyable{
public:
    FixedBuffer():cur_(data){}
    ~FixedBuffer(){}

    void append(const char* buf, size_t len){
        if(avail()>static_cast<int>(len)){
            memcpy(cur_,buf,len);
            add(len);
        }
    }

    const char* data() const {return data_;}
    int length() const {return static_cast<int>(cur_-data_);}
    char current(){return cur_;}
    int avail() const {return static_cast<int>(end()-cur_);}
    void add(size_t len){cur_+=len;}
    void reset(){cur_=data_;}
    void bzero(){memset(data_,0,sizeof data_);}

private:
    const char* end() const{
        return data_+sizeof(data_);
    }
    char data_[SIZE];
    char* cur_;
};


#endif
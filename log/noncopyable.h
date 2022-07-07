/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-22 14:35:08
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-27 14:46:54
 */
#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H

class noncopyable{
protected:
    noncopyable(){}
    ~noncopyable(){}

private:
    noncopyable(const noncopyable&)=delete;
    const noncopyable& operator=(const noncopyable&)=delete;
};

#endif
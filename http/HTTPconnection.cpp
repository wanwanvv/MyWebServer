/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-05-28 10:57:06
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-20 15:14:01
 */
#include "HTTPconnection.h"

bool HTTPconnection::isET;
const char* HTTPconnection::srcDir;
std::atomic<int> HTTPconnection::userCount;

HTTPconnection::HTTPconnection(){
    fd_=-1;
    addr_={0};
    isClose_=true;
}

HTTPconnection::~HTTPconnection(){
    closeHTTPConn();
}

/**
 * @name: 
 * @msg: 开启HTTP连接
 * @param {int} fd : connect socket
 * @param {sockaddr_in&} addr: 客户端的地址
 * @return {*}
 */
void HTTPconnection::initHTTPConn(int fd,const sockaddr_in& addr){
    assert(fd>0);
    userCount++;
    addr_=addr;
    fd_=fd;
    writeBuffer_.initPtr();
    readBuffer_.initPtr();
    isClose_=false;
}

//关闭HTTP连接
void HTTPconnection::closeHTTPConn(){
    response_.unmapFile_();
    if(isClose_==false){
        isClose_=true;
        userCount--;
        close(fd_);
    }
}

int HTTPconnection::getFd() const {
    return fd_;
};

struct sockaddr_in HTTPconnection::getAddr() const {
    return addr_;
}

const char* HTTPconnection::getIP() const {
    return inet_ntoa(addr_.sin_addr);//将用网络字节序整数表示的IPv4地址转化为用点分十进制字符串表示的IPV4地址
}

int HTTPconnection::getPort() const {
    return addr_.sin_port;
}

ssize_t HTTPconnection::readBuffer(int* saveErrno){
    ssize_t len=-1;
    do{
        len=readBuffer_.readFd(fd_,saveErrno);
        if(len<=0){
            break;
        }else{
            std::cout<<"readFd:len="<<len<<std::endl;
        }
    }while(isET);
    return len;
}

ssize_t HTTPconnection::writeBuffer(int* saveErrno){
    ssize_t len=-1;
    do{
        len=writev(fd_,iov_,iovCnt_);
        std::cout<<"writev len="<<len<<std::endl;
        if(len <= 0) {
            *saveErrno = errno;
            break;
        }
       if(iov_[0].iov_len + iov_[1].iov_len  == 0) { break; } /* 传输结束 */
       else if(static_cast<size_t>(len)>iov_[0].iov_len){
            iov_[1].iov_base=(uint8_t*)iov_[1].iov_base+(len-iov_[0].iov_len);
            iov_[1].iov_len-=(len-iov_[0].iov_len);
            if(iov_[0].iov_len){
                writeBuffer_.initPtr();
                iov_[0].iov_len=0;
            }
       }else{
           iov_[0].iov_base=(uint8_t *)iov_[0].iov_base+len;
           iov_[0].iov_len-=len;
           writeBuffer_.updateReadPtr(len);
       }
    }while(isET|| writeBytes() > 10240);
    return len;
}

bool HTTPconnection::handleHTTPConn()
{
    request_.init();
    if(readBuffer_.readableBytes()<=0){
        std::cout<<"readBuffer is empty!"<<std::endl;
        return false;
    }else if(request_.parse(readBuffer_)){
        response_.init(srcDir,request_.path(),request_.isKeepAlive(),200);
    }else{
        std::cout<<"400!"<<std::endl;
        response_.init(srcDir,request_.path(),false,400);
    }
    response_.makeResponse(writeBuffer_);
    /* 响应头存在iov_[0] */
    iov_[0].iov_base=const_cast<char*>(writeBuffer_.curReadPtr());
    iov_[0].iov_len = writeBuffer_.readableBytes();
    iovCnt_ = 1;

    /* 文件存在iov_[1] */
    if(response_.fileLen()>0&&response_.file()){
        iov_[1].iov_base=response_.file();
        iov_[1].iov_len=response_.fileLen();
        iovCnt_=2;
    }
    return true;
}
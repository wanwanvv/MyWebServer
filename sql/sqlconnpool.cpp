/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-07-12 17:06:36
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-07-12 20:44:29
 */
#include "sqlconnpool.h"

SqlConnPool::SqlConnPool(){
    useCount_=0;
    freeCount_=0;
}

SqlConnPool* SqlConnPool::Instance(){
    static SqlConnPool connPool;
    return &connPool;
}

void SqlConnPool::init(const char* host, int port,const char* user, const char* pwd,const char* dbName, int connSize=10){
    assert(connSize>0);
    for(int i=0;i<connSize;++i){
        //初始化函数
        MYSQL* sql=nullptr;
        sql=mysql_init(sql); //参数可以传null
        //可用int mysql_options设置连接选项
        if(!sql){
            std::cout<<"Mysql init error!"<<std::endl;
            assert(sql);
        }
        //连接数据库
        /****
        mysql 初始化的句柄指针
        host 主机地址
        user 用户名 – mysql数据库 root
        passwd 用户的密码
        db 要连接的数据库
        port 端口 一般填0,mysql默认端口 3306
        unix_socket 本地套接字 ,一般填NULL
        client_flag 连接标志 一般填0
        ****/
        sql=mysql_real_connect(sql,host,user,pwd,dbName,port,nullptr,0);
        if(!sql){
            std::cout<<"Mysql connect error!"<<std::endl;
            assert(sql);
        }
        connQue_.push(sql);
    }
    MAX_CONN_=connSize;
    //0表示当前进程的局部信号量，最后一个参数指定信号量的初始值
    sem_init(&semld_,0,MAX_CONN_); 
}

MYSQL* SqlConnPool::getConn(){
    MYSQL* sql=nullptr;
    if(connQue_.empty()){
        std::cout<<"SqlConnPool busy!"<<std::endl;
        return nullptr;
    }
    sem_wait(&semld_);
    {
        std::lock_guard<std::mutex> lk(mutex_);
        sql=connQue_.front();
        connQue_.pop();
    }
    return sql;
}

int SqlConnPool::getFreeConnCount(){
    std::lock_guard<std::mutex> lk(mutex_);
    return connQue_.size();
}

void SqlConnPool::freeConn(MYSQL* sql){
    assert(sql);
    std::lock_guard<std::mutex> lk(mutex_);
    connQue_.push(sql);
    sem_post(&semld_);
}

void SqlConnPool::closePool(){
    std::lock_guard<std::mutex> lk(mutex_);
    while(!connQue_.empty()){
        auto item = connQue_.front();
        connQue_.pop();
        //释放连接资源
        mysql_close(item);
    }
}

SqlConnPool::~SqlConnPool(){
    closePool();
}
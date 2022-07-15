/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-07-12 17:06:27
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-07-12 19:20:35
 */
#ifndef SQL_CONN_POOL_H
#define SQL_CONN_POOL_H

#include<mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <semaphore.h>
#include <assert.h>
#include <iostream>

class SqlConnPool{
public:
    static SqlConnPool *Instance();

    MYSQL* getConn();
    void freeConn(MYSQL* conn);
    int getFreeConnCount();

    void init(const char* host, int port,
     const char* user, const char* pwd,
     const char* dbName, int connSize);
    
    void closePool();

private:
    SqlConnPool();
    ~SqlConnPool();

    int MAX_CONN_;
    int useCount_;
    int freeCount_;

    std::queue<MYSQL*> connQue_;
    std::mutex mutex_;
    sem_t semld_;
};

#endif
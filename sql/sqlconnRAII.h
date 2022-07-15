/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-07-12 17:07:57
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-07-12 20:04:29
 */
#ifndef SQL_CONN_RAII_H
#define SQL_CONN_RAII_H
#include "sqlconnpool.h"


/*RAII机制，通过在栈上创建临时变量，这样临时变量就接管了堆上内存的控制权
当该临时变量声明周期结束时，则对应的堆上内存自然就被释放了。
资源在对象构造初始化 资源在对象析构时释放*/

class SqlConnRAII{
public:
    SqlConnRAII(MYSQL** sql, SqlConnPool* connpool){
        assert(connpool);
        *sql=connpool->getConn();
        sql_=*sql;
        connpool_=connpool;
    }

    ~SqlConnRAII(){
        if(sql_){connpool_->freeConn(sql_);}
    }
private:
    MYSQL* sql_;
    SqlConnPool* connpool_;
};

#endif
/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-05-22 09:44:24
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-07-14 11:11:29
 */
#include <unistd.h>
#include "webserver.h"
using namespace std;

int main(int argc, char* args[]){
    //守护进程，后台运行
    //daemon(1,0);
    WebServer server(50080,3,60000,false,4,/* 端口 ET模式 timeoutMs 优雅退出 线程数  */
    "127.0.0.1",3306,"root","123456","webserver",12, /* 数据库配置 */
    true); /* 是否开启ssl */
    server.Start();
    return 0;
}   

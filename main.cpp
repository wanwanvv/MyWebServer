/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-05-22 09:44:24
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-21 21:24:44
 */
#include <unistd.h>
#include "webserver.h"
using namespace std;

int main(int argc, char* args[]){
    //守护进程，后台运行
    //daemon(1,0);
    //int port,int trigMode,int timeoutMS,bool optLinger,int threadNum
    WebServer server(50080,3,60000,false,4);
    server.Start();
    return 0;
}   

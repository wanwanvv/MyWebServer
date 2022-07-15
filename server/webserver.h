/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-05-22 10:04:15
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-07-14 11:07:00
 */
#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "epoller.h"
#include "timer.h"
#include "threadpool.h"
#include "HTTPconnection.h"

class WebServer{
public:
    WebServer(int port,int trigMode,int timeoutMS,bool optLinger,int threadNum,const char* sqlIP,int sqlPort, const char* sqlUser, const  char* sqlPwd,
            const char* dbName, int connPoolNum, bool sslFlag=false);
    ~WebServer();
    void Start(); //启动服务

private:

    //对服务端的监听socket进行设置，得到listenFd_
    bool initSocket_();
    //事件模式的初始化
    void initEventMode_(int trigMode);

    //添加一个HTTP连接
    void addClientConnection(int fd,sockaddr_in addr);
    //关闭一个HTTP连接
    void closeConn_(HTTPconnection* client);

    //处理客户端请求
    void handleListen_();
    void handleWrite_(HTTPconnection* client);
    void handleRead_(HTTPconnection* client);
    void handleSSLConn_(HTTPconnection* client);

    //发送错误信息给客户端
    void sendError_(int fd,const char* info);
    void extentTime_(HTTPconnection* client);
    
    void onRead_(HTTPconnection* client);
    void onWrite_(HTTPconnection* client);
    void onProcess_(HTTPconnection* client);

    static const int MAX_FD = 65536;
    static int setFdNonblock(int fd);

    //***********variables***********
    int port_;
    int timeoutMS_;//毫秒，定时器默认过期时间
    bool isClose_;
    int listenFd_;
    bool openLinger_;
    char* srcDir_;//需要获取的路径
    

    uint32_t listenEvent_;
    uint32_t connectionEvent_;

    std::unique_ptr<ThreadPool> threadpool_;
    std::unique_ptr<TimerManager> timer_;
    std::unique_ptr<Epoller> epoller_;
    std::unordered_map<int,HTTPconnection> users_;
};

#endif //WEBSERVER_H
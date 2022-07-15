/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-05-22 10:03:52
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-07-14 11:07:17
 */

#include "webserver.h"

WebServer::WebServer(int port,int trigMode,int timeoutMS,bool optLinger,int threadNum,const char* sqlIP,int sqlPort, const char* sqlUser, const  char* sqlPwd,
            const char* dbName, int connPoolNum,bool sslFlag):port_(port),openLinger_(optLinger),timeoutMS_(timeoutMS),isClose_(false),
    timer_(new TimerManager()),threadpool_(new ThreadPool(threadNum)),epoller_(new Epoller())
{
    //获取当前工作目录的绝对路径
    srcDir_=getcwd(nullptr,256);
    assert(srcDir_);
    //拼接字符串
    strncat(srcDir_,"/resources/",16);
    HTTPconnection::userCount=0;
    HTTPconnection::srcDir=srcDir_;
    std::cout<<"srcDir_="<<srcDir_<<std::endl;
    //初始化数据库连接池
    SqlConnPool::Instance()->init(sqlIP, sqlPort, sqlUser, sqlPwd, dbName, connPoolNum);
    initEventMode_(trigMode);
    if(!initSocket_()) isClose_=true;
    if(sslFlag){
        //初始化 SSL 算法库函数，调用 SSL 系列函数之前必须调用此函数
        SSL_library_init ();
        //加载 SSL 错误消息
        SSL_load_error_strings ();
        // // 加载 BIO 抽象库的错误信息
        // ERR_load_BIO_strings();
        // // 加载所有 加密 和 散列 函数
        // OpenSSL_add_all_algorithms();
        //创建SSL_CTX结构体指针
        HTTPconnection::ctx=SP_SSL_CTX(SSL_CTX_new(SSLv23_method()),SSL_CTX_free);
        //载入用户的数字证书， 此证书用来发送给服务端端。 证书里包含有公钥 
        if (SSL_CTX_use_certificate_file(HTTPconnection::ctx.get(), "../ssl/ca.crt", SSL_FILETYPE_PEM) <= 0) {
            std::cout<<"ssl_ctx_use_certificate_file failed"<<std::endl;
	    }
        if (SSL_CTX_use_PrivateKey_file(HTTPconnection::ctx.get(), "../ssl/ca.key", SSL_FILETYPE_PEM) <= 0) {
            std::cout<<"ssl_ctx_use_privatekey_file failed"<<std::endl;
	    }
        if (SSL_CTX_check_private_key(HTTPconnection::ctx.get())<= 0) {
            std::cout<<"ssl_ctx_check_privatekey_file failed"<<std::endl;
	    }
        HTTPconnection::isSSL=true;
    }else{
        HTTPconnection::isSSL=false;
    }
}

WebServer::~WebServer()
{
    close(listenFd_);
    isClose_=true;
    free(srcDir_);
}

void WebServer::initEventMode_(int trigMode)
{
    listenEvent_=EPOLLRDHUP;//EPOLLRDHUP:流套接字对等端关闭连接或关闭写入连接的一半（用于ET模式下编写代码检测对等关闭）
    connectionEvent_ = EPOLLONESHOT | EPOLLRDHUP;//EPOLLONESHOT:只触发一次，事件自动被删除

    switch (trigMode)
    {
    case 0:
        break;
    case 1:
        connectionEvent_ |= EPOLLET;
        break;
    case 2:
        listenEvent_ |= EPOLLET;//edege trigered边缘触发
        break;
    case 3:
        listenEvent_ |= EPOLLET;
        connectionEvent_ |= EPOLLET;
        break;
    default:
        listenEvent_ |= EPOLLET;
        connectionEvent_ |= EPOLLET;
        break;
    }
    HTTPconnection::isET = (connectionEvent_ & EPOLLET);
}

bool WebServer::initSocket_(){
    int ret;
    /* sockaddr_in 存放套接字地址，in_表示internet
      * Port number.  
      * Internet address. 
      * Pad to size of `struct sockaddr'.  
      */
    struct sockaddr_in addr;
    if(port_>65535||port_<1024){
        std::cout<<"port ="<<port_<<" error!"<<std::endl;
        return false;
    }
    addr.sin_family=AF_INET;//协议族，AF_INET表示采用IPV4地址(32位)与端口号(16位)的组合
    addr.sin_addr.s_addr=htonl(INADDR_ANY);/* Address to accept any incoming messages.  */
    addr.sin_port=htons(port_);
    /*
    *tcp连接断开的时候调用closesocket函数。设置断开的连接方式
    */
   //(1).optLinger.l_onoff = 0;optLinger.l_linger = 忽略;立刻返回，会将未发送完的数据发送完再释放资源，即优雅的断开
    //(1).optLinger.l_onoff = 非零;optLinger.l_linger = 0;立刻返回，不会发送未发送完的数据，即强制的断开
    struct linger optLinger={0};
    if(openLinger_) {
        /* 优雅关闭: 直到所剩数据发送完毕或超时 */
        /**
         * 不会立刻返回，内核会延迟一段时间，直到l_linger时间超时或数据发送完成；
         * 套接字必须设置为阻塞；
         * 在超时时间内保持尝试发送，若超时则立即放弃
         */
        optLinger.l_onoff = 1;//开关
        optLinger.l_linger = 1;//表示容许逗留的时间(1s)
    }
    listenFd_=socket(AF_INET,SOCK_STREAM,0);
    if(listenFd_<0){
        return false;
    }

    // 主要设置socket的一些属性，第二个参数level是被设置的选项的级别，SOL——SOCKET指在套接字级别设置选项
    //SO_LINGER ：设置套接字里排队的消息成功发送或者到达超时时间才会返回，否则立即返回
    ret=setsockopt(listenFd_,SOL_SOCKET,SO_LINGER, &optLinger, sizeof(optLinger));
    if(ret<0){
        close(listenFd_);
        return false;
    }

    int optval=1;
    /* SO_REUSEADDR: 打开或关闭地址复用功能，不等于0表示打开端口复用 */
    /* 只有最后一个套接字会正常接收数据。 */
    ret = setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
    if(ret==-1){
        std::cout<<"set socket setsockopt error!"<<std::endl;
        close(listenFd_);
        return false;
    }

    /**
     * @brief 
     * @sockaddr: 为了接受各种类型的套接字地址结构的指针，而C当时没有void*
     * 因此无论何时需要将sockaddr_in强制转换成sockaddr
     */
    ret=bind(listenFd_,(struct sockaddr*)&addr,sizeof(addr));
    if(ret < 0) {
        //std::cout<<"Bind Port"<<port_<<" error!"<<std::endl;
        close(listenFd_);
        return false;
    }

    //listen告诉内核该描述符是被服务器使用的，即从主动套接字转化为监听套接字
    //backlog=6表示在开始拒绝连接请求前队列中要排队的未完成的连接请求的数量
    ret=listen(listenFd_,6);
    if(ret<0){
        std::cout<<"Listen port="<<port_<<" error!"<<std::endl;
        close(listenFd_);
        return false;
    }

    ret=epoller_->addFd(listenFd_,listenEvent_|EPOLLIN);
    if(ret==0){
        close(listenFd_);
        return false;
    }
    setFdNonblock(listenFd_);
    return true;

}

void WebServer::Start()
{
    int timeMS=-1;//epoll wait timeout==-1就是无事件一直阻塞
    if(!isClose_){
        std::cout<<"============================";
        std::cout<<"Server Start!";
        std::cout<<"============================";
        std::cout<<std::endl;
    }
    while(!isClose_){
        if(timeoutMS_>0)
        {
            timeMS=timer_->getNextHandle();
        }
        //调用epoll_wait后events_中存储已就绪事件
        int eventCnt=epoller_->wait(timeMS);
        for(int i=0;i<eventCnt;++i){
            //获取已就绪事件的fd
            int fd=epoller_->getEventFd(i);
            //获取已就绪事件的类型
            uint32_t events=epoller_->getEvents(i);
            if(fd==listenFd_){
                std::cout<<"*********************************listening start***************************"<<std::endl;
                handleListen_();
                std::cout<<"listenFd_="<<fd<<" is listening!"<<std::endl;
                std::cout<<"*********************************listening end***************************"<<std::endl;
            }else if(events&(EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
                 /**
                 * EPOLLRDHUP: TCP连接被对方关闭
                 * EPOLLHUP: 挂起
                 * EPOLLERR: 错误
                 */
                std::cout<<"*********************************close start***************************"<<std::endl;
                assert(users_.count(fd));
                std::cout<<"Ready event: IP="<<users_[fd].getIP()<<":"<<users_[fd].getPort()<<" fd="<<users_[fd].getFd()<<" events="<<events<<" port=EPOLLRDHUP | EPOLLHUP | EPOLLERR!"<<std::endl;
                closeConn_(&users_[fd]);
                std::cout<<fd<<" close!"<<std::endl;
                std::cout<<"*********************************close end***************************"<<std::endl;
            }else if(events&EPOLLIN){//EPOLLIN:数据（普通和优先）可读
                std::cout<<"*********************************read start***************************"<<std::endl;
                assert(users_.count(fd)>0);
                std::cout<<"Ready event: IP="<<users_[fd].getIP()<<":"<<users_[fd].getPort()<<" fd="<<users_[fd].getFd()<<" events=EPOLLIN!"<<std::endl;
                if(events&EPOLLET) handleRead_(&users_[fd]);
                else handleSSLConn_(&users_[fd]);
                std::cout<<"*********************************read end***************************"<<std::endl;
            }else if(events&EPOLLOUT){
                std::cout<<"*********************************write start***************************"<<std::endl;
                assert(users_.count(fd)>0);
                std::cout<<"Ready event: IP="<<users_[fd].getIP()<<":"<<users_[fd].getPort()<<" fd="<<users_[fd].getFd()<<" events=EPOLLOUT!"<<std::endl;
                handleWrite_(&users_[fd]);
                std::cout<<"*********************************write end***************************"<<std::endl;
            }else{
                std::cout<<"*********************************unexpected start***************************"<<std::endl;
                std::cout<<"Unexpected event"<<std::endl;
                std::cout<<"*********************************unexpected end***************************"<<std::endl;
            }
        }
    }
}

void WebServer::handleListen_()
{
    struct sockaddr_in addr;
    socklen_t len=sizeof(addr);
    do{
        //addr用来获取被接受连接的远端socket地址，并返回一个新的连接socket
        int fd=accept(listenFd_,(struct sockaddr*)&addr,&len);
        std::cout<<"accept fd="<<fd<<std::endl;
        if(fd<0){return;}
        else if(HTTPconnection::userCount>=MAX_FD){
            sendError_(fd,"Server busy!");
            std::cout<<"userCount="<<HTTPconnection::userCount<<", Clients is full!"<<std::endl;
            return;
        }
        addClientConnection(fd,addr);
    }while(listenEvent_&EPOLLET);
}

void WebServer::handleRead_(HTTPconnection* client) 
{
    assert(client);
    //延长超时时间
    extentTime_(client);
    //向队列中提交执行事件
     threadpool_->submit(std::bind(&WebServer::onRead_,this,client));
}

void WebServer::handleSSLConn_(HTTPconnection* client){
    int result=SSL_do_handshake(client->getSSL().get());
    if(1==result){
        client->setSSLConnect(true);
        epoller_->addFd(client->getFd(),EPOLLIN|connectionEvent_);
        return;
    }
    int error=SSL_get_error(client->getSSL().get(),result);
	if (SSL_ERROR_WANT_WRITE==error){
        epoller_->addFd(client->getFd(),EPOLLOUT|connectionEvent_);
	}else if (SSL_ERROR_WANT_READ==error){
        epoller_->addFd(client->getFd(),EPOLLIN);
	}else {
		std::cout<<"SSL handshake error!"<<std::endl;
		closeConn_(client);
    }
    return;
}

void WebServer::handleWrite_(HTTPconnection* client){
    assert(client);
    extentTime_(client);
    threadpool_->submit(std::bind(&WebServer::onWrite_,this,client));
}

//关闭连接：1）将俩连接fd从epoll中删除不再监听；2）设置连接对象属性
void WebServer::closeConn_(HTTPconnection* client){
    assert(client);
    std::cout<<"client"<<client->getFd()<<" quit!"<<std::endl;
    epoller_->delFd(client->getFd());
    client->closeHTTPConn();
}

void WebServer::addClientConnection(int fd,sockaddr_in addr)
{
    assert(fd>0);
    users_[fd].initHTTPConn(fd,addr);
    if(timeoutMS_>0){
        timer_->addTimer(fd,timeoutMS_,std::bind(&WebServer::closeConn_,this,&users_[fd]));
    }
    //读事件/水平触发，并把这个连接socket对应的fd添加到epoll_pool中
    if(HTTPconnection::isSSL&&!users_[fd].getSSLConnect()) epoller_->addFd(fd,EPOLLIN);
    else epoller_->addFd(fd,EPOLLIN|connectionEvent_);
    setFdNonblock(fd);
}

/**
 * @fcntl：用来管理文件描述符的API，可以设置非阻塞，加读写锁等
 * 设置非阻塞需要先把fd对应的标志flags读出来然后加上非阻塞标志O_NONBLOCK，再设置回去
 * fcntl (int __fd, int __cmd, ...)
 * @param fd : 文件描述符
 * @param cmd：具体操作
 * @return int ：
 */
int WebServer::setFdNonblock(int fd) {
    assert(fd > 0);
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}

void WebServer::sendError_(int fd,const char* info)
{
    assert(fd>0);
    int ret=send(fd,info,strlen(info),0);
    if(ret<0){
        std::cout<<"send error to client"<<fd<<" error!"<<std::endl;
    }
    close(fd);
}

void WebServer:: extentTime_(HTTPconnection* client){
    assert(client);
    if(timeoutMS_>0){
        timer_->update(client->getFd(),timeoutMS_);
    }
}

void WebServer::onRead_(HTTPconnection* client)
{
    assert(client);
    int ret=-1;
    int readErrno=0;
    ret=client->readBuffer(&readErrno);
    //std::cout<<"read "<<ret<<" bytes!"<<std::endl;
    if(ret<0&&readErrno!=EAGAIN){
        //如果读取错误，关闭连接
        closeConn_(client);
        return;
    }
    //如果读取数据成功，继续处理
    onProcess_(client);
}

void WebServer::onWrite_(HTTPconnection* client)
{
    assert(client);
    int ret=-1;
    int writeErrno=0;
    ret=client->writeBuffer(&writeErrno);
    if(client->writeBytes()==0){
        /* 传输完成 */
        if(client->isKeepAlive()) {
            onProcess_(client);
            std::cout<<"write finshed onProcess..."<<std::endl;
            return;
        }
    }else if(ret<0){
        if(writeErrno == EAGAIN) {
            /* 继续传输 */
            epoller_->modFd(client->getFd(), connectionEvent_ | EPOLLOUT);
            return;
        }
    }
    closeConn_(client);
}

void WebServer::onProcess_(HTTPconnection* client)
{
    if(client->handleHTTPConn()){
        //如果向缓冲区写入数据，改为写事件
        epoller_->modFd(client->getFd(),connectionEvent_|EPOLLOUT);
    }else{
        //否则还是读事件
        epoller_->modFd(client->getFd(),connectionEvent_|EPOLLIN);
    }
}



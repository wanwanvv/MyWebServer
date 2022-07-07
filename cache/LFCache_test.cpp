/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-22 20:45:38
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-25 20:41:08
 */

#include <unistd.h>      
#include <fcntl.h> 
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <vector>
#include <thread>
#include "LFUCache.h"

void makeResponse(string path){
    auto ret=getCache().get(path);
    if(!ret.first){
        int srcFd=open(path.data(),O_RDONLY);
        struct stat fileStat;
        if(srcFd<0){
            std::cout<<path<<" File Notfound!"<<std::endl;
            return;
        }
        stat(path.data(),&fileStat);
        int* mmRet=(int*) mmap(0,fileStat.st_size,PROT_READ, MAP_PRIVATE,srcFd,0);
        if(*mmRet==-1){
            std::cout<<path<<" File Notfound!"<<std::endl;
            return;
        }
        char* mmFile=(char*)mmRet;
        close(srcFd);
        getCache().set(path,mmFile,fileStat.st_size);
        std::cout<<"Cache miss "<<path<<", cache_size="<<getCache().getSize()<<std::endl;
    }else{
        std::cout<<"Cache hit "<<path<<", cache_len="<<ret.second<<std::endl;
    }
    sleep(3);
    return;
}

int main(){
    string srcDir="../resources";
    std::vector<std::string> paths={"/index.html","/welcome.html","/400.html","/index.html","/welcome.html","/403.html","/404.html","/405.html","/error.html","/index.html","/login.html","/picture.html","/register.html","/video.html","/welcome.html"};
    //std::vector<std::thread> threads;
    getCache().init(5);
    for(int i=0;i<paths.size();++i){
        makeResponse(srcDir+paths[i]);
        //threads.emplace_back(th);
    }
    return 0;
}
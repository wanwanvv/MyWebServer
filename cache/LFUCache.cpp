/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-22 16:01:08
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-25 20:49:54
 */
#include "LFUCache.h"

//********************LFUCache***************
LFUCache::LFUCache(){
    capacity=0;
    min_freq=0;
    size=0;
}

void LFUCache::init(int cap){
    capacity=cap;
    min_freq=0;
    size=0;
}

 int LFUCache::getSize(){
    return size;
}

void LFUCache::set(const string& key, char* header,  size_t header_len, char* body,size_t body_len){
    size_t total_len=header_len+body_len;
    char* new_content=new char[1000000];
    //std::cout<<"header_len="<<header_len<<" body_len="<<body_len<<std::endl;
    memcpy(  new_content, header,header_len );
    //std::cout<<"copy header sucessfully!"<<std::endl;
    if(body) memcpy( new_content+header_len,body, body_len );
    //std::cout<<"copy body sucessfully!"<<std::endl;
    //拷贝到新开辟内存的缓存节点
    cache_node* new_node=new cache_node();
    new_node->getValue().init(key,new_content,total_len,1);
    std::cout<<"total_len="<<total_len<<std::endl;
    //如果超出缓存大小，先移除节点
    {
        std::unique_lock<std::mutex> lk(cache_mutex);
        if(size>=capacity){
            freq_list* cur_list=freq_map[min_freq];
            cur_list->deleteLast();
            if(cur_list->isEmpty()) freq_map.erase(min_freq);
            --size;
        }
        //将新节点添加到key_map中
        key_map[key]=new_node;
        //将新节点插入对应freq_list的头结点
        if(!freq_map.count(min_freq)) freq_map[min_freq]=new freq_list();
        freq_map[min_freq]->addToHead(new_node);
        //更新min_freq和size
        min_freq=1;
        ++size;
    }
    return;
}

std::pair<char*,size_t> LFUCache::get(const string& key){
    if(!capacity) return std::make_pair(nullptr,0);
    else if(key_map.find(key)==key_map.end()) return std::make_pair(nullptr,0);
    else{
        cache_node* cur_node=key_map[key];
        int cur_freq=cur_node->getValue().getFreq();
        //更新LFU
        {
            std::unique_lock<std::mutex> lk(cache_mutex);
            freq_list* cur_list=freq_map[cur_freq];
            cur_list->removeCur(cur_node);
            if(cur_list->isEmpty()){
                if(min_freq==cur_freq) ++min_freq;
                freq_map.erase(cur_freq);
            }
            ++cur_freq;
            if(!freq_map.count(cur_freq)) freq_map[cur_freq]=new freq_list();
            freq_map[cur_freq]->addToHead(cur_node);
        }
        return std::make_pair(cur_node->getValue().getContent(),cur_node->getValue().getLen());
    }
}

//********************NodeList***************
template<typename T> bool NodeList<T>::isEmpty(){
    return head->next==tail&&tail->prev==head;
}

template<typename T> void NodeList<T>::addToHead(T*& node){
    node->next=head->next;
    head->next->prev=node;
    head->next=node;
    node->prev=head;
    return;
}

template<typename T> void NodeList<T>::removeCur(T*& node){
    node->prev->next=node->next;
    node->next->prev=node->prev;  
}

template<typename T> void NodeList<T>::deleteLast(){
    T* node=tail->prev;
    removeCur(node);
    node->destroy();
}

LFUCache& getCache(){
    return cache;
}
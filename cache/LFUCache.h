/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-22 16:00:58
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-07-08 10:18:12
 */
#ifndef LFU_CACHE_H
#define LFU_CACHE_H

#include <unordered_map>
#include <string>
#include <string.h>
#include <mutex>
 #include <sys/uio.h>
 #include <iostream>

using std::string;
using std::unordered_map;

template <typename T>
class Node{
public:
    T val;
    Node* prev=nullptr;
    Node* next=nullptr;
    ~Node(){
        val.destroy();
    }

    void destroy(){
        val.destroy();
    }

    void setPre(Node *p){
		prev=p;
	}

	void setNext(Node *n){
		next=n;
	}

	Node* getPre(){
		return prev;
	}

	Node* getNext(){
		return next;
	}	

    T& getValue(){
        return val;
    }
};

template<typename T>
class NodeList{
private:
    T* head;
    T* tail;

public:
    //构造函数
    NodeList(){
        head=new T();
        tail=new T();
        head->next=tail;
        tail->prev=head;
    }

    ~NodeList(){
        while(head){
            head->~T();
            head=head->next;
        }
    }
    //调用接口
    bool isEmpty();
    void addToHead(T*& node);
    void removeCur(T*& node);
    void deleteLast();
};

class CacheContent{
private:
        string key;
        char* content;
        size_t len;
        int freq;

public:
        CacheContent():key(""),content(nullptr),len(0),freq(0){}
        ~CacheContent(){
            if(content){
                delete content;
                content=nullptr;
            }
        }

        void destroy(){
            if(content){
                delete content;
            }
        }
        
        void init(string k, char*& c,size_t l,int f){
            key=k;
            content=c;
            c=nullptr;
            len=l;
            freq=f;
            return;
        }

        string getKey(){
            return key;
        }

        char* getContent(){
            return content;
        } 

        size_t getLen(){
            return len;
        }

        int getFreq(){
            return freq;
        }
};

typedef Node<CacheContent> cache_node;
typedef NodeList<cache_node> freq_list;

class LFUCache{
public:
    std::pair<char*,size_t> get(const string& key);//通过key返回value并进行LFU操作
	void set(const string& key, char* header,  size_t header_len, char* body=nullptr,size_t body_len=0);//更新LFU缓存
    void init(int cap);
    LFUCache();
   // ~LFUCache();
    int getSize();

private:
    int capacity;
    int size;
    int min_freq;
    std::mutex cache_mutex;
    unordered_map<int,freq_list*> freq_map;
    unordered_map<string,cache_node*> key_map;

};

//类相关函数
static LFUCache cache;
LFUCache& getCache();
#endif
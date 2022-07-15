/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-05-29 09:16:02
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-07-14 10:50:52
 */
#include "HTTPrequest.h"

//允许的请求URI

const std::unordered_set<std::string> HTTPrequest::DEFAULT_HTML
{
    "/index", "/welcome", "/video", "/picture","/register","/login"
};


const std::unordered_map<std::string,int > HTTPrequest::DEFAULT_HTML_TAG{
    {"/register.html",0},{"/login.html",1}
};

void HTTPrequest::init()
{
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    header_.clear();
    post_.clear();
}

bool HTTPrequest::isKeepAlive() const
{
    if(header_.count("Connection")==1){
         return header_.find("Connection")->second == "keep-alive" && version_ == "1.1";
    }
    return false;
}

/**************解析HTTP请求中的数据**********/
bool HTTPrequest::parse(Buffer& buff)
{
    const char CRLF[]="\r\n";
    if(buff.readableBytes()<=0) return false;
    // std::cout<<"parse buff start:"<<std::endl;
    // buff.printContent();
    // std::cout<<"parse buff finish:"<<std::endl;
    //逐行读取
    while(buff.readableBytes()&&state_!=FINISH){
        //当函数查找成功时，该迭代器指向查找到的子序列中的第一个元素
        const char* lineEnd=std::search(buff.curReadPtr(),buff.curWritePtrConst(),CRLF,CRLF+2);
        std::string line(buff.curReadPtr(),lineEnd);
        //state_初始化为REQUEST_LINE，每次读取后依次改变
        switch (state_)
        {
        case REQUEST_LINE:
            std::cout<<"REQUEST: "<<line<<std::endl;
            if(!parseRequestLine_(line)){
                return false;
            }
            parsePath_();
            break;
        case HEADERS:
            std::cout<<"HEADER: "<<line<<std::endl;
            parseRequestHeader_(line);
            if(buff.readableBytes() <= 2) {
                state_ = FINISH;
            }
            break;
        case BODY:
            std::cout<<"BODY: "<<line<<std::endl;
            parseDataBody_(line);
            break;
        default:
            break;
        }
        if(lineEnd==buff.curWritePtr()) break;
        buff.updateReadPtrUntilEnd(lineEnd+2);
    }
    return true;
}

bool HTTPrequest::parseRequestLine_(const std::string& line)
{
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
    if(std::regex_match(line,subMatch,patten)){
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = HEADERS;
        std::cout<<"RequestLine: method_="<<method_<<" path_="<<path_<<" version_="<<version_<<std::endl;
        return true;
    }
    return false;
}

void HTTPrequest::parseRequestHeader_(const std::string& line)
{
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if(regex_match(line, subMatch, patten)) {
        header_[subMatch[1]] = subMatch[2];
    }
    else {
        state_ = BODY;
    }
}

void HTTPrequest::parseDataBody_(const std::string& line)
{
    body_ = line;
    parsePost_();
    state_=FINISH;
}

void HTTPrequest::parsePath_()
{
    if(path_=="/"){
        path_="/index.html";
    }else{
        for(auto &item:DEFAULT_HTML){
            if(item==path_){
                path_+=".html";
                break;
            }
        }
    }
}

void HTTPrequest::parseFromUrlencoded_(){
    if(body_.size()==0) return;
    std::string key,value;
    int num=0;
    int n=body_.size();
    int i=0,j=0;
    for(;i<n;++i){
        char ch=body_[i];
        switch (ch)
        {
        case '=':
            key = body_.substr(j, i - j);
            j = i + 1;
            break;

        case '+':
            body_[i] = ' ';
            break;
        
        case '%':
            num=convertHex(body_[i+1])*16+convertHex(body_[i+2]);
            body_[i + 2] = num % 10 + '0';
            body_[i + 1] = num / 10 + '0';
            i += 2;
            break;
        
        case '&':
            value=body_.substr(j,i-j);
            j=i+1;
            post_[key]=value;
            break;

        default:
            break;
        }
    }
    assert(j<=i);
    if(post_.count(key)==0&&j<i){
        value=body_.substr(j,i-j);
        post_[key]=value;
    }
}

void HTTPrequest::parsePost_()
{
    if(method_=="POST"&&header_["Content-Type"]=="application/x-www-form-urlencoded"){
        parseFromUrlencoded_();
        if(DEFAULT_HTML_TAG.count(path_)){
            int tag=DEFAULT_HTML_TAG.find(path_)->second;
            std::cout<<"tag="<<tag<<" "<<DEFAULT_HTML_TAG.find(path_)->first<<std::endl;
            if(tag==0||tag==1){
                bool isLogin=(tag==1);
                if(UserVerify(post_["username"],post_["password"],isLogin)){
                    path_= "/welcome.html";
                }else{
                    path_="/error.html";
                }
            }
        }
    }
}

bool HTTPrequest::UserVerify(const std::string& name, const std::string& pwd, bool isLogin){
    if(name==""||pwd=="") return false;
    std::cout<<"Very name="<<name<<" pwd="<<pwd<<std::endl;
    MYSQL* sql;//连接句柄
    SqlConnRAII(&sql,SqlConnPool::Instance());
    assert(sql);
    bool flag=false;
    unsigned int j=0;
    char order[256]={0};
    MYSQL_FIELD* fields=nullptr;
    MYSQL_RES* res=nullptr;
    if(!isLogin) flag=true;
    /* 查询用户及密码 */
    snprintf(order, 256, "SELECT username, password FROM user WHERE username='%s' LIMIT 1", name.c_str());
    std::cout<<"Sql:"<<order<<std::endl;
    if(mysql_query(sql,order)){//成功返回0，失败返回非0
        mysql_free_result(res);//释放结果集
        return false;
    }
    res=mysql_store_result(sql);//获得结果集，失败返回null指针
    j=mysql_num_fields(res);
    fields=mysql_fetch_fields(res);
    while(MYSQL_ROW row=mysql_fetch_row(res)){
        std::cout<<"MYSQL ROW: "<<row[0]<<" "<<row[1]<<std::endl;
        std::string password(row[1]);
        //注册
        if(isLogin){
            if(pwd==password) {flag=true;}
            else{
                flag=false;
                std::cout<<"Pwd error!"<<std::endl;
            }
        }else{
            flag=false;
            std::cout<<"Usename exists!"<<std::endl;
        }
    }
    mysql_free_result(res);
    //注册行为，且用户名未被使用
    if(!isLogin&&flag){
        std::cout<<"Register!"<<std::endl;
        bzero(order,256);
        snprintf(order, 256,"INSERT INTO user(username, password) VALUES('%s','%s')", name.c_str(), pwd.c_str());
        std::cout<<"Sql:"<<order<<std::endl;
        if(mysql_query(sql,order)){
            std::cout<<"Insert error!"<<std::endl;
            flag=false;
        }
        flag=true;
    }
    SqlConnPool::Instance()->freeConn(sql);
    std::cout<<"User verify success!"<<std::endl;
    return flag;
}

int HTTPrequest::convertHex(char ch)
{
    if(ch >= 'A' && ch <= 'F') return ch -'A' + 10;
    if(ch >= 'a' && ch <= 'f') return ch -'a' + 10;
    return ch;
}

/**************获取HTTP请求中的数据**********/
std::string HTTPrequest::path() const{
    return path_;
}

std::string& HTTPrequest::path(){
    return path_;
}
std::string HTTPrequest::method() const {
    return method_;
}

std::string HTTPrequest::version() const {
    return version_;
}


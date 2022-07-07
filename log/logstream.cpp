/*
 * @Descripttion: 
 * @version: 
 * @Author: wanwanvv
 * @Date: 2022-06-27 14:50:53
 * @LastEditors: wanwanvv
 * @LastEditTime: 2022-06-28 10:06:29
 */
#include "logstream.h"

// From muduo
template <typename T>
size_t LogStream::convert(char buf[], T value) {
  T i = value;
  char* p = buf;

  do {
    int lsd = static_cast<int>(i % 10);
    i /= 10;
    *p++ = zero[lsd];
  } while (i != 0);

  if (value < 0) {
    *p++ = '-';
  }
  *p = '\0';
  std::reverse(buf, p);

  return p - buf;
}

//public functions
LogStream& LogStream::operator<<(bool v){
	buffer_.append(v?"1":"0",1);
	return *this;
}

LogStream& LogStream::operator<<(short v){
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned short v){
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(int v){
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned int v){
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long v){
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long v){
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(long long v){
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(unsigned long long v){
	formatInteger(v);
	return *this;
}

LogStream& LogStream::operator<<(float v){
	formatDecimal(v);
	return *this;
}

LogStream& LogStream::operator<<(double v){
	formatDecimal(v);
	return *this;
}

LogStream& LogStream::operator<<(long double v){
	formatDecimal(v);
	return *this;
}

LogStream& LogStream::operator<<(char v){
	buffer_.append(&v,1);
	return *this;
}

LogStream& LogStream::operator<<(const char *v){
	if(v)
		buffer_.append(v,strlen(v));
	else
		buffer_.append("(null)",6);
	return *this;
}

LogStream& LogStream::operator<<(const std::string& v){
	buffer_.append(v.c_str(),v.size());
	return *this;
}

void LogStream::append(const char *data,size_t len){
	buffer_.append(data,len);
}


//private functions

template<typename T>
void LogStream::formatInteger(T v){
    if (buffer_.avail() >= kMaxNumericSize) {
		size_t len = convert(buffer_.current(), v);
		buffer_.add(len);
  	}
}

template<typename T>
void LogStream::formatDecimal(T v){
    if (buffer_.avail() >= kMaxNumericSize) {
		int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12Lg", v);
		buffer_.add(len);
  	}
}
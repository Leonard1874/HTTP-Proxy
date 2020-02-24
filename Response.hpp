#include "ResponseParser.hpp"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>
#include <iostream>
#include <vector>
#include <map>
#include <list>

class Response{
private:
  ResponseParser myParser;
  std::string responseInfo;
  std::string type;
  std::string time;
  double freshtime;
  bool revalidate;
  
public:
  Response(const std::string& resInfo, const std::string& rtype, const std::string& rtime):myParser(), responseInfo(resInfo), type(rtype), time(rtime){
    freshtime = myParser.parseExpire(responseInfo);
    revalidate = myParser.needValidate(responseInfo);
  }
  
  //test 
  std::string getResponseInfo(){
    return responseInfo;
  }
  
  //for cache
  bool canCache(){
    if(type != "GET"){
      return false;
    }
    else if(responseInfo.find("HTTP/1.1 200 OK") == std::string::npos){
      return false;
    }
    else{
      return true;
    }
  }
};

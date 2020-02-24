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
  double arriveTime;
  double expireTime;
  bool revalidate;
  
public:
  Response(const std::string& resInfo, const std::string& rtype, const double rtime):myParser(), responseInfo(resInfo), type(rtype), arriveTime(rtime){
    expireTime = myParser.parseExpire(responseInfo) + arriveTime;
    revalidate = myParser.needValidate(responseInfo);
    std::cout << expireTime << std::endl;
  }
  
  //test 
  std::string getResponseInfo(){
    return responseInfo;
  }

  double getExpireTime(){
    return expireTime;
  }
  
  //for cache
  bool canCache(){
    if(type != "GET"){
      return false;
    }
    else if(responseInfo.find("HTTP/1.1 200 OK") == std::string::npos){
      return false;
    }
    else if(!myParser.canCache(responseInfo)){
      return false;
    }
    else{
      return true;
    }
  }
};

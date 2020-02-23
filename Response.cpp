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
  std::string responseInfo;
  std::string type;
  
public:
  Response(const std::string& resInfo, const std::string& rtype):responseInfo(resInfo), type(rtype){}

  //test 
  std::string getResponseInfo(){
    return responseInfo;
  }

  //for cache
  std::string getValue(){
    if(!canCache()){
      return "";
    }
    else{
      return responseInfo;
    }
  }
  
private:
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

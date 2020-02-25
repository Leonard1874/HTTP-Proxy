#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "ResponseParser.hpp"

class Response {
private:
  ResponseParser myParser;
  std::string responseInfo;
  std::string type;
  double arriveTime;
  double expireTime;
  bool revalidate;

public:
  Response(const std::string & resInfo, const std::string & rtype, const double rtime) :
    myParser(),
    responseInfo(resInfo),
    type(rtype),
    arriveTime(rtime) {
    expireTime = myParser.parseExpire(responseInfo) + arriveTime;
    revalidate = myParser.needValidate(responseInfo);
    std::cout << expireTime << std::endl;
  }

  //test
  std::string getResponseInfo() { return responseInfo; }

  double getExpireTime() { return expireTime; }

  bool getRevalidate() { return revalidate; }

  //for cache
  bool canCache() {
    if (type != "GET") {
      return false;
    }
    else if (responseInfo.find("HTTP/1.1 200 OK") == std::string::npos) {
      return false;
    }
    else if (!myParser.canCache(responseInfo)) {
      return false;
    }
    else {
      return true;
    }
  }
};

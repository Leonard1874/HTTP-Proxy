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
  std::string ETAG;
  std::string lastModified;

 public:
  Response(const std::string & resInfo, const std::string & rtype, const double rtime) :
      myParser(),
      responseInfo(resInfo),
      type(rtype),
      arriveTime(rtime) {
    //ETAG = "";
    // lastModified = "";
    expireTime = myParser.parseExpire(responseInfo) + arriveTime;
    revalidate = myParser.needValidate(responseInfo);
    std::cout << expireTime << std::endl;
  }
  //Get the etag or last modified
  void validateParser() {
    std::cout << "Starting parsing"
              << "-------------------------------------" << std::endl;
    size_t pos = responseInfo.find("Last-Modified:");
    if (pos != std::string::npos) {
      while (responseInfo[pos] != ' ') {
        pos++;
      }
      pos++;
      while (responseInfo[pos] != '\r') {
        lastModified += responseInfo[pos];
        pos++;
      }
    }
    size_t pos1 = responseInfo.find("ETag:");
    if (pos1 != std::string::npos) {
      while (responseInfo[pos1] != ' ') {
        pos1++;
      }
      pos1++;
      while (responseInfo[pos1] != '\r') {
        ETAG += responseInfo[pos1];
        pos1++;
      }
    }
  }

  //validation
  std::string getETAG() { return ETAG; }
  std::string getLastModified() { return lastModified; }

  //test
  std::string getResponseInfo() { return responseInfo; }

  double getExpireTime() { return expireTime; }

  bool getRevalidate() { return revalidate; }

  //for cache
  std::string canCache() {
    if (type != "GET") {
      return "GET";
    }
    else if (responseInfo.find("HTTP/1.1 200 OK") == std::string::npos) {
      return "NOT 200 OK";
    }
    else if (myParser.canCache(responseInfo) != "Can cache") {
      return myParser.canCache(responseInfo);
    }
    else {
      return "Can cache";
    }
  }
};

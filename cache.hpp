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
#include <unordered_map>
#include <vector>

#include "Response.hpp"
using namespace std;
class Cache {
private:
  class LRUNode {
  public:
    std::unordered_map<string, string>
    information;  //Storing all the information about the node
    std::string key;
    Response value;
    LRUNode(const std::string & rkey, const Response & rval) : key(rkey), value(rval) {}
  };

  std::map<std::string, std::list<LRUNode>::iterator> LRUMap;
  std::list<LRUNode> LRUlist;

public:
  size_t capacity;
  Cache(size_t rcapacity) : capacity(rcapacity) {}

  std::string get(const std::string & key, const double curTime) {
    if (LRUMap.count(key) == 0) {
      return "notfound";
    }
    else if (LRUMap[key]->information["revalidate"] == "true") {
      return "revalidate";
    }
    else {
      Response tVal = (*LRUMap[key]).value;
      if (tVal.getExpireTime() < curTime) {
        std::string tempKey = (*LRUMap[key]).key;
        LRUlist.erase(LRUMap[key]);  //remove old place
        LRUMap.erase(tempKey);
        return "expires";
      }
      else {
        LRUlist.emplace_back(key, tVal);  // insert to end
        LRUlist.erase(LRUMap[key]);       //remove old place
        std::list<LRUNode>::iterator it = LRUlist.end();
        it--;
        LRUMap[key] = it;  // update Map
        return ;
      }
    }
  }

  void put(const std::string & key, Response & value) {
         if (LRUMap.count(key) != 0)  //exist
        {
          LRUlist.erase(LRUMap[key]);        // remove old place, add end
          LRUlist.emplace_back(key, value);  // insert to end
          std::list<LRUNode>::iterator it = LRUlist.end();
          it--;
          LRUMap[key] = it;  // update Map
          it->information["expireTime"] = to_string(value.getExpireTime());
          if (value.getRevalidate()) {
            it->information["revalidate"] = "true";
          }
          else {
            it->information["revalidate"] = "false";
          }
        }
      else {                               // not exist
        if (LRUlist.size() >= capacity) {  // not enough space
          LRUMap.erase(LRUlist.front().key);
          LRUlist.pop_front();
        }
        LRUlist.emplace_back(key, value);  // insert to end
        std::list<LRUNode>::iterator it = LRUlist.end();
        it--;
        LRUMap[key] = it;  // update Map
        it->information["expireTime"] = to_string(value.getExpireTime());
        if (value.getRevalidate()) {
          it->information["revalidate"] = "true";
        }
        else {
          it->information["revalidate"] = "false";
        }
      }
  }
};

#include "Response.hpp"
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

class Cache {
private:  
  class LRUNode{
  public:
    std::string key;
    Response value;
    LRUNode(const std::string& rkey, const Response& rval): key(rkey), value(rval){}
  };
  
  std::map<std::string,std::list<LRUNode>::iterator> LRUMap;
  std::list<LRUNode>LRUlist;
  
public:
  size_t capacity;
  Cache(size_t rcapacity): capacity(rcapacity) {}
    
  std::string get(const std::string& key) {
    if(LRUMap.count(key) == 0){
      return "";
    }
    else{
      Response tVal = (*LRUMap[key]).value; 
      LRUlist.emplace_back(key, tVal); // insert to end
      LRUlist.erase(LRUMap[key]); //remove old place
      std::list<LRUNode>::iterator it = LRUlist.end();
      it --;
      LRUMap[key] = it;// update Map
      //std::cout << "##############" <<tVal.getResponseInfo() << std::endl;
      return tVal.getResponseInfo();
    }
  }
    
  void put(const std::string& key, const Response& value) {
    if(LRUMap.count(key) != 0) //exist
      {
        LRUlist.erase(LRUMap[key]); // remove old place, add end
        LRUlist.emplace_back(key, value); // insert to end
        std::list<LRUNode>::iterator it = LRUlist.end();
        it --;
        LRUMap[key] = it;// update Map
      }
    else{ // not exist
      if(LRUlist.size() >= capacity){ // not enough space
        LRUMap.erase(LRUlist.front().key);
        LRUlist.pop_front();
      }
      LRUlist.emplace_back(key, value); // insert to end
      std::list<LRUNode>::iterator it = LRUlist.end();
      it --;
      LRUMap[key] = it;// update Map
    }
  } 
};

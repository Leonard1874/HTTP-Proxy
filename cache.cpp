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
    std::string info;
    int expTime;
    LRUNode(const std::string& rkey, const std::string& rinfo, const int rexpTime): key(rkey), info(rinfo), expTime(rexpTime){}
  };
  
  std::map<std::string,std::list<LRUNode>::iterator> LRUMap;
  std::list<LRUNode>LRUlist;
  
public:
  size_t capacity;
  Cache(size_t rcapacity): capacity(rcapacity) {}
    
  std::string get(std::string key) {
    if(LRUMap.count(key) == 0){
      return NULL;
    }
    else{
      std::string tinfo = (*LRUMap[key]).info;
      int texpTime = (*LRUMap[key]).expTime; 
      LRUlist.erase(LRUMap[key]); //remove old place
      LRUlist.emplace_back(key, tinfo, texpTime); // insert to end
      LRUMap[key] = LRUlist.end(); // update Map
      return tinfo;
    }
  }
    
  void put(std::string key, std::string info, int expTime) {
    if(LRUMap.count(key) != 0) //exist
      {
        LRUlist.erase(LRUMap[key]); // remove old place, add end
        LRUlist.emplace_back(key, info, expTime); // insert to end
        LRUMap[key] = LRUlist.end(); // update Map
      }
    else{ // not exist
      if(LRUlist.size() >= capacity){ // not enough space
        LRUMap.erase(LRUlist.front().key);
        LRUlist.pop_front();
      }
      LRUlist.emplace_back(key, info, expTime); // insert to end
      LRUMap[key] = LRUlist.end(); // update Map
    }
  }
  
};

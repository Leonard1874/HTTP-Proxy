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

class ResponseParser{
private:
  Response toParse;
public:
  ResponseParser(const Response& rtoParse): toParse(rtoParse){}
  std::string parseExpire(){
    
  }
private:
  std::string parseExpireControl(){
    
  }
  std::string parseExpireExp(){
    
  }
  std::string parseExpireLastModify(){
    
  }
};

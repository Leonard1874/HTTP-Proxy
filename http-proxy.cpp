#include "proxy.hpp"

int main(int argc, char* argv[]){
  if(argc < 3){
    std::cerr << "wrong number of arguments" << std::endl;
  }
  const char *hostname = argv[1];
  const char *port     = argv[2];
  Proxy myProxy(hostname,port);
  
  while(true){
    std::string requestInfo;
    if(myProxy.lisenClient(requestInfo)){
      return EXIT_FAILURE;
    }
    Request reqObj(requestInfo);
    //std::string cached = "";
    if(reqObj.getType() == "GET"){
      myProxy.handleGet(reqObj);
    }
    else if(reqObj.getType() == "POST"){
      myProxy.handlePost(reqObj);
    }
    else{
      //continue;
      myProxy.handleConnect(reqObj);
    }
    myProxy.closeSockfds();
  }  
  return EXIT_SUCCESS;
}

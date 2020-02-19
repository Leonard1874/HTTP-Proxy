#include "proxy.hpp"

int getReq(Proxy& myProxy, std::string& info, std::vector<std::string>& parsedInfo){
  std::string originHostName;
  if(!myProxy.response_parser(originHostName,parsedInfo[1])){
    std::cerr << "parse error" << std::endl;
  }
  
  if(myProxy.connectToSocket(originHostName.c_str(),"80") < 0){
    std::cerr << "connect to origin host error" << std::endl;
    return EXIT_FAILURE;
  }

  if(!myProxy.Send(myProxy.getSockfdO(),info.c_str())){
    std::cerr << "send to origin error" << std::endl;
    return EXIT_FAILURE;
  }
  
  std::string getInfo;
  if(!myProxy.recieve(myProxy.getSockfdO(),getInfo)){
    std::cerr << "recv from origin error" << std::endl;
    return EXIT_FAILURE;
  }
  
  std::cout << getInfo << std::endl;
  
  if(!myProxy.Send(myProxy.getSockfdB(),getInfo)){
    std::cerr << "send to browser" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int main(int argc, char* argv[]){
  if(argc < 3){
    std::cerr << "wrong number of arguments" << std::endl;
  }
  const char *hostname = argv[1];
  const char *port     = argv[2];
  Proxy myProxy;
  if(myProxy.setupSocket(hostname,port) < 0){
    std::cerr << "setup socket to browser error" << std::endl;
    return EXIT_FAILURE;
  }
  if(myProxy.acceptSocket() < 0){
    std::cerr << "accept from browser" << std::endl;
    return EXIT_FAILURE;
  }
  std::string info;
  if(!myProxy.recieve(myProxy.getSockfdB(),info)){
    std::cerr << "accept from browser" << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << info << std::endl;
  std::vector<std::string> parsedInfo = myProxy.parseInputLines(info);

  //getReq(myProxy,info,parsedInfo);
  std::string originHostName;
  if(!myProxy.response_parser(originHostName,parsedInfo[1])){
    std::cerr << "parse error" << std::endl;
  }
  
  if(myProxy.connectToSocket(originHostName.c_str(),"80") < 0){
    std::cerr << "connect to origin host error" << std::endl;
    return EXIT_FAILURE;
  }

  if(!myProxy.Send(myProxy.getSockfdO(),info.c_str())){
    std::cerr << "send to origin error" << std::endl;
    return EXIT_FAILURE;
  }
  
  std::string getInfo;
  if(!myProxy.recieve(myProxy.getSockfdO(),getInfo)){
    std::cerr << "recv from origin error" << std::endl;
    return EXIT_FAILURE;
  }
  
  std::cout << getInfo << std::endl;
  
  if(!myProxy.Send(myProxy.getSockfdB(),getInfo)){
    std::cerr << "send to browser" << std::endl;
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}

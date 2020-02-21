#include "proxy.hpp"

int Proxy::listenBrowser(const char* hostname, const char* port){
  if(setupSocket(hostname,port) < 0){
    std::cerr << "setup socket to browser error" << std::endl;
    return EXIT_FAILURE;
  }
  if(acceptSocket() < 0){
    std::cerr << "accept from browser" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int Proxy::getRequest(std::string& request){
  if(!recieve(getSockfdB(),request)){
    std::cerr << "accept from browser" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int Proxy::getServerSendBrowser(const std::string& originHostName, const std::string& requestInfo, std::string& getInfo){
  if(connectToSocket(originHostName.c_str(),"80") < 0){
    std::cerr << "connect to origin host error" << std::endl;
    return EXIT_FAILURE;
  }

  if(!Send(getSockfdO(),requestInfo.c_str())){
    std::cerr << "send to origin error" << std::endl;
    return EXIT_FAILURE;
  }
  
  if(!recieve_origin(getSockfdO(),getInfo)){
    std::cerr << "recv from origin error" << std::endl;
    return EXIT_FAILURE;
  }
  
  if(!Send(getSockfdB(),getInfo)){
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
  
  while(true){
    if(myProxy.listenBrowser(hostname, port)){
      std::cerr <<"listen error!"<< std::endl;
      return EXIT_FAILURE;
    }

    std::string requestInfo;
    while(requestInfo.empty()){
      if(myProxy.getRequest(requestInfo)){
        std::cerr << "get request error" << std::endl;
        return EXIT_FAILURE;
      }
    }
    std::cout << "req: " <<requestInfo << std::endl;
    std::vector<std::string> parsedInfo = myProxy.parseInputLines(requestInfo);

    std::string originHostName;
    if(!myProxy.response_parser(originHostName,parsedInfo[1])){
      std::cerr << "parse error" << std::endl;
    }

    std::cout << originHostName << std::endl;
    
    /*check request*/
    /*check cache*/
  
    std::string getInfo;
    if(myProxy.getServerSendBrowser(originHostName, requestInfo, getInfo)){
      std::cerr <<"get error!"<< std::endl;
      return EXIT_FAILURE;
    }
    std::cout << getInfo << std::endl;
    /*update cache*/
    myProxy.closeSockfds();
   }
  
  return EXIT_SUCCESS;
}

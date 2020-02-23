#include "proxy.hpp"
#include "Request.cpp"
#include "Response.cpp"
#include "logger.cpp"
#include "cache.cpp"

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

int Proxy::sendCacheBrowser(const std::string& cachedInfo){
  if(!Send(getSockfdB(),cachedInfo)){
    std::cerr << "send to browser" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int Proxy::getServerSendBrowser(const std::string& hostName, const std::string& requestInfo, std::string& getInfo){
  if(connectToSocket(hostName.c_str(),"80") < 0){ // port to be check
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
  Cache myCache(20);
  int ID = 0;
  logger my_logger;
  
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

    Request reqObj(requestInfo);
    if(reqObj.getType()!="CONNECT"){
      /*check request*/
      /*check cache*/
      std::cout << reqObj.getKey() << std::endl;
      std::string cached = myCache.get(reqObj.getKey());
      if(!cached.empty()){
        //      my_logger.printCache("in cache");
        std::cout << "*******************Cache: found " << std::endl;
        if(myProxy.sendCacheBrowser(cached)){
          std::cerr <<"get error!"<< std::endl;
          return EXIT_FAILURE;
        }
      }
      else{
        /*logger*/
        //  my_logger.printCache("not in cache");
        std::cout << "**************Cache: not found" << std::endl;
        my_logger.getrequest(ID, reqObj);
        my_logger.printlogline();
    
        std::string getInfo;
        if(myProxy.getServerSendBrowser(reqObj.getHostname(), reqObj.getRequestInfo(), getInfo)){
          std::cerr <<"get error!"<< std::endl;
          return EXIT_FAILURE;
        }
        std::cout << getInfo << std::endl;
        Response resObj(getInfo,reqObj.getType());
        /*update cache*/
        std::string value = resObj.getValue();
        if(!value.empty()){
          myCache.put(reqObj.getKey(),value,10);
        }
      }
    }
    myProxy.closeSockfds();
  }
  
  return EXIT_SUCCESS;
}

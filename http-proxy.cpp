#include "proxy.hpp"
int threadCommunicate(int sockfd_to_browser, Proxy& myProxy, Cache & myCache, Timer & myTimer, logger & myLogger, int ID ){
  std::string requestInfo;
  if(myProxy.getRequest(requestInfo,sockfd_to_browser)){
    close(sockfd_to_browser);
    return EXIT_FAILURE;
  }
  Request reqObj(requestInfo);

  std::string hostConnect = reqObj.getHostname();
  std::string portConnect = reqObj.getPort();
  if(portConnect.empty()){
    portConnect = "80";
  }

  int sockfd_to_origin = myProxy.connectToSocket(hostConnect.c_str(),portConnect.c_str());
  if(sockfd_to_origin < 0){
    std::cerr << "connect to origin host error" << std::endl;
    close(sockfd_to_browser);
    close(sockfd_to_origin);
    return EXIT_FAILURE;
  }
  
  if (reqObj.getType() == "GET") {
    myProxy.handleGet(reqObj, myCache, myTimer, myLogger, ID, sockfd_to_browser, sockfd_to_origin);
  }
  else if (reqObj.getType() == "POST") {
    myProxy.handlePost(reqObj, myTimer, myLogger, ID, sockfd_to_browser, sockfd_to_origin);
  }
  else {
    myProxy.handleConnect(reqObj, myLogger, ID, sockfd_to_browser, sockfd_to_origin);
  }
  close(sockfd_to_browser);
  close(sockfd_to_origin);
  return EXIT_SUCCESS;
}

int main(int argc, char * argv[]) {
  if (argc < 3) {
    std::cerr << "wrong number of arguments" << std::endl;
  }
  const char * hostname = argv[1];
  const char * port = argv[2];
  Proxy myProxy(hostname, port);
  Timer myTimer;
  logger myLogger;
  Cache myCache(50);
  int ID = 0;

  int sockfd_own = myProxy.setupSocket(hostname,port);
  
  while (true) {
    std::string requestInfo;
    int sockfd_to_browser = myProxy.acceptSocket(sockfd_own);
    ID++;
    std::thread(threadCommunicate, sockfd_to_browser, std::ref(myProxy), std::ref(myCache), std::ref(myTimer), std::ref(myLogger), ID).detach();
  }
  close(sockfd_own);
  return EXIT_SUCCESS;
}

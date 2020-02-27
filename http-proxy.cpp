#include "proxy.hpp"

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
  while (true) {
    ID++;
    std::string requestInfo;
    if (myProxy.lisenClient(requestInfo)) {
      return EXIT_FAILURE;
    }
    Request reqObj(requestInfo);
    //std::string cached = "";
    if (reqObj.getType() == "GET") {
      myProxy.handleGet(reqObj, myCache, myTimer, myLogger, ID);
    }
    else if (reqObj.getType() == "POST") {
      myProxy.handlePost(reqObj, myTimer, myLogger, ID);
    }
    else {
      //continue;
      myProxy.handleConnect(reqObj, myLogger, ID);
    }
    myProxy.closeSockfds();
  }
  return EXIT_SUCCESS;
}

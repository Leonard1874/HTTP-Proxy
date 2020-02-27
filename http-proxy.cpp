#include "proxy.hpp"

/*std::thread myThreads[4];

    for (int i=0; i<4; i++){
        //myThreads[i].start(exec, i); //?? create, start, run
        //new (&myThreads[i]) std::thread(exec, i); //I tried it and it seems to work, but it looks like a bad design or an anti-pattern.
    }
    for (int i=0; i<4; i++){
        myThreads[i].join();
    }
*/

int main(int argc, char * argv[]) {
  if (argc < 3) {
    std::cerr << "wrong number of arguments" << std::endl;
  }
  const char * hostname = argv[1];
  const char * port = argv[2];
  WebToolKit myKit;
  Proxy myProxy(hostname, port);
  Timer myTimer;
  logger myLogger;
  Cache myCache(50);
  int ID = 0;
  
  //std::thread myThreads[10];
  
  while (true) {
    ID++;
    std::string requestInfo;
    if (myProxy.lisenClient(requestInfo, myKit)) {
      return EXIT_FAILURE;
    }
    Request reqObj(requestInfo);
    usleep(1000);
    //std::string cached = "";
    if (reqObj.getType() == "GET") {
      std::thread(&Proxy::handleGet, reqObj, std::ref(myCache), std::ref(myTimer), std::ref(myLogger), ID, myKit).detach();
      //myProxy.handleGet(reqObj, myCache, myTimer, myLogger, ID);
    }
    else if (reqObj.getType() == "POST") {
      std::thread(&Proxy::handlePost, reqObj, std::ref(myTimer), std::ref(myLogger), ID, myKit).detach();
      //myProxy.handlePost(reqObj, myTimer, myLogger, ID);
    }
    else {
      std::thread(&Proxy::handleConnect, reqObj, std::ref(myLogger), ID, myKit).detach();
      //myProxy.handleConnect(reqObj, myLogger, ID);
    }
    myKit.closeSockfds();
  }
  return EXIT_SUCCESS;
}

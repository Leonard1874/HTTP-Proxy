#include "Webtoolkit.hpp"
#include "cache.hpp"
#include "logger.hpp"

class Proxy {
 private:
  const char * hostname;
  const char * port;
  WebToolKit myKit;
  Timer myTimer;
  logger myLogger;
  Cache myCache;
  int ID;

 public:
  Proxy(const char * rhostname, const char * rport) :
      hostname(rhostname),
      port(rport),
      myCache(20) {}

  int lisenClient(std::string & requestInfo) {
    if (myKit.listenBrowser(hostname, port)) {
      std::cerr << "listen error!" << std::endl;
      return EXIT_FAILURE;
    }

    while (requestInfo.empty()) {
      if (myKit.getRequest(requestInfo)) {
        std::cerr << "get request error" << std::endl;
        return EXIT_FAILURE;
      }
    }
    std::cout << "req: " << requestInfo << std::endl;
    return EXIT_SUCCESS;
  }

  int handleGet(Request & reqObj) {
    std::string cached = myCache.get(reqObj.getKey(), myTimer.getCurrentSec());
    if (cached != "notfound" && cached != "revalidate" && cached[0] != 'I' &&
        cached != "expires") {
      std::cout << "****************Cache: found*******************" << std::endl;
      //std::cout << cached << std::endl;
      if (myKit.sendCacheBrowser(cached)) {
        std::cerr << "get error!" << std::endl;
        return EXIT_FAILURE;
      }
      myLogger.printCache("in cache", ID);
      /*in cache situations*/
      return EXIT_SUCCESS;
    }
    else if (cached == "notfound" || cached == "expires") {
      /*update cache*/
      std::string getInfo;
      if (myKit.getServerSendBrowser(
              reqObj.getHostname(), reqObj.getRequestInfo(), getInfo)) {
        return EXIT_FAILURE;
      }
      std::cout << getInfo << std::endl;
      Response resObj(getInfo, reqObj.getType(), myTimer.getCurrentSec());

      if (resObj.canCache()) {
        myCache.put(reqObj.getKey(), resObj);
      }
      myLogger.printCache("not in cache", ID);
      myLogger.getrequest(ID, reqObj);
      myLogger.printlogline();
      return EXIT_SUCCESS;
    }
    //Need revalidate
    else {
      //revalidate cache
      reqObj.setRequest(cached);
      std::string getInfo;
      if (myKit.getServerSendBrowser(
              reqObj.getHostname(), reqObj.getRequestInfo(), getInfo)) {
        return EXIT_FAILURE;
      }
      std::cout << getInfo << std::endl;
      if (getInfo.find("304 Not Modified") != std::string::npos) {
        //Get from cache directly
        string temp = myCache.noModifyGet(reqObj.getKey());
        if (myKit.sendCacheBrowser(temp)) {
          std::cerr << "get error!" << std::endl;
          return EXIT_FAILURE;
        }
        //myLogger.printCache("in cache", ID);
        /*in cache situations*/
        return EXIT_SUCCESS;
      }
      //Request a new response from the origin server
      else {
        std::string getInfo;
        if (myKit.getServerSendBrowser(
                reqObj.getHostname(), reqObj.getRequestInfo(), getInfo)) {
          return EXIT_FAILURE;
        }
        std::cout << getInfo << std::endl;
        Response resObj(getInfo, reqObj.getType(), myTimer.getCurrentSec());
        if (resObj.canCache()) {
          myCache.put(reqObj.getKey(), resObj);
        }
        myLogger.printCache("not in cache", ID);
        myLogger.getrequest(ID, reqObj);
        myLogger.printlogline();
        return EXIT_SUCCESS;
      }
    }
  }

  int handlePost(Request & reqObj) {
    std::string getInfo;

    if (myKit.getServerSendBrowser(
            reqObj.getHostname(), reqObj.getRequestInfo(), getInfo)) {
      return EXIT_FAILURE;
    }
    std::cout << getInfo << std::endl;
    Response resObj(getInfo, reqObj.getType(), myTimer.getCurrentSec());
    if (resObj.canCache()) {
      myCache.put(reqObj.getKey(), resObj);
    }
    return EXIT_SUCCESS;
  }

  int handleConnect(Request & reqObj) {
    myLogger.getrequest(ID, reqObj);
    myLogger.printlogline();
    if (myKit.selectBrowserServer(reqObj.getHostname(), reqObj.getPort())) {
      return EXIT_FAILURE;
    }
    myLogger.printCache("Tunnel closed", ID);
    return EXIT_SUCCESS;
  }

  void closeSockfds() { myKit.closeSockfds(); }
};

#include "Webtoolkit.hpp"
#include "cache.hpp"
#include "logger.hpp"

class Proxy {
private:
  const char * hostname;
  const char * port;
  WebToolKit myKit;

public:
  Proxy(const char * rhostname, const char * rport) : 
    hostname(rhostname),
    port(rport){}

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

  int handleGet(Request & reqObj, Cache& myCache, Timer& myTimer, logger& myLogger, int& ID) {
    
    myLogger.getrequest_time(ID, reqObj); 
    myLogger.print_recieve_requestline();
    
    std::string cached = myCache.get(reqObj.getKey(), myTimer.getCurrentSec());
    std::cout << "****************Cached information*******************" << std::endl;
    cout<<"******------********"<<cached<<endl;
    if (cached != "notfound" && cached.find("revalidate:")==std::string::npos && cached[0] != 'I' && cached.find("expires:")==std::string::npos) {
      std::cout << "****************Cache: found*******************" << std::endl;
      //std::cout << cached << std::endl;
      if (myKit.sendCacheBrowser(cached)) {
        std::cerr << "send cached error!" << std::endl;
        return EXIT_FAILURE;
      }
      myLogger.printCache("in cache, valid", ID);
      //respond
    }
    else if (cached == "notfound" || cached.find("expires:")!=std::string::npos) {
      std::cout << "****************Cache: not found or expire*******************" << std::endl;
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
      if(cached == "notfound"){
        myLogger.printCache("not in cache", ID);
      }
      else{
        std::string expireString = "in cache, but expired at ";
        cached = cached.substr(cached.find_first_of(':')+1);
        long expireInt = std::stol(cached);
        expireString += myTimer.getlocalTimeStr(expireInt);
        myLogger.printCache(expireString, ID);
      }
      myLogger.getrequest_requesting(ID, reqObj);
      myLogger.print_send_requestline();
      //recved
      //responding
    }
    //Need revalidate
    else {
      cached = cached.substr(cached.find_first_of(':')+1);
      reqObj.setRequest(cached);
      std::string getInfo;
      cout << "*****************************Revalidate*****************\n";
      if (myKit.getServerSendBrowser(
                                     reqObj.getHostname(), reqObj.getRequestInfo(), getInfo)) {
        return EXIT_FAILURE;
      }
      std::cout << getInfo << std::endl;
      myLogger.printCache("in cache, need revalidate", ID);
      if (getInfo.find("304 Not Modified") != std::string::npos) {
        //Get from cache directly
        string temp = myCache.noModifyGet(reqObj.getKey());
        if (myKit.sendCacheBrowser(temp)) {
          std::cerr << "get error!" << std::endl;
          return EXIT_FAILURE;
        }
        myLogger.printCache("NOTE revalidate success", ID);
        //responding
      }
      else{
        //Request a new response from the origin server
        std::string getInfo;
        if (myKit.getServerSendBrowser(
                                       reqObj.getHostname(), reqObj.getRequestInfo(), getInfo)) {
          return EXIT_FAILURE;
        }
        //std::cout << getInfo << std::endl;
        Response resObj(getInfo, reqObj.getType(), myTimer.getCurrentSec());
        if (resObj.canCache()) {
          myCache.put(reqObj.getKey(), resObj);
        }
        myLogger.printCache("NOTE revalidate fail", ID);
        myLogger.getrequest_requesting(ID, reqObj);
        myLogger.print_send_requestline();
        //recv
        //respond
      }
    }
    return EXIT_SUCCESS;
  }

  int handlePost(Request & reqObj, Cache& myCache, Timer& myTimer, logger& myLogger, int& ID) {
    myLogger.getrequest_time(ID, reqObj); 
    myLogger.print_recieve_requestline();
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
    myLogger.getrequest_requesting(ID, reqObj);
    myLogger.print_send_requestline();
    //recv
    //respond
    return EXIT_SUCCESS;
  }

  int handleConnect(Request & reqObj, logger& myLogger, int& ID) {
    myLogger.getrequest_time(ID, reqObj); 
    myLogger.print_recieve_requestline();
    myLogger.getrequest_requesting(ID, reqObj);
    myLogger.print_send_requestline();
    if (myKit.selectBrowserServer(reqObj.getHostname(), reqObj.getPort())) {
      return EXIT_FAILURE;
    }
    myLogger.printCache("Tunnel closed", ID);
    return EXIT_SUCCESS;
  }

  void closeSockfds() { myKit.closeSockfds(); }
};

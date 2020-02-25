#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#define MAXDATASIZE 100
class WebToolKit{
private:
  int sockfd_own;
  int sockfd_to_browser;
  int sockfd_to_origin;
  std::vector<std::string> stringFromBrowser;
  
public:
  WebToolKit() = default;
  void closeSockfds(){
    close(sockfd_own);
    close(sockfd_to_browser);
    close(sockfd_to_origin);
  }
  
  int getSockfdOwn(){
    return sockfd_own;
  }

  int getSockfdB(){
    return sockfd_to_browser;
  }

  int getSockfdO(){
    return sockfd_to_origin;
  }
  
  int listenBrowser(const char* hostname, const char* port){
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
  
  int getRequest(std::string& request){
    if(!recieve(getSockfdB(),request)){
      std::cerr << "accept from browser" << std::endl;
      return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
  }

  int selectBrowserServer(const std::string& hostName, const std::string& port){
    if(connectToSocket(hostName.c_str(),port.c_str()) < 0){ // port to be check
      std::cerr << "connect to origin host error" << std::endl;
      return EXIT_FAILURE;
    }
    
    if(!Send(getSockfdB(),"HTTP/1.1 200 OK\r\n\r\n")){
      std::cerr << "send 200 OK to client error" << std::endl;
      return EXIT_FAILURE;
    }

    int fdmax = ((getSockfdB() > getSockfdO()) ? getSockfdB() : getSockfdO());
    //bool isOver = false;
    fd_set sockfds;
    FD_ZERO(&sockfds);
    fd_set sockfds_temp;
    FD_ZERO(&sockfds_temp);
    FD_SET(getSockfdB(), &sockfds);
    FD_SET(getSockfdO(), &sockfds);
    while(true){
      sockfds_temp = sockfds; // copy it
      if (select(fdmax+1, &sockfds_temp, NULL, NULL, NULL) == -1) {
        std::perror("select");
        return EXIT_FAILURE;
      }
      for(int i = 0; i <= fdmax; i++) {
        if (FD_ISSET(i, &sockfds_temp)) {
          if(i == getSockfdB()){
            std::cout << "B:" <<getSockfdB() << std::endl;
            char temp[65536];
            int numbytes = 0;
            if((numbytes = recv(getSockfdB(), temp, 65536, 0)) == -1) {
              std::perror("recv client");
              return EXIT_FAILURE;
            }
            std::cout << numbytes << std::endl;
            if(numbytes == 0){
              std::cout << "BOver!" << std::endl;
              return EXIT_SUCCESS;
            }

            if (send(getSockfdO(), temp, numbytes, 0) == -1){ 
              std::perror("connect send to host");
              return EXIT_FAILURE;
            }
            std::cout << "send!" << std::endl;
            continue;
          }
          else if(i == getSockfdO()){
            std::cout << "O: " <<getSockfdO() << std::endl;
            char temp[65536];
            int numbytes = 0;
            if((numbytes = recv(getSockfdO(), temp, 65536, 0)) == -1) {
              std::perror("recv server");
              return EXIT_FAILURE;
            }
            std::cout << numbytes << std::endl;
            if(numbytes == 0){
              std::cout << "OOver!" << std::endl;
              return EXIT_SUCCESS;
            }
            
            if (send(getSockfdB(), temp, numbytes, 0) == -1){ 
              std::perror("connect send to client");
              return EXIT_FAILURE;
            }
            std::cout << "Send!" << std::endl;
            continue;
            //temp.clear();
          }
          else{
            continue;
            std::cout << "Wrong fd!" << std::endl;
          }
        }
      }
    }
    return EXIT_SUCCESS;
  }
    int getServerSendBrowser(const std::string& hostName, const std::string& requestInfo, std::string& getInfo){
      if(connectToSocket(hostName.c_str(),"80") < 0){ // port to be check
        std::cerr << "connect to origin host error" << std::endl;
        return EXIT_FAILURE;
      }

      if(!Send(getSockfdO(),requestInfo.c_str())){
        std::cerr << "send to origin error" << std::endl;
        return EXIT_FAILURE;
      }

      int chunck = recieve_origin(getSockfdO(),getInfo);
      if(chunck < 0){
        std::cerr << "recv from origin error" << std::endl;
        return EXIT_FAILURE;
      }
   
      if(chunck == 0){
        if(!recieve_unchunck(getSockfdO(),getInfo)){
          std::cerr << "recv from unchuncked origin error" << std::endl;
          return EXIT_FAILURE;
        }
        std::cout << getInfo << std::endl;
        if(!Send(getSockfdB(),getInfo)){
          std::cerr << "send to browser" << std::endl;
          return EXIT_FAILURE;
        }
      }
  
      return EXIT_SUCCESS;
    }
  
    int sendCacheBrowser(const std::string& cachedInfo){
      if(!Send(getSockfdB(),cachedInfo.c_str())){
        std::cerr << "send to origin error" << std::endl;
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    }
  
  private:
    bool Send(int sendFd, std::string toSend){
      if (send(sendFd, toSend.c_str(), strlen(toSend.c_str()), 0) == -1){ 
        std::perror("send port number");
        return false;
      }
      return true;
    }

    int getLength(std::string& recv){
      size_t pos = recv.find("Content-Length: ");
      if(pos == std::string::npos){
        std::cerr << "can't find length!" << std::endl;
        return -1;
      }
      else{
        std::string lengthStr;
        while(recv[pos]!='\n'){
          if(isdigit(recv[pos])){
            lengthStr += recv[pos];
          }
          pos ++;
        }
        std::cout << lengthStr << std::endl;
        return std::stoi(lengthStr);
      }
    }

    int getHeadLength(std::string& recv){
      size_t pos = recv.find("\r\n\r\n");
      if(pos == std::string::npos){
        std::cerr << "can't find empty line!" << std::endl;
        return -1;
      }
      return pos;
    }

    bool recieve_unchunck(int sockfd, std::string& toGet){
      int allLen = 0;
      int hasGot = toGet.size();
      int numbytes = 0;
      if(toGet.find("HTTP/1.1 200 OK") == std::string::npos){
        allLen = 0;
      }
      else{
        int len = getLength(toGet);
        if(len < 0){
          return false;
        }
        int headlen = getHeadLength(toGet);
        if(headlen < 0){
          return false;
        }
        allLen = headlen + len;
      }
      while(hasGot < allLen){
        char temp[65536];
        if((numbytes = recv(sockfd, temp, 65535, 0)) == -1) {
          std::perror("recv");
          return false;
        }
        hasGot += numbytes;
        std::string tempStr(temp);
        if(hasGot >= allLen){
          toGet += tempStr;	
          break;
        }
        else{
          toGet += tempStr;
        }
      }
      return true;
    }

    
  bool recieve_chunck(int sockfd, std::string& toGet){
    int numbytes = 0;
    char temp[65536];
    while(true){
      memset(temp,'\0',sizeof(temp));
      std::string endMark = "0\r\n\r\n";
      if((numbytes = recv(sockfd, temp, 65535, 0)) == -1) {
        std::perror("recv");
        return false;
      }
      std::string tempStr(temp);
      if(tempStr.find(endMark)!=std::string::npos){
        toGet += tempStr;
        //std::cout << "end! " <<toGet << std::endl;
        break;
      }
      else{
        //std::cout << toGet << std::endl;
        toGet += tempStr;
      }
    }
    return true;
  }
    
    bool isChuncked(const std::string& header){
      bool chunck = false;
      if(header.find("Transfer-Encoding:") != std::string::npos){
        if(header.find("Chuncked") != std::string::npos){
          chunck = true;
        }
      }
      return chunck;
    }
  
    int recieve_origin(int sockfd, std::string& toGet){
      int numbytes = 0;
      int hasGot = 0;
      char temp[65536];
      if((numbytes = recv(sockfd, temp, 65535, 0)) == -1) {
        std::perror("recv");
        return -1;
      }
      hasGot += numbytes;
      std::string tempStr(temp);
      toGet += tempStr;
      if(tempStr.find("HTTP/1.1 200 OK") == std::string::npos){
        return 0;
      }
      else{
        if(isChuncked(tempStr)){
          return 1;
        }
      }
      return 0;
    }
  
    bool recieve(int sockfd, std::string& toGet){
      int numbytes = 0;
      char temp[65536];
      while(true){
        memset(temp,'\0',sizeof(temp));
        std::string endMark = "\r\n\r\n";
        if((numbytes = recv(sockfd, temp, 65535, 0)) == -1) {
          std::perror("recv");
          return false;
        }
        std::string tempStr(temp);
        if(tempStr.find(endMark)!=std::string::npos){
          toGet += tempStr;
          //std::cout << "end! " <<toGet << std::endl;
          break;
        }
        else{
          //std::cout << toGet << std::endl;
          toGet += tempStr;
        }
      }
      return true;
    }

    int acceptSocket(){
      struct sockaddr_storage connector_addr;
      socklen_t sin_size;
      sin_size = sizeof connector_addr;
      sockfd_to_browser = accept(sockfd_own, (struct sockaddr *)&connector_addr, &sin_size);
      if (sockfd_to_browser == -1) {
        std::perror("accept");
        return -1;
      }
      return 0;
    }
  
    int connectToSocket(const char* hostname, const char* port){
      struct addrinfo host_info;
      struct addrinfo *host_info_list;

      memset(&host_info, 0, sizeof(host_info));
      host_info.ai_family   = AF_UNSPEC;
      host_info.ai_socktype = SOCK_STREAM;
      host_info.ai_flags = AI_PASSIVE;

      // std::cout << hostname << ":" << port << std::endl;
  
      int status = getaddrinfo(hostname, port, &host_info, &host_info_list);
      if (status != 0) {
        std::cerr << gai_strerror(status) << std::endl;
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "(" << hostname << "," << port << ")" << std::endl;
        return -1;
      } 
  
      sockfd_to_origin = socket(host_info_list->ai_family, 
                                host_info_list->ai_socktype, 
                                host_info_list->ai_protocol);
      if (sockfd_to_origin == -1) {
        std::perror("build socket");
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << hostname << ":" << port << ")" << std::endl;
        return -1;
      }
  
      status = connect(sockfd_to_origin, host_info_list->ai_addr, host_info_list->ai_addrlen);
      if (status == -1) {
        std::perror("connect client");
        std::cerr << "Error: cannot connect to socket" << std::endl;
        std::cerr << "  (" << hostname << ":" << port << ")" << std::endl;
        return -1;
      }
      freeaddrinfo(host_info_list);
      return 0;
    }

    int setupSocket(const char* hostname, const char* port){
      int status;
      struct addrinfo host_info;
      struct addrinfo *host_info_list;

      memset(&host_info, 0, sizeof(host_info));

      host_info.ai_family   = AF_UNSPEC;
      host_info.ai_socktype = SOCK_STREAM;
      host_info.ai_flags    = AI_PASSIVE;

      status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  
      if (status != 0) {
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
      }
  
      sockfd_own = socket(host_info_list->ai_family, 
                          host_info_list->ai_socktype, 
                          host_info_list->ai_protocol);
      if (sockfd_own == -1) {
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
      }

      int yes = 1;
      status = setsockopt(sockfd_own, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
      status = bind(sockfd_own, host_info_list->ai_addr, host_info_list->ai_addrlen);
      if (status == -1) {
        std::cerr << "Error: cannot bind socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
      }

      status = listen(sockfd_own, 100);
      if (status == -1) {
        std::cerr << "Error: cannot listen on socket" << std::endl; 
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return -1;
      }
      freeaddrinfo(host_info_list);
      return 0;
    }
  };
  /*
    int selectPort(std::vector<int>& sockets, int ownNum, int playerNum){
    int fdmax = *(std::max_element(sockets.begin(),sockets.end()));
    fd_set sockfds;
    FD_ZERO(&sockfds);
    for(size_t i = 0; i < sockets.size(); i++){
    FD_SET(sockets[i], &sockfds);
    }
    while(true){
    fd_set sockfds_temp = sockfds; // copy it
    if (select(fdmax+1, &sockfds_temp, NULL, NULL, NULL) == -1) {
    std::perror("select");
    return -1;
    }
    // run through the existing connections looking for data to read
    for(int i = 0; i <= fdmax; i++) {
    if (FD_ISSET(i, &sockfds_temp)) {
    if (i == sockets[0]) { //ringmaster
    struct potato p;
    int gameStatus = getMsg(p,i);
    if(gameStatus < 0){
    std::cerr << "get potato from host error" << std::endl;
    return -1;
    }
    else if(gameStatus > 0){
    sendEnd(sockets,1);
    sendEnd(sockets,2);
    return 0;
    }
    int status = handlePotato(p,sockets,ownNum,playerNum);
    if(status < 0){
    std::cerr << "handle potato from host error" << std::endl;
    return -1;
    }
    }
    else{ //clients
    struct potato p;
    int gameStatusClient = getMsg(p,i);
    if(gameStatusClient < 0){
    std::cerr << "get potato from client error" << std::endl;
    return -1;
    }
    else if(gameStatusClient > 0){
    sendEnd(sockets,1);
    sendEnd(sockets,2);
    return 0;
    }
    int status = handlePotato(p,sockets,ownNum,playerNum);
    if(status < 0){
    std::cerr << "handle potato from client error" << std::endl;
    return -1;
    }
    }
    }
    }
    }
    return 0;
    }

  */
  /*
    char myIP[16];
    unsigned int myPort;
    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    socklen_t len = sizeof(my_addr);
    getsockname(socket_fd_own, (struct sockaddr *) &my_addr, &len);
    inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
    myPort = ntohs(my_addr.sin_port);

    //std::cout << myIP << ":" << myPort << std::endl;
    std::string portStr(std::to_string(myPort));
    portStr += ';';
    char const *pchar = portStr.c_str(); 
  
    if (send(sockfd, pchar, strlen(pchar), 0) == -1){ 
    std::perror("send port number"); 
    }
  
  */

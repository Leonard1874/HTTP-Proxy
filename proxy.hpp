#include "Request.cpp"
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
class Proxy{
private:
  int sockfd_own;
  int sockfd_to_browser;
  int sockfd_to_origin;
  std::vector<std::string> stringFromBrowser;
public:
  Proxy() = default;
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
  
public:
  int listenBrowser(const char* hostname, const char* port);
  int getRequest(std::string& reuqest);
  int getServerSendBrowser(Request& reqObj, std::string& getInfo);
  
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
  
  bool recieve_origin(int sockfd, std::string& toGet){
    int numbytes = 0;
    int recvNum = 0;
    int allLen = 0;
    int len = 0;
    int headlen = 0;
    int hasGot = 0;
    char temp[65536];
    while(true){
   
      if((numbytes = recv(sockfd, temp, 65535, 0)) == -1) {
        std::perror("recv");
        return false;
      }

      hasGot += numbytes;
      std::string tempStr(temp);
      std::cout << tempStr << std::endl;
      if(!recvNum){
        if(tempStr.find("HTTP/1.1 200 OK") == std::string::npos){
          allLen = 0;
        }
        else{
          len = getLength(tempStr);
          if(len < 0){
            return false;
          }
          headlen = getHeadLength(tempStr);
          if(headlen < 0){
            return false;
          }
          allLen = headlen + len;
        }
        recvNum ++;
      }

      std::cout << hasGot << " : " << allLen << std::endl;
      
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
  
  bool recieve(int sockfd, std::string& toGet){
    int numbytes = 0;
    char temp[65536];
    while(true){
      memset(temp,'\0',sizeof(temp));
      if((numbytes = recv(sockfd, temp, 65535, 0)) == -1) {
        std::perror("recv");
        return false;
      }
      std::string tempStr(temp);
      if(numbytes < 65536){
        toGet += temp;
        //std::cout << "end! " <<toGet << std::endl;
        break;
      }
      else{
        //std::cout << toGet << std::endl;
        toGet += temp;
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
  
public:
  ~Proxy(){}
};
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

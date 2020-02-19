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

  int getSockfdOwn(){
    return sockfd_own;
  }

  int getSockfdB(){
    return sockfd_to_browser;
  }

  int getSockfdO(){
    return sockfd_to_origin;
  }

  bool response_parser(std::string & hostname, std::string& log_raw){
    size_t position = log_raw.find("Host:");
    if(position == std::string::npos){
      std::cout<<"We cannot find the hostname\n";
      return false;
    }
    else{
      size_t i = position;
      while(i < log_raw.size()){
        if(log_raw[i] != ' '){
          i++;
        }
        else{
          i ++;
          break;
        }
      }
    
      while(i < log_raw.size()-1){
        hostname += log_raw[i];
        i++;
      }
      return true;
    }
  }


  std::vector<std::string> parseInputLines(std::string& recv){
    std::stringstream ss(recv);
    std::string temp;
    std::vector<std::string> res;
    while(std::getline(ss,temp,'\n')){
      temp[temp.size()-1] = '\n';
      res.push_back(temp);
    }
    return res;
  }
  
  bool Send(int sendFd, std::string toSend){
    if (send(sendFd, toSend.c_str(), strlen(toSend.c_str()), 0) == -1){ 
      std::perror("send port number");
      return false;
    }
    return true;
  }
  
  bool recieve(int sockfd, std::string& toGet){
    int numbytes = 0;
    char temp[65536];
    std::string endMark = "\0\r\n\r\n";
    while(true){
      memset(temp,'\0',sizeof(temp));
      if((numbytes = recv(sockfd, temp, 65535, 0)) == -1) {
        std::perror("recv");
        return false;
      }
      std::string tempStr(temp);
      if(tempStr.find(endMark) != std::string::npos){
        toGet += temp;
        //std::cout << "end! " <<toGet << std::endl;
        break;
      }
      else{
        //std::cout << toGet << std::endl;
        toGet += temp;
      }
    }
    //std::cout << "client: received " << toGet << std::endl;
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

  ~Proxy(){
    close(sockfd_own);
    close(sockfd_to_browser);
    close(sockfd_to_origin);
  }
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

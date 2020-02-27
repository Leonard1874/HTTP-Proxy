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
  //int sockfd_own;
  //int sockfd_to_browser;
  //int sockfd_to_origin;
  std::vector<std::string> stringFromBrowser;
  
public:
  WebToolKit() = default;
  /*
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
  */
  
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

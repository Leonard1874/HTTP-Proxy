#include "Request.hpp"
#include "Timer.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
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

using namespace std;
class logger{
private:
  std::string send_requestline;
  std::string recieve_requestline;
  Timer logTimer;
public:
  //Print the content into the logfile
  void print_recieve_requestline(){
    string filePath = "./var/log/erss/proxy.log";
    ofstream os(filePath, ios::app );
    os << recieve_requestline;
    os.close();
  }
  void print_send_requestline(){
    string filePath = "./var/log/erss/proxy.log";
    ofstream os(filePath, ios::app );
    os << send_requestline;
    os.close();
  }

  void printCache(string content,int& ID){
    ID++;
    string temp;
    temp += to_string(ID);
    temp += ": ";
    temp  = temp + content +"\n";
    string filePath = "./var/log/erss/proxy.log";
    ofstream os(filePath, ios::app );
    os << temp;
    os.close();
  }
  


  void getrequest_time(int& ID, Request request){
    std::string requestInfo = request.getRequestInfo();
    std::string firstLine;
   
    size_t i = 0 ;
    while(requestInfo[i] != '\r' && i < requestInfo.size()){
      firstLine += requestInfo[i];
      i++;
    }
    hostent * record = gethostbyname(request.getHostname().c_str());
    if(record == NULL){
        cerr<< "Can not get ip address"<<endl;
        exit(1);
      }
    in_addr * address = (in_addr * )record->h_addr;
    string ip_address = inet_ntoa(* address);
    std::string temp;
    //Refresh recieve_requestline
    recieve_requestline = temp;
    ID++;
    recieve_requestline += std::to_string(ID);
    recieve_requestline += ": ";
    recieve_requestline += firstLine;//First line in the request
    recieve_requestline += " from ";
    recieve_requestline += ip_address;
    recieve_requestline += " @ ";
    recieve_requestline = recieve_requestline + logTimer.getCurrentDateTime("now") + "\n"; 
  }
  void getrequest_requesting(int& ID, Request request){
    std::string requestInfo = request.getRequestInfo();
    std::string firstLine;
   
    size_t i = 0 ;
    while(requestInfo[i] != '\r' && i < requestInfo.size()){
      firstLine += requestInfo[i];
      i++;
    }
    hostent * record = gethostbyname(request.getHostname().c_str());
    if(record == NULL){
        cerr<< "Can not get ip address"<<endl;
        exit(1);
      }
    in_addr * address = (in_addr * )record->h_addr;
    string ip_address = inet_ntoa(* address);
    std::string temp;
    //Refresh send_requestline
    send_requestline = temp;
    ID++;
    send_requestline += std::to_string(ID);
    send_requestline += ": ";
    send_requestline += "Requesting ";
    send_requestline += firstLine;//First line in the request
    send_requestline = send_requestline + " from " + request.getHostname() + "\n";
  }
};

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "Request.hpp"
#include "Timer.hpp"

using namespace std;
class logger {
 private:
  std::string send_requestline;
  std::string recieve_requestline;
  std::string response_recieveline;
  std::string response_sendline;
  Timer logTimer;

 public:
  logger() = default;
  /*
  //lazy initialization
  logger* getLogger(){
    if(thisLogger == NULL){
      return new logger();
    }
    else{
      return thisLogger;
    }
  }
  */
  //Print the content into the logfile
  void print_response_recieveline() {
    string filePath = "./var/log/erss/proxy.log";
    ofstream os(filePath, ios::app);
    os << response_recieveline;
    os.close();
  }
  void print_response_sendline() {
    string filePath = "./var/log/erss/proxy.log";
    ofstream os(filePath, ios::app);
    os << response_sendline;
    os.close();
  }

  void print_recieve_requestline() {
    string filePath = "./var/log/erss/proxy.log";
    ofstream os(filePath, ios::app);
    os << recieve_requestline;
    os.close();
  }
  void print_send_requestline() {
    string filePath = "./var/log/erss/proxy.log";
    ofstream os(filePath, ios::app);
    os << send_requestline;
    os.close();
  }

  void printCache(string content, int & ID) {
    string temp;
    temp += to_string(ID);
    temp += ": ";
    temp = temp + content + "\n";
    string filePath = "./var/log/erss/proxy.log";
    ofstream os(filePath, ios::app);
    os << temp;
    os.close();
  }

  void getrequest_time(int & ID, Request request) {
    std::string requestInfo = request.getRequestInfo();
    std::string firstLine;

    size_t i = 0;
    while (requestInfo[i] != '\r' && i < requestInfo.size()) {
      firstLine += requestInfo[i];
      i++;
    }
    hostent * record = gethostbyname(request.getHostname().c_str());
    if (record == NULL) {
      cerr << "Can not get ip address" << endl;
      exit(1);
    }
    in_addr * address = (in_addr *)record->h_addr;
    string ip_address = inet_ntoa(*address);
    std::string temp;
    //Refresh recieve_requestline
    recieve_requestline = temp;
    recieve_requestline += std::to_string(ID);
    recieve_requestline += ": ";
    recieve_requestline += firstLine;  //First line in the request
    recieve_requestline += " from ";
    recieve_requestline += ip_address;
    recieve_requestline += " @ ";
    recieve_requestline = recieve_requestline + logTimer.getCurrentDateTime("now") + "\n";
  }

  void getrequest_requesting(int & ID, Request request) {
    std::string requestInfo = request.getRequestInfo();
    std::string firstLine;

    size_t i = 0;
    while (requestInfo[i] != '\r' && i < requestInfo.size()) {
      firstLine += requestInfo[i];
      i++;
    }
    hostent * record = gethostbyname(request.getHostname().c_str());
    if (record == NULL) {
      cerr << "Can not get ip address" << endl;
      exit(1);
    }
    in_addr * address = (in_addr *)record->h_addr;
    string ip_address = inet_ntoa(*address);
    std::string temp;
    //Refresh send_requestline
    send_requestline = temp;
    send_requestline += std::to_string(ID);
    send_requestline += ": ";
    send_requestline += "Requesting ";
    send_requestline += firstLine;  //First line in the request
    send_requestline = send_requestline + " from " + request.getHostname() + "\n";
  }
  void getresponse_recieve(int & ID, string responseInfo, string hostname) {
    std::string firstLine;
    size_t i = 0;
    while (responseInfo[i] != '\r' && i < responseInfo.size()) {
      firstLine += responseInfo[i];
      i++;
    }
    std::string temp;
    //Refresh send_requestline
    response_recieveline = temp;
    response_recieveline += std::to_string(ID);
    response_recieveline += ": ";
    response_recieveline += "Recieved ";
    response_recieveline += firstLine;  //First line in the response
    response_recieveline = send_requestline + " from " + hostname + "\n";
  }
  void getresponse_send(int & ID, string responseInfo) {
    std::string firstLine;
    size_t i = 0;
    while (responseInfo[i] != '\r' && i < responseInfo.size()) {
      firstLine += responseInfo[i];
      i++;
    }
    std::string temp;
    //Refresh send_requestline
    response_sendline = temp;
    response_sendline += std::to_string(ID);
    response_sendline += ": ";
    response_sendline += "Responding ";
    response_sendline = response_sendline + firstLine + '\n';
  }
};

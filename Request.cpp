#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>
#include <iostream>
#include <vector>
#include <map>
#include <list>

/*This class is for reuqest strings, designed for high cohesion, it will get the recivevd clinet request string, parse the hostname and port itself and save the string, the host name and the port to fields, the use get to get them. If there is no port, port will be an empty string. During the call of Proxy::getServersendbrowser, port need to be checked. */

class Request{
private:
  std::string requestInfo;
  std::string hostname;
  std::string port;
  void parseRequest(){
    size_t pos = requestInfo.find("Host: "); //exception? To be done
    std::string hostLine = requestInfo.substr(pos+strlen("Host: "));
    size_t endPos = hostLine.find_first_of("\r\n");
    hostLine = hostLine.substr(0,endPos); // get this line
    //std::cout << "/" <<hostLine << "/" <<std::endl;
    size_t portPos = hostLine.find(":");
    if(portPos != std::string::npos){
      hostname = hostLine.substr(0,portPos);
      port = hostLine.substr(portPos);
    }
    else{
      port = "";
      hostname = hostLine;
    }
    //std::cout << "/" << hostname << "/:/" << port << "/" <<std::endl;
  }
  
public:
  Request(const std::string& reqInfo):requestInfo(reqInfo){
    parseRequest();
  }

  std::string getHostname(){
    return hostname;
  }

  std::string getPort(){
    return port;
  }

  std::string getRequestInfo(){
    return requestInfo;
  }
  
};
/*
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
      //std::cout << temp.length() << std::endl;
      res.push_back(temp);
    }
    return res;
  }
  */

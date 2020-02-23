#include <cstdio>
#include <cstdlib>
#include <cstdio>
#include <string>

class logger{
 private:
  std::string logline;
 public:
  //Print the logline for testing, later will redirect it into a file
  void printlogline(){
    std::cout<< "Logline:"<<logline<<std::endl;
  }
 void getrequest(int ID, Request request){
   std::string requestInfo = request.getRequestInfo();
   std::string requestLine;
   size_t i = 0 ;
     while(requestInfo[i] != '\r' && i < requestInfo.size()){
       requestLine += requestInfo[i];
       i++;
     }
   std::string temp;
   //Refresh logline
   logline = temp;
   ID++;
   logline += std::to_string(ID);
   logline += ": ";
   logline += "Requesting ";
   logline += requestLine;//First line in the request or response
   logline = logline + " from " + request.getHostname();
 }
};

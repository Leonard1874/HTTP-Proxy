#include <cstdio>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;
class logger{
 private:
  std::string send_requestline;
  std::string recieve_requestline;
 public:
  //Print the content into the logfile
  void printlogline(){
string filePath = "./var/log/erss/proxy.log";
 ofstream os(filePath, ios::app );
 os << recieve_requestline;
 os << send_requestline;
 os.close();
  }

  void printCache(string content,int ID){
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


  string getCurrentDateTime( string s ){
    time_t now = time(0);
    struct tm  tstruct;
    char  buf[80];
    tstruct = *localtime(&now);
    if(s=="now")
        strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    else if(s=="date")
        strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
    return string(buf);
}
  
 void getrequest(int& ID, Request request){
   std::string requestInfo = request.getRequestInfo();
   std::string firstLine;
   
   size_t i = 0 ;
     while(requestInfo[i] != '\r' && i < requestInfo.size()){
       firstLine += requestInfo[i];
       i++;
     }
     hostent * record = gethostbyname(request.getHostname().c_str());
     if(record == NULL)
       {
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
	recieve_requestline = recieve_requestline + getCurrentDateTime("now") + "\n"; 
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

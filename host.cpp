#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

int client(const char* hostname,const char *port);

bool response_parser(string & hostname, string & port, string& log_raw){
  size_t position = log_raw.find("Host:");
  if(position == string::npos){
    cout<<"We cannot find the hostname\n";
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
    
    while( i < log_raw.size()){
      if(log_raw[i] != ':'){
	hostname += log_raw[i];
	i++;
      }
      else{
	i++;
	break;
      }
    }
    
    while(i < log_raw.size()){
      if(isdigit(log_raw[i])){
	port += log_raw[i];
	i++;
      }
      else{
	break;
      }
    }
    return true;
  }
}

int main(int argc, char *argv[])
{
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = "vcm-12398.vm.duke.edu";
  const char *port     = "12345";

  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags    = AI_PASSIVE;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  int yes = 1;
  status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot bind socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  status = listen(socket_fd, 100);
  if (status == -1) {
    cerr << "Error: cannot listen on socket" << endl; 
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  cout << "Waiting for connection on port " << port << endl;
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_connection_fd;
  client_connection_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  if (client_connection_fd == -1) {
    cerr << "Error: cannot accept connection on socket" << endl;
    return -1;
  } //if

  char buffer[1000];
  int numbytes = recv(client_connection_fd, buffer, 999, 0);
  buffer[numbytes] = '\0';
  string log_raw(buffer);

  cout << "Server received: " << buffer << endl;
  string hostname_client;
  string port_client;

  if(!response_parser(hostname_client, port_client, log_raw)){
    cout << "Wrong format of log\n";
  }
  
  int socket_client_fd = client(hostname_client.c_str(), port_client.c_str());

  //send to org server
  const char *message = log_raw.c_str();
  send(socket_client_fd, message, strlen(message), 0);

  //recv org server
  char buffer_client[1000];
  int num = recv(socket_client_fd, buffer_client, 999, 0);
  buffer_client[num] = '\0';
  cout << "Server received: " << buffer_client << endl;

  const char* temp = "<html><head><title>500 Internal Server Error</title></head><body bgcolor=\"white\"><center><h1>500 Internal Server Error</h1></center><hr><center>Server</center></body></html>";

  //send back
   send(client_connection_fd, temp, strlen(temp), 0);

  close(socket_fd);
  close(socket_client_fd);
  close(client_connection_fd);
  freeaddrinfo(host_info_list);
  
  return 0;
}


int client(const char* hostname, const char* port)
{
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;

  cout << "/" << hostname << "/" <<endl;
  cout << "/" <<port << "/" << endl;
  
  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port, &host_info, &host_info_list);
  if (status != 0) {
    cerr << "Error: cannot get address info for host" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if

  socket_fd = socket(host_info_list->ai_family, 
		     host_info_list->ai_socktype, 
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    cerr << "Error: cannot create socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if
  
  cout << "Connecting to " << hostname << " on port " << port << "..." << endl;
  
  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    cerr << "Error: cannot connect to socket" << endl;
    cerr << "  (" << hostname << "," << port << ")" << endl;
    return -1;
  } //if
  freeaddrinfo(host_info_list);
 
  return socket_fd;
}

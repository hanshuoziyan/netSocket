#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/time.h>//select
#include <unistd.h>//close

#include <string.h>
#include <iostream>
#include <stdlib.h>
using namespace std;
#define TCP_SERVER_PORT 8000
#define UDP_SERVER_PORT 8001
#define SEND_BUF 500
int tcp_socket_fd;
int udp_socket_fd, udp_connect_fd;

void initsocket(){
	if((tcp_socket_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1){
	cout<<"create socket error\n";
	exit(0);
	}
	struct sockaddr_in tcp_server_addr, udp_server_addr;
	memset(&tcp_server_addr, 0, sizeof(tcp_server_addr));
	tcp_server_addr.sin_family = AF_INET;
	tcp_server_addr.sin_addr.s_addr = inet_addr("202.118.236.187");
	tcp_server_addr.sin_port = htons(TCP_SERVER_PORT);

   if(connect(tcp_socket_fd, (struct sockaddr*)&tcp_server_addr, sizeof(tcp_server_addr)) < 0){
      cout<<"connect error\n";
      exit(0);
   }
   char buf[SEND_BUF] = "haha, server, i am tcp";
   buf[SEND_BUF-1] = '\0';

   send(tcp_socket_fd, buf, strlen(buf), 0);
}


int main(){
	initsocket();
   close(tcp_socket_fd);
	return 0;
}

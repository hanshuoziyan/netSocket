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
#define TCP_CONNECT_NUM 5
#define RECEIVE_BUF 500
int tcp_socket_fd;
int udp_socket_fd, udp_connect_fd;

void initsocket(){
	if((tcp_socket_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1){
	cout<<"create socket error\n";
	exit(0);
	}
	struct sockaddr_in tcp_serveraddr, udp_serveraddr;
	memset(&tcp_serveraddr, 0, sizeof(tcp_serveraddr));
	tcp_serveraddr.sin_family = AF_INET;
	tcp_serveraddr.sin_addr.s_addr = inet_addr("202.118.236.187");
	tcp_serveraddr.sin_port = htons(TCP_SERVER_PORT);
	if(bind(tcp_socket_fd, (struct sockaddr*)&tcp_serveraddr, sizeof(tcp_serveraddr)) == -1){
	cout<<"bind error \n";
	exit(0);
	}
	//设置非阻塞模式
	//unsigned long nMode = 1;
	if(fcntl(tcp_socket_fd, F_SETFL, O_NONBLOCK) == -1){
	cout<<"ioctl error \n";
	exit(0);
	}
	if(listen(tcp_socket_fd, TCP_CONNECT_NUM) == -1){
	cout<<"listen error \n";
	exit(0);
	}
	cout<<"start to accept \n";
}

fd_set m_accfdRead;
timeval m_tv;
int m_tcpClientList[TCP_CONNECT_NUM];//socket 数组
int clientNum = 0;

void init_fd(){
   m_tv.tv_sec = 100;
   m_tv.tv_usec = 0;

   FD_ZERO(&m_accfdRead);
   for(int i = 0; i < TCP_CONNECT_NUM; i++){
      m_tcpClientList[i] = 0;
   }   
}

int tcp_listen_socket(){
   struct sockaddr_in client_addr;
   socklen_t client_size = sizeof(client_addr);
   int tcp_connect_fd = accept(tcp_socket_fd, (struct sockaddr*)&client_addr, &client_size);
   if(tcp_connect_fd <= 0){
      cout<<"connect error\n";
      return 0;
   }
   if(clientNum < TCP_CONNECT_NUM){
      for(int i = 0; i < TCP_CONNECT_NUM; i++){
         if(m_tcpClientList[i] == 0){
            m_tcpClientList[i] = tcp_connect_fd;
            clientNum++;
            break;
         }
      }
   }else{
      cout<<"max connection is reached..\n";
      char a[50] = "max connection";
      a[49] = '\0';
      send(tcp_connect_fd, a, strlen(a), 0);
      close(tcp_connect_fd);
   }
   return 1;
}

int tcp_accept_socket(){
   for(int i = 0; i < TCP_CONNECT_NUM; i++){
      if(m_tcpClientList[i] >0 && FD_ISSET(m_tcpClientList[i], &m_accfdRead)){
         char buf[RECEIVE_BUF];
         int ret = recv(m_tcpClientList[i], buf, sizeof(buf), 0);
         if(ret <= 0){
            cout<<"receive err\n";
            close(m_tcpClientList[i]);
            m_tcpClientList[i] = 0;
            clientNum--; 
         }else{
            buf[ret] = '\0';
            cout<<"client say: "<<buf<<endl;
         }
      }   
   }
}

void select_fd(){
   while(1){
      //初始化设备描述表
      FD_ZERO(&m_accfdRead);
      FD_SET(tcp_socket_fd, &m_accfdRead);
      
      int maxfdp = tcp_socket_fd;
      int i = 0;
      for(; i < TCP_CONNECT_NUM; i++){
         int tmp = m_tcpClientList[i];
         if(tmp != 0){
            if(tmp > maxfdp)
               maxfdp = tmp;
            FD_SET(tmp, &m_accfdRead);
         }
      }

      int ret = select(maxfdp+1, &m_accfdRead, NULL, NULL, &m_tv);
      if(ret == 0){
         cout<<"timeout, continue select .....\n";
         continue;
      }else if(ret < 0){
         cout<<"select error\n";
         break;
      }
      tcp_accept_socket();
      if(FD_ISSET(tcp_socket_fd, &m_accfdRead)){
         tcp_listen_socket();
      }
      
   }
}
int main(){
	initsocket();
   init_fd();
   select_fd();
	return 0;
}

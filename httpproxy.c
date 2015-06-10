// 服务器端
#include <iostream>
#include<string>
#include <event.h>
#include<netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<malloc.h>
#include<fstream>
using namespace std;

struct event_base* main_base;

static const char MESSAGE[] ="Hello, World!\n";
//void on_read(int sock,short event,void * arg)
void on_read(int sock)
{
     int recvbyte=0;
//		do{
     //struct event* ev=(struct event *)arg ;
     char buffer[10250];
    //bzero(buffer,MEM_SIZE);
	    // memset(buffer,0,sizeof(buffer));
     int size=recv(sock,buffer,10250,0);
     printf("client -> proxy:\n%s",buffer);
     printf("\n");
     if(size<=0)
   {
    //    event_del(ev);free(ev);
       close(sock);
        return;
     }
    string sendss="";
		sendss.append(buffer,size);
		size_t i0=sendss.find(" ");
    string tempss=sendss.substr(0,i0+1);
		sendss.erase(0,i0+1);
		size_t ii=sendss.find("//");
		sendss.erase(0,ii+2);
		ii=sendss.find("/");
		sendss.erase(0,ii);
		//sendss=sendss.substr(ii+1);
		size_t i=sendss.find("Host: ");
    tempss+=sendss.substr(0,i);
		sendss.erase(0,i);
		tempss+="Connection: close\r\n";
		string hostname;
		size_t i1=sendss.find("\r\n");
    hostname=sendss.substr(6,i1-6);
		tempss+=sendss;
       cout<<"proxy -> server\r\n"<<tempss<<endl;
		cout<<"cyc host name:"<<hostname<<endl;
       //proxy connect dest
     int ss;
     struct  sockaddr_in destaddr;

		 hostent* he=gethostbyname(hostname.c_str());
    // bzero(&destaddr,sizeof(struct sockaddr_in));
		 char **pptr=he->h_addr_list;
     destaddr.sin_family=AF_INET;
    // destaddr.sin_addr.s_addr=inet_addr("119.75.217.109");//119.75.217.109
		 destaddr.sin_addr=*((struct in_addr*)he->h_addr);
		 cout<<"ip address:"<<inet_ntoa(*(struct in_addr* )*pptr)<<endl;
     destaddr.sin_port=htons(80);
		 memset(destaddr.sin_zero,'\0',sizeof destaddr.sin_zero);
     ss=socket(AF_INET,SOCK_STREAM,0);
     char buffer1[10240];
    //FILE *fp;
     //ofstream fp("baidu.txt"); 
     //fp=fopen("/tmp/cyc/cyc.txt","w+");
     if(connect(ss,(struct sockaddr *)&destaddr,sizeof destaddr)==-1)
     {
          cout<<"connct fail"<<endl;
          return ; 
     }
     
				// char buffer2[2048];
       send(ss,tempss.c_str(),tempss.size(),0);
	     cout<<"ddddddd\n"<<endl;
			 string str;
			 int countbyte=0;
			 do{
	     //memset(buffer1,0,sizeof(buffer1));
       recvbyte=recv(ss,buffer1,10240,0);

	    // cout<<"recvbyte:"<<recvbyte<<endl;
       if(recvbyte<=0)
		break ;
			 send(sock,buffer1,recvbyte,0);
			 countbyte+=recvbyte;
			 str.append(buffer1,recvbyte);
			 }while(recvbyte>0);
     //  fp<<buffer1;
	    // fwrite(buffer1,1,recvbyte,fp);
			 //char * pp=str.c_str();
//			 send(sock,str.c_str(),countbyte,0);
			 cout<<"baidu data:"<<countbyte<<endl<<str<<endl;
      // FILE *bf ;
      // bf=fopen(fill,"r+")
       //fclose(fp);
     
	//	}while(recvbyte>0);
    // send(fd,buffer1,sizeof(buffer1),0);
}
//void accept_handle(const int sfd, const short event, void *arg)
void accept_handle(const int sfd)
{
	   cout<<"accept handle"<<endl;

		 while(1)
		 {
	   struct sockaddr_in addr;

	   socklen_t addrlen = sizeof(addr);
     int fd=0;
     fd = accept(sfd, (struct sockaddr *) &addr, &addrlen); //处理连接
    if(!fork())
		{
				close(sfd);
				on_read(fd);
				return ;
		}
		close(fd);
		 }
		 return ;
//     struct event *read_ev=(struct event *)malloc(sizeof(struct event));
//     event_set(read_ev,fd,EV_READ|EV_PERSIST,on_read,read_ev);
  //   event_base_set(main_base,read_ev);
    //    event_add(read_ev,NULL);       
//     struct bufferevent* buf_ev;

	//   buf_ev = bufferevent_new(fd, NULL, NULL, NULL, NULL);

	  // buf_ev->wm_read.high = 4096;

	  // cout<<"event write"<<endl;
     //char MESSAGE1[]="\000\x5aUVWXYZ";
       // MESSAGE1[0]=0x00;
        //MESSAGE1[1]=0x5A;
        //MESSAGE1[2]='\0';
	//bufferevent_write(buf_ev, MESSAGE1, 8);//strlen(MESSAGE1));
       

}

int main()
{
	cout<<"hello man!"<<endl;

	// 1. 初始化EVENT
//	main_base = event_init();
	//if(main_base)
	//	cout<<"init event ok!"<<endl;

	// 2. 初始化SOCKET
	int sListen;

	// Create listening socket
	sListen = socket(AF_INET, SOCK_STREAM,0);// IPPROTO_TCP);

	// Bind
	struct sockaddr_in server_addr;
	//bzero(&server_addr,sizeof(struct sockaddr_in));
	server_addr.sin_family=AF_INET;
	//server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  server_addr.sin_addr.s_addr=INADDR_ANY;//inet_addr("192.168.72.134");
	int portnumber = 9080;
	server_addr.sin_port = htons(portnumber);
  memset(server_addr.sin_zero,'\0',sizeof server_addr.sin_zero);
	/* 捆绑sockfd描述符  */
	if(bind(sListen,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)
	{
		cout<<"error!"<<endl;
		return -1;
	}
        cout<<"bind sucess"<<endl;
	// Listen
	::listen(sListen, 5);
	cout<<"Server is listening!\n"<<endl;

	/*将描述符设置为非阻塞*/
	//int flags = ::fcntl(sListen, F_GETFL);

	//flags |= O_NONBLOCK;short a=0;
	accept_handle(sListen);
  //      struct event ev;
        
	//fcntl(sListen, F_SETFL, flags);
    //    accept_handle(sListen,a,&ev);
	// 3. 创建EVENT 事件
//	struct event ev;
//	event_set(&ev, sListen, EV_READ | EV_PERSIST, accept_handle, (void *)&ev);

	// 4. 事件添加与删除
	//event_add(&ev, NULL);

	// 5. 进入事件循环
	//event_base_loop(main_base, 0);
// event_dispatch();
	cout<<"over!"<<endl;
	return 0;
}

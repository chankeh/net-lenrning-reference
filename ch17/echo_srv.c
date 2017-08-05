#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <sys/socket.h>  
#include <sys/epoll.h>
  
#define BUF_SIZE    100
#define EPOLL_SIZE	50
  
void error_handling(char *message);  
  
int main(int argc, char *argv[]){  
    int serv_sock,clnt_sock;  
    char buf[BUF_SIZE];  
    struct sockaddr_in serv_adr;  
    struct sockaddr_in clnt_adr;  
    socklen_t adr_sz;  
    int str_len,i;
    struct epoll_event *ep_events;
    struct epoll_event event;
    int epfd,event_cnt;  

    if(argc != 2){  
        printf("Usage : %s <port>\n",argv[0]);  
        exit(1);  
    }  

    serv_sock = socket(PF_INET,SOCK_STREAM,0);  
    if(serv_sock == -1){  
        error_handling("socket error");  
    }  
  
    memset(&serv_adr,0,sizeof(serv_adr));  
    serv_adr.sin_family=AF_INET;  
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);  
    serv_adr.sin_port=htons(atoi(argv[1]));  
  
    if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr)) == -1){  
        error_handling("bind() error");  
    }  
  
    if(listen(serv_sock,5) == -1){  //自动生成服务器端套接字（守门员）
        error_handling("listen() error");  
    }  
    
    epfd = epoll_create(EPOLL_SIZE);//创建epoll例程，返回例程的文件描述符
    ep_events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);
    //用于保存发生事件的文件描述符
    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd,EPOLL_CTL_ADD,serv_sock,&event);
    //将serv_sock添加到例程空间中
    while(1){ 
	event_cnt = epoll_wait(epfd,ep_events,EPOLL_SIZE,-1);
    //返回发生事件的文件描述符个数
	if(event_cnt  == -1){
	    puts("epoll wait error");
	    break;
	}

	for(i=0;i<event_cnt;i++){
	    if(ep_events[i].data.fd == serv_sock){
		adr_sz = sizeof(clnt_adr);
        	clnt_sock = accept(serv_sock,(struct sockaddr *)&clnt_adr,&adr_sz);  
		    //处理请求套接字生成
		event.events = EPOLLIN;
		event.data.fd = clnt_sock;
		epoll_ctl(epfd,EPOLL_CTL_ADD,clnt_sock,&event);
		//将发生请求的套接字添加到epoll例程中
		printf("connected client : %d \n", clnt_sock);
	    }else{	//read message
		//这个地方设计的可能有点啰嗦，不如直接写入，这里先通过读，判断了字符串数据读取的字节数
		str_len = read(ep_events[i].data.fd,buf,BUF_SIZE);
        //将文件描述符的缓冲区里的数据拷贝到buf缓冲文件中
		if(str_len == 0){	//close
		    epoll_ctl(epfd,EPOLL_CTL_DEL,ep_events[i].data.fd,NULL);
			//如果文件描述符的缓冲中没有数据，就将该fd从epoll例程中移除
		    close(ep_events[i].data.fd);//同时关闭该文件描述符对应的套接字
		    printf("closed client %d \n",ep_events[i].data.fd);
		}else{
			write(ep_events[i].data.fd,buf,str_len);	//echo
            //从buf里把数据写入到文件描述符对应的套接字缓冲区里

		}	    
	    }
	}
    }  
    close(serv_sock);  
    close(epfd);
    return 0;  
}  
  
void error_handling(char *message){  
  
    fputs(message,stderr);  
    fputs("\n",stderr);  
    exit(1);  
}
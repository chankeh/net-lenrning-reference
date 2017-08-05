#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <sys/socket.h>  
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
  
#define BUF_SIZE    4
#define EPOLL_SIZE	50
  
void error_handling(char *message);  
void setnonblockingmode(int fd);
  
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
  
    if(listen(serv_sock,5) == -1){  
        error_handling("listen() error");  
    }  
    
    epfd = epoll_create(EPOLL_SIZE);
    ep_events = malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

    event.events = EPOLLIN;
    event.data.fd = serv_sock;
    epoll_ctl(epfd,EPOLL_CTL_ADD,serv_sock,&event);

    while(1){ 
	event_cnt = epoll_wait(epfd,ep_events,EPOLL_SIZE,-1);

	if(event_cnt  == -1){
	    puts("epoll wait error");
	    break;
	}

	puts("return epoll wait");
	for(i=0;i<event_cnt;i++){
	    if(ep_events[i].data.fd == serv_sock){
		adr_sz = sizeof(clnt_adr);
        	clnt_sock = accept(serv_sock,(struct sockaddr *)&clnt_adr,&adr_sz);  
		setnonblockingmode(clnt_sock);
		event.events = EPOLLIN | EPOLLET;//设置成边缘触发
		event.data.fd = clnt_sock;
		epoll_ctl(epfd,EPOLL_CTL_ADD,clnt_sock,&event);
		printf("connected client : %d \n", clnt_sock);
	    }else{	//read message
		while(1){
		    str_len = read(ep_events[i].data.fd,buf,BUF_SIZE);

		    if(str_len == 0){	//close
		    	epoll_ctl(epfd,EPOLL_CTL_DEL,ep_events[i].data.fd,NULL);
		    	close(ep_events[i].data.fd);
		    	printf("closed client %d \n",ep_events[i].data.fd);
		    	break;
		    }else if(str_len < 0){
			if(errno == EAGAIN)
			{//无数据可读
			    break;
			}
		    }else{
			write(ep_events[i].data.fd,buf,str_len);	//echo
		    }
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

void setnonblockingmode(int fd)
{
    int flag = fcntl(fd,F_GETFL,0);//设置成非阻塞形式
    fcntl(fd,F_SETFL,flag | O_NONBLOCK);
}
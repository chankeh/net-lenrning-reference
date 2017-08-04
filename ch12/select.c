#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>  
#include <sys/time.h>
#include <sys/select.h>
  
#define BUF_SIZE    100
  
void error_handling(char *message);  
  
int main(int argc, char *argv[]){  
    int serv_sock,clnt_sock;  
    char buf[BUF_SIZE];  
    struct sockaddr_in serv_adr;  
    struct sockaddr_in clnt_adr;  
    socklen_t adr_sz;  
    fd_set reads,cpy_reads;
    struct timeval timeout;
    int fd_max,str_len,fd_num,i;
  
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
    
    FD_ZERO(&reads);
    FD_SET(serv_sock,&reads);
    fd_max = serv_sock;//为什么要用serv_sock作为最大文件描述符，这个地方没看懂

    while(1){ 
	cpy_reads = reads;
	timeout.tv_sec = 5;
	timeout.tv_usec = 5000;

	if((fd_num = select(fd_max+1,&cpy_reads,0,0,&timeout)) == -1)
	    break;
	if(fd_num == 0)	
	    continue;
	for(i=0;i<fd_max+1;i++){
	    if(FD_ISSET(i,&cpy_reads)){
		if(i == serv_sock){	//connection request
		    adr_sz = sizeof(clnt_adr);
        	    clnt_sock = accept(serv_sock,(struct sockaddr *)&clnt_adr,&adr_sz);  
		    FD_SET(clnt_sock,&reads);	
		
		    if(fd_max < clnt_sock){
			fd_max = clnt_sock;
		    }
		    printf("connected client : %d \n", clnt_sock);
		}else{	//read message
		    str_len = read(i,buf,BUF_SIZE);

		    if(str_len == 0){	//close
			FD_CLR(i,&reads);
			close(i);
			printf("closed client %d \n",i);
		    }else{
			write(i,buf,str_len);	//echo
		    }	    
		}
	    }
	}
    }  
    close(serv_sock);  
  
    return 0;  
}  
  
void error_handling(char *message){  
  
    fputs(message,stderr);  
    fputs("\n",stderr);  
    exit(1);  
}
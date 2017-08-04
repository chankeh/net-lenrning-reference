#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>  
  
#define BUF_SIZE    30  
void error_handling(char *message);  
void read_childproc(int sig);
  
int main(int argc, char *argv[]){  
    int serv_sock,clnt_sock;  
    pid_t pid;
    struct sigaction act;
    char buf[BUF_SIZE];  
    int str_len,state;  
    struct sockaddr_in serv_adr;  
    struct sockaddr_in clnt_adr;  
    socklen_t adr_sz;  
      
    if(argc != 2){  
        printf("Usage : %s <port>\n",argv[0]);  
        exit(1);  
    }  

    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD,&act,0);

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
  
    while(1){ 
    	adr_sz = sizeof(clnt_adr);  
        clnt_sock = accept(serv_sock,(struct sockaddr *)&clnt_adr,&adr_sz);  //创建请求套接字
        if(clnt_sock == -1){  
            continue;
        }else{  
            printf("new client ...\n");  
        }  

	pid = fork();//创建子进程

	if(pid == -1){
	    close(clnt_sock);//创建失败直接continue
	    continue;
	}else if(pid == 0){
	    close(serv_sock);//若是子进程，就关闭服务器套接字，子进程
	    while((str_len = read(clnt_sock,buf,BUF_SIZE)) != 0){
	   	write(clnt_sock,buf,str_len);
	    }
	
	    close(clnt_sock);
	    puts("client disconnected...");
	    return 0;
	}else{
	    close(clnt_sock);
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

void read_childproc(int sig){
    pid_t pid;
    int status;
    pid = waitpid(-1,&status, WNOHANG);
    printf("removed proc id : %d \n",pid);
}  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 	10
void error_handling(char *message);

int main(int argc, char *argv[]){
	int serv_sock;
	int clnt_sock;

	char message[BUF_SIZE];
	int str_len,i;
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	
	if(argc != 2){
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}
	serv_sock = socket(PF_INET,SOCK_STREAM,0);
	if(serv_sock == -1){
		error_handling("socket() error");
	}

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));

	if(bind(serv_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1){
		error_handling("bind() error");
	}

	if(listen(serv_sock,5) == -1){
		error_handling("listen() error");
	}

	clnt_addr_size = sizeof(clnt_addr);
	for(i=0;i<5;i++){
		clnt_sock = accept(serv_sock,(struct sockaddr *)&clnt_addr,&clnt_addr_size);
		if(clnt_sock == -1){
			error_handling("accept() error");
		}else{
			printf("Connected client %d\n",i+1);
		}
		while((str_len = read(clnt_sock,message,BUF_SIZE)) != 0)
		{	
			write(clnt_sock,message,str_len);
			printf("client %d:message %s",i+1,message);
		}
		close(clnt_sock);
	}
	close(serv_sock);

	return 0;

}

void error_handling(char *message){

	fputs(message,stderr);
	fputs("\n",stderr);
	exit(1);
}

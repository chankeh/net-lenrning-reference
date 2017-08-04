#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE	10
void error_handling(char *message);

int main(int argc,char *argv[]){
	int sock;
	struct sockaddr_in serv_addr;
	char message[BUF_SIZE];
	int str_len;

	if(argc != 3){
		printf("Usage : %s <IP> <port>\n",argv[0]);
		exit(1);
	}

	sock = socket(PF_INET,SOCK_STREAM,0);
	if(sock == -1){
		error_handling("socket() error");
	}
	
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) == -1){
		error_handling("connect() error\r\n");
	}else{
		printf("Connected....");
	}

	while(1){
		printf("Input message(Q to quit):");
		fgets(message,BUF_SIZE,stdin);

		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
			break;
		write(sock,message,strlen(message));
		str_len = read(sock,message,BUF_SIZE-1);
		message[str_len] = 0;

		printf("Message from server : %s \n",message);
	}
	close(sock);
	return 0;
}

void error_handling(char *message){
	fputs(message,stderr);
	fputs("\n",stderr);
	exit(1);
}

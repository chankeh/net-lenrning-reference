#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE	1024
void error_handling(char *message);

int main(int argc,char *argv[]){
	int sock;
	struct sockaddr_in serv_addr;
	char message[BUF_SIZE];
	char recv_message[BUF_SIZE]="";
	int str_len,recv_len,recv_cnt;

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
		printf("Connected....\n");
	}

	while(1){
		recv_cnt = 1;
		int count = 0;
		while(recv_cnt>0){
			recv_cnt = read(sock,message,10);//返回成功读取的字节数
			printf("Current Msg : %s \n",message); 
			strcat(recv_message,message);
			printf("recv_cnt = %d\n",recv_cnt);
		    if(recv_cnt == -1)
			{
				error_handling("read() error!");
				break;
			}
			count+=recv_cnt;
		}
		recv_message[count-1] = 0;
		printf("Client get Msg : %s \n",recv_message);  
		sleep(3);
		if(recv_cnt==0)
		   break;
	}
	
	close(sock);
	return 0;
}

void error_handling(char *message){
	fputs(message,stderr);
	fputs("\n",stderr);
	exit(1);
}

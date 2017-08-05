#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <semaphore.h>
#include <pthread.h>

#define BUF_SIZE	100
#define NAME_SIZE	20

void error_handling(char *message);
void * send_msg(void * arg);
void * recv_msg(void * arg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc,char *argv[]){
	int sock;
	struct sockaddr_in serv_adr;
	pthread_t snd_thread,rcv_thread;
	void * thread_return;

	if(argc != 4){
		printf("Usage : %s <IP> <port> <name>\n",argv[0]);
		exit(1);
	}

	sprintf(name,"[%s]",argv[3]);
	sock = socket(PF_INET,SOCK_STREAM,0);
	if(sock == -1){
		error_handling("socket() error");
	}
	
	memset(&serv_adr,0,sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	if(connect(sock,(struct sockaddr *)&serv_adr,sizeof(serv_adr)) == -1){
		error_handling("connect() error\r\n");
	}else{
		printf("Connected....");
	}

	pthread_create(&snd_thread,NULL,send_msg,(void *)&sock);
	pthread_create(&rcv_thread,NULL,recv_msg,(void *)&sock);

	pthread_join(snd_thread,&thread_return);
	pthread_join(rcv_thread,&thread_return);

	close(sock);
	return 0;
}

void error_handling(char *message){
	fputs(message,stderr);
	fputs("\n",stderr);
	exit(1);
}

void * send_msg(void *arg){
	int sock = *((int *)arg);
	char name_msg[NAME_SIZE + BUF_SIZE];

	while(1){
		fgets(msg,BUF_SIZE,stdin);
		if(!strcmp(msg,"q\n") || ! strcmp(msg,"Q\n")){
			close(sock);
			exit(0);
		}

		sprintf(name_msg,"%s %s",name,msg);
		write(sock,name_msg,strlen(name_msg));
	}

	return NULL;
}

void * recv_msg(void * arg){
	int sock = *((int *)arg);
	char name_msg[NAME_SIZE + BUF_SIZE];
	int str_len;

	while(1){
		str_len = read(sock,name_msg,NAME_SIZE+BUF_SIZE-1);

		if(str_len == -1){
			return (void *) -1;
		}
	
		name_msg[str_len] = 0;
		fputs(name_msg,stdout);
	}
	
	return NULL;
}
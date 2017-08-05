
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>

#define BUF_SIZE 	100
#define MAX_CLNT	256

void * handle_clnt(void * arg);
void send_msg(char *msg,int len);
void error_handling(char *message);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[]){
	int serv_sock;
	int clnt_sock;
	struct sockaddr_in serv_adr;
	struct sockaddr_in clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;//创建线程id

	if(argc != 2){
		printf("Usage : %s <port>\n",argv[0]);
		exit(1);
	}
	pthread_mutex_init(&mutx,NULL);//因为是多线程，所以要创建互斥量，以便后期互斥访问临界区
	
	serv_sock = socket(PF_INET,SOCK_STREAM,0);
	if(serv_sock == -1){
		error_handling("socket() error");
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
		clnt_adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
		pthread_mutex_lock(&mutx);//通过加锁机制，保护临界区
		clnt_socks[clnt_cnt++] = clnt_sock;//临界区代码
		pthread_mutex_unlock(&mutx);//释放临界区

		pthread_create(&t_id,NULL,handle_clnt,(void *)&clnt_sock);
		pthread_detach(t_id);//销毁线程
		printf("Connetcted client IP :%s \n",inet_ntoa(clnt_adr.sin_addr));
	}

	close(serv_sock);

	return 0;
}

void error_handling(char *message){

	fputs(message,stderr);
	fputs("\n",stderr);
	exit(1);
}

void * handle_clnt(void * arg){
	int clnt_sock = *((int *)arg);
	int str_len =0;
	int i;
	char msg[BUF_SIZE];

	while((str_len = read(clnt_sock,msg,sizeof(msg))) != 0)
		send_msg(msg,str_len);

	pthread_mutex_lock(&mutx);//临界区代码加锁
	for(i=0;i<clnt_cnt;i++){
		if(clnt_sock == clnt_socks[i]){
			while(i++ < clnt_cnt - 1)
				clnt_socks[i] = clnt_socks[i+1];
			break;
		}
	}

	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);

	return NULL;
}

//send to all
void send_msg(char *msg,int len)
{
	int i;
	
	pthread_mutex_lock(&mutx);
	for(i=0;i<clnt_cnt;i++){
		write(clnt_socks[i],msg,len);
	}

	pthread_mutex_unlock(&mutx);
}
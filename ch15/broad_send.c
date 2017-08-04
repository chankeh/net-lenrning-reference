#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <sys/socket.h>  
  
#define BUF_SIZE    30
  
void error_handling(char *message);  
  
int main(int argc, char *argv[]){  
    int send_sock;
    FILE *fp;  
    char buf[BUF_SIZE];  
    struct sockaddr_in broad_adr;
    int so_brd = 1;
  
    if(argc != 3){  
        printf("Usage : %s <group><port>\n",argv[0]);  
        exit(1);  
    }  

    send_sock = socket(PF_INET,SOCK_DGRAM,0);  
    if(send_sock == -1){  
        error_handling("socket() error");  
    }  
  
    memset(&broad_adr,0,sizeof(broad_adr));  
    broad_adr.sin_family=AF_INET;  
    broad_adr.sin_addr.s_addr=inet_addr(argv[1]);	//ip  
    broad_adr.sin_port=htons(atoi(argv[2]));  //port
  
    setsockopt(send_sock,SOL_SOCKET,SO_BROADCAST,(void *)&so_brd,sizeof(so_brd));
    if((fp = fopen("news.txt","r")) == NULL){
  	error_handling("fopen");
    }

    while(!feof(fp)){
	fgets(buf,BUF_SIZE,fp);
	sendto(send_sock,buf,strlen(buf),0,(struct sockaddr *)&broad_adr,sizeof(broad_adr));
	sleep(2);
    }
    fclose(fp);
    close(send_sock);

    return 0;  
}  
  
void error_handling(char *message){  
  
    fputs(message,stderr);  
    fputs("\n",stderr);  
    exit(1);  
}  
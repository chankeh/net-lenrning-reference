#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <sys/socket.h>  
  
#define BUF_SIZE    30
  
void error_handling(char *message);  
  
int main(int argc, char *argv[]){  
    int recv_sock;
    int str_len;
    char buf[BUF_SIZE];  
    struct sockaddr_in adr;
    struct ip_mreq join_adr;
      
    if(argc != 3){  
        printf("Usage : %s <group><port>\n",argv[0]);  
        exit(1);  
    }  

    recv_sock = socket(PF_INET,SOCK_DGRAM,0);  
    if(recv_sock == -1){  
        error_handling("socket() error");  
    }  
    printf("addr init\n");
	printf("recv_sock is :%d\n",recv_sock);
    memset(&adr,0,sizeof(adr));  
    adr.sin_family=AF_INET;  
    adr.sin_addr.s_addr=htonl(INADDR_ANY);	//multi ip  
    adr.sin_port=htons(atoi(argv[2]));  //port
    printf("bind\n");
    if(bind(recv_sock,(struct sockaddr *)&adr,sizeof(adr))==-1){
	error_handling("bind error");
    }

    join_adr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);

    setsockopt(recv_sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,(void *)&join_adr,sizeof(join_adr));
    
	//str_len = recvfrom(recv_sock,buf,BUF_SIZE-1,0,NULL,0);//
	//printf("%d\n",str_len);
	printf("while()\n");
    while(1){//
	    printf("in while!\n");
    	str_len = recvfrom(recv_sock,buf,BUF_SIZE-1,0,NULL,0);
		printf("str_len = %d\n",str_len);
		printf("%s\n",buf);
	if(str_len < 0){
	    break;
	}
	buf[str_len] = 0;
	printf("%s\n",buf);
	fputs(buf,stdout);
    }
    close(recv_sock);

    return 0;  
}  
  
void error_handling(char *message){  
  
    fputs(message,stderr);  
    fputs("\n",stderr);  
    exit(1);  
}  
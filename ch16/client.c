#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int main(int argc, char * argv[]){
	int sock;
	FILE *readfp;
	FILE *writefp;
	struct sockaddr_in serv_adr;
	socklen_t clnt_adr_sz;
	char buf[BUF_SIZE];

	sock = socket(PF_INET,SOCK_STREAM,0);

	memset(&serv_adr,0,sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));

	connect(sock,(struct sockaddr *)&serv_adr,sizeof(serv_adr));

	readfp = fdopen(sock,"r");
	writefp = fdopen(sock,"w");

	while(1){
	    if(fgets(buf,sizeof(buf),readfp) == NULL){
		break;
	    }

	    fputs(buf,stdout);
    	    fflush(stdout);
	}

	fputs("from client: thank you\n",writefp);
	fflush(writefp);
	fclose(readfp);
	fclose(writefp);

	return 0;
}
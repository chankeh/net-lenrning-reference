#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig){
    int status;

    pid_t id = waitpid(-1, &status, WNOHANG);

    if(WIFEXITED(status)){
	printf("Removed proc id : %d \n",id);
	printf("child send : %d \n",WEXITSTATUS(status));
    }
}

int main(int argc, char *argv[]){
    pid_t pid;
    struct sigaction act;

    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGCHLD,&act,0);
    
    pid = fork();

    if(pid == 0){
	//child 
   	puts("Hi, I'm child process");
  	sleep(5);
   	return 12;
    }else{
	printf("Child proc1 id : %d \n",pid);

 	pid = fork();

	if(pid ==0){
	    puts("Hi, sec child");
	    sleep(5);
	    exit(24);
	}else{
	    int i;
	    printf("child proc2 id : %d \n",pid);
	    for(i=0;i<10;i++){
		puts("wait...");
		sleep(1);
	    }
	}
    }
    return 0;
}
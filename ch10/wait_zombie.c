#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    int status;
    pid_t pid;

    pid = fork();

    if(pid == 0){
    	return 3;
    }else{
	printf("Child1 PID :%d \n",pid);
   	pid = fork();

  	if(pid == 0){
	    exit(7);
	}else {
			printf("Child2 PID :%d \n",pid);
			wait(&status);
		
			if(WIFEXITED(status)){
				printf("Child one : %d \n",WEXITSTATUS(status));
			}

			wait(&status);

			if(WIFEXITED(status)){
				printf("Child two : %d \n",WEXITSTATUS(status));
			}

			sleep(5);
		}
    }

    return 0;
}
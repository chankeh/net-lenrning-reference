#include <stdio.h>
#include <unistd.h>

int gval = 10;
int main(int argc, char *argv[]){
    pid_t pid;
    int lval = 20;

    gval++;
    lval += 5;

    pid = fork();

    if(pid == 0){
	//child 
	gval += 2;
	lval += 2;
    }else{
	gval -= 2;
  	lval -= 2;
    }

    if(pid == 0){
	printf("child gval = %d, lval = %d \n",gval,lval);
    }else{
	printf("parent gval = %d, lval = %d \n",gval,lval);
    }
    return 0;
}


/*
 输出：
	parent gval = 9, lval = 23
	child gval = 13, lval = 27
	因为fork后分成了两个完全不同的进程，只是二者共享代码而已
*/
#include <stdio.h>
#include <unistd.h>

int main(){
	int tubo[2];
	pipe(tubo);
	pid_t p = fork();
	if(p==0){		//hijo (ls)
		dup2(tubo[1],1);
		close(tubo[0]);
		execlp("ls","ls",NULL);
	}else if(p>0){	//padre
		dup2(tubo[0],0);
		close(tubo[1]);
		execlp("wc","wc",NULL);
	}else{
		perror("Error creando proceso");
	}
	return 0;
}
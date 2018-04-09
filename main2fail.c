#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAXCOM 10
#define MAXARG 20

typedef struct{
	char *args[MAXARG];	//maximo 20 argumentos
	int narg;
}comando;

int nhijos; 

static void handler(int signum, siginfo_t *info, void * otro){
	if(signum == SIGALRM){
		printf("Teclee un comando\n");
		alarm(5);
	}else if(signum == SIGCHLD){
		printf("info:\n %d\n",(int)info->si_pid);
		int status;
		if(waitpid(-1, &status, WNOHANG) != -1){
			printf("Hijo muerto\n");
			nhijos++;
		}
	}
}


int main(){
	int i,j,k,status;
	pid_t hijos[MAXCOM];
	char *ctok,*atok,*cptr,*aptr;
	char user[50];
	char hostname[50];
	char cwd[100];
	size_t usize = 50;
	size_t hsize = 50;
	getlogin_r(user,usize);
	comando comandos[MAXCOM];		//almacenar comandos 
	int ncom;
	gethostname(hostname,hsize);

	struct sigaction sa;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	//sigfillset(&sa.sa_mask);
	if(sigaction(SIGALRM, &sa, NULL) == -1 || sigaction(SIGCHLD, &sa, NULL) == -1 ){
		perror("Error sigaction\n");
	}
	char buffer[1024];
	alarm(5);

	while(1){
		int error = 0;
		nhijos = 0;
		ncom = 0;					//numero de comando de linea
		getcwd(cwd,100);
		printf("[%s@%s %s](OwO)>> ",user,hostname,strrchr(cwd,'/')+1);	// strrchr da posicion de ultima ocurrencia del caracter
		if(fgets(buffer,1024,stdin)) alarm(10);
		if(strcmp(buffer,"set on\n")==0){
			alarm(5);
			continue;
		}else if(strcmp(buffer,"set off\n")==0){
			alarm(0);
			continue;
		}else if(strcmp(buffer,"exit\n")==0) exit(0);
		
		printf("%s\n",buffer);
		cptr=buffer;
		ctok = strtok_r(buffer,"|\n",&cptr);
		
		while(ctok!=NULL){
			//printf("comando: %s\n",ctok);	//separo argumentos 
			int narg = 0;
			aptr=ctok;
			atok = strtok_r(ctok," ",&aptr);
			while(atok!=NULL){
				//printf(" argumento: (%s)\n",atok);
				//printf("%d\n",narg);
				comandos[ncom].args[narg]=(char*)malloc(50);	//doy espacio para el comando (debe ser liberado despues!)
				strcpy(comandos[ncom].args[narg],atok);
				narg++;
				atok = strtok_r(NULL," \n\0",&aptr);
			}
			comandos[ncom].narg=narg;
			comandos[ncom].args[narg]=NULL;						//ultimo puntero es NULL
			ncom++;
			ctok = strtok_r(NULL,"|\n",&cptr);
		}
		int npipes = ncom -1;
		int pipes[MAXCOM-1][2];
		for(i=0;i<npipes;i++){
			pipe(pipes[i]);
		}
		
		for(i=0;i<ncom;i++){
			if(ncom==1){	//si es solo un comando no uso pipes
				pid_t pid=fork();

				if(pid==0){
					//printf("ejecuto %s",comandos[0].args[0]);
					if(execvp(comandos[0].args[0],comandos[0].args)<0) {
						perror("Error al ejecutar");
						kill(getppid(),SIGCHLD);
					}
					//printf("fdsfsdf\n");
				}else if(pid<0){
					perror("Error al crear hijo\n");
				}else{
					hijos[0]=pid;
				}
				break;
			}
			pid_t pid=fork();
			//hijos[i]=pid;
			if(pid==0){									//proceso hijo
				if(i==0){								//primer comando solo abre salida
					//solo hago pipe de salida
					dup2(pipes[i][1],1);				//redirijo su salida
					close(pipes[i][0]);
					
				}else if(i==ncom-1){					//ultimo comando solo abre entrada
					dup2(pipes[i-1][0],0);
					close(pipes[i-1][1]);
				}else{
					dup2(pipes[i-1][0],0);
					dup2(pipes[i][1],1);
					close(pipes[i-1][1]);
					close(pipes[i][0]);
				}
				for(j=0;j<npipes;j++){					//cierro pipes sin usar
					if(j==i-1 || j==i) continue;
					close(pipes[j][0]);
					close(pipes[j][1]);
				}
				if(execvp(comandos[i].args[0],comandos[i].args)<0){ 
					perror("Error al ejecutar");
					kill(getppid(),SIGCHLD);
				}
			}else if(pid<0){							//fallo al crear proceso
				perror("Error creando proceso\n");
			}
		}
		for(i=0;i<npipes;i++){				//padre cierra las pipes
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
		//espero a los hijos
		while(1){
			//printf("ncom = %d nhijos = %d\n",ncom,nhijos);
			if(ncom == nhijos) break;
		}
		//libero memoria pedida
		for(i=0;i<ncom;i++){
			int narg = comandos[i].narg;
			for(j=0;j<narg;j++){
				free(comandos[i].args[j]);				//libero memoria pedida
			}
		}
		strcpy(buffer,"");				// limpia el buffer
		/*
		for(i=0;i<ncom;i++){
			printf("comando %d:\n",i);
			int narg = comandos[i].narg;
			for(j=0;j<narg;j++){
				printf("%s\n",comandos[i].args[j]);
			}
		}
		*/
	}
	return 0;
}
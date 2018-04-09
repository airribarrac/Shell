#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXCOM 10
#define MAXARG 20

typedef struct{
	char *args[MAXARG];	//maximo 20 argumentos
	int narg;
}comando;

int main(int argc , char *argv[]){
	int i,j,k,status;
	pid_t hijos[MAXCOM];
	char buffer[1024];
	char *ctok,*atok,*cptr,*aptr;
	char user[50];
	size_t usize = 50;
	char hostname[50];
	size_t hsize = 50;
	getlogin_r(user,usize);
	comando comandos[MAXCOM];		//almacenar comandos 
	int ncom;
	char cwd[100];
	gethostname(hostname,hsize);
	while(1){
		ncom = 0;					//numero de comando de linea
		getcwd(cwd,100);
		printf("[%s@%s %s] (OwO)>> ",user,hostname,strrchr(cwd,'/')+1);	// strrchr da posicion de ultima ocurrencia del caracter
		fgets(buffer,1024,stdin);
		if(buffer[strlen(buffer)-1]=='\n') buffer[strlen(buffer)-1]='\0';
		if(strcmp(buffer,"exit")==0){
			printf("Adios %s!\n",user);
			exit(0);
		}

		cptr=buffer;
		ctok = strtok_r(buffer,"|",&cptr);
		while(ctok!=NULL){
			//printf("comando: %s\n",ctok);	//separo argumentos 
			int narg = 0;
			aptr=ctok;
			atok = strtok_r(ctok," ",&aptr);
			while(atok!=NULL){
				//printf(" argumento: (%s)\n",atok);
				//printf("%d\n",narg);
				comandos[ncom].args[narg]=(char*)malloc(50);	//doy espacio para el comando (debe ser liberado despues!)
				strcpy(comandos[ncom].args[narg],atok);			//50 chars debería ser mas que suficiente por argumento
				narg++;
				atok = strtok_r(NULL," \0",&aptr);
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

				if(strcmp("cd",comandos[0].args[0])==0){
					if(chdir(comandos[0].args[1])<0){
						perror("Error al cambiar de directorio");
					}
					break;
				}

				pid_t pid=fork();

				if(pid==0){
					//printf("ejecuto %s",comandos[0].args[0]);
					if(execvp(comandos[0].args[0],comandos[0].args)<0) perror("Error al ejecutar");
					exit(-1);
					//printf("fdsfsdf\n");
				}else if(pid<0){
					perror("Error al crear hijo\n");
				}else{
					hijos[0]=pid;
				}
				break;
			}
			pid_t pid=fork();
			hijos[i]=pid;
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
				if(execvp(comandos[i].args[0],comandos[i].args)<0) perror("Error al ejecutar");
				exit(-1);
			}else if(pid<0){							//fallo al crear proceso
				perror("Error creando proceso\n");
			}
		}
		for(i=0;i<npipes;i++){				//padre cierra las pipes
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
		//espero a los hijos
		for(int i=0;i<ncom;i++){
			waitpid(hijos[i],&status,WCONTINUED);
		}
		//libero memoria pedida
		for(i=0;i<ncom;i++){
			int narg = comandos[i].narg;
			for(j=0;j<narg;j++){
				free(comandos[i].args[j]);				//libero memoria pedida
			}
		}
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
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAXCOM 10
#define MAXARG 20

typedef struct{
	char *args[MAXARG];	//maximo 20 argumentos
	int narg;
}comando;

int main(){
	int i,j,k;
	char buffer[1024];
	char *ctok,*atok,*cptr,*aptr;
	char user[50];
	size_t usize = 50;
	char hostname[50];
	size_t hsize = 50;
	getlogin_r(user,usize);
	comando comandos[MAXCOM];		//almacenar comandos 
	for(i=0;i<MAXCOM;i++){
		for(j=0;j<MAXARG;j++){
			comandos[i].args[j]=(char*)malloc(50);
		}
	}
	int ncom;
	char cwd[100];
	gethostname(hostname,hsize);
	while(1){
		ncom = 0;					//numero de comando de linea
		getcwd(cwd,100);
		printf("[%s@%s][ %s ](OwO)>> ",user,hostname,cwd);
		fgets(buffer,1024,stdin);
		ctok = strtok_r(buffer,"|\n",&cptr);
		while(ctok!=NULL){
			//printf("comando: %s\n",ctok);	//separo argumentos 
			int narg = 0;
			atok = strtok_r(ctok," ",&aptr);
			while(atok!=NULL){
				//printf(" argumento: (%s)\n",atok);
				//printf("%d\n",narg);
				strcpy(comandos[ncom].args[narg],atok);
				//printf("chao\n");
				narg++;
				atok = strtok_r(NULL," \n\0",&aptr);
			}
			comandos[ncom].narg=narg;
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
				int pid=fork();
				if(pid==0){
					printf("ejecuto %s",comandos[0].args[0]);
					if(execvp(comandos[0].args[0],comandos[0].args)<0) perror("Error al ejecutar");
					printf("fdsfsdf\n");
				}else if(pid<0){
					perror("Error al crear hijo\n");
				}
				break;
			}
			int pid=fork();
			if(pid==0){									//proceso hijo
				if(i==0){								//primer comando solo abre salida

				}else if(i==ncom-1){					//ultimo comando solo abre entrada

				}else{

				}
			}else if(pid<0){							//fallo al crear proceso
				perror("Error creando proceso\n");
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
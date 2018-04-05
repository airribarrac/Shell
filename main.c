#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
	char *name,*arguments[];	//comando
}command;

int main(){
	char buffer[1024];
	char *ctok,*atok,*cptr,*aptr;
	char user[50];
	size_t usize = 50;
	char hostname[50];
	size_t hsize = 50;
	getlogin_r(user,usize);
	command comandos[10];		//almacenar comandos con sus nombres para ejecutar
	char cwd[100];
	gethostname(hostname,hsize);
	while(1){
		getcwd(cwd,100);
		printf("[%s@%s][ %s ](OwO)>> ",user,hostname,cwd);
		fgets(buffer,1024,stdin);
		ctok = strtok_r(buffer,"|\n",&cptr);
		while(ctok!=NULL){
			printf("comando: %s\n",ctok);	//separo las weas de argumentos maldita sea
			atok = strtok_r(ctok," ",&aptr);
			while(atok!=NULL){
				printf(" argumento: (%s)\n",atok);
				atok = strtok_r(NULL," \n\0",&aptr);
			}
			ctok = strtok_r(NULL,"|\n",&cptr);
		}
	}
	return 0;
}
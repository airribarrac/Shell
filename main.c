#include <stdio.h>
#include <unistd.h>

typedef struct{
	char *name,**arguments;	//comando, arreglo de argumentos
}command;

int main(){
	char buffer[1024];
	char user[50];
	size_t usize = 50;
	char hostname[50];
	size_t hsize = 50;
	getlogin_r(user,usize);
	char *cwd=get_current_dir_name();
	gethostname(hostname,hsize);
	printf("%s",cwd);
	while(1){
		printf("[%s@%s](UwU)>> ",user,hostname);
		fgets(buffer,1024,stdin);
	}
	return 0;
}
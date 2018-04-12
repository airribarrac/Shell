#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define MAXCOM 10
#define MAXARG 20

typedef struct{
	char *args[MAXARG];	//maximo 20 argumentos
	int narg;
}comando;

struct nodo{
	int pid;
	clock_t time;
	int status;
	comando comandos[MAXCOM];
	struct nodo *next;
};

void push(struct nodo **head,int pid, clock_t time, int status, comando com[]){
	if(*head == NULL){
		puts("null");
		(*head) = (struct nodo*)malloc(sizeof(struct nodo));
		(*head)->pid = pid;
		(*head)->time = time;
		(*head)->status = status;
		int i;
		for(i = 0; i<MAXCOM; i++){
			(*head)->comandos[i] = com[i];
		}
		(*head)->next = NULL;
	}else{
		puts("no null");
		struct nodo *new = (struct nodo*)malloc(sizeof(struct nodo));
		new->next = *head;
		*head = new;
		(*head)->pid = pid;
		(*head)->time = time;
		(*head)->status = status;
		int i;
		for(i = 0; i<MAXCOM; i++){
			(*head)->comandos[i] = com[i];
		}
	}

}

int main(){
	struct nodo* lista;
	puts("wa");
	comando com[MAXCOM];

	com[0].args[0] = "Hola";
	com[3].args[5] = "Chao";

	push(&lista,1,(clock_t)1,3,com);
	push(&lista,2,(clock_t)2,50,com);
	push(&lista,3,(clock_t)3,100,com);

	while(lista != NULL){
		printf("%d %d %d %s\n", lista->pid, lista->time, lista->status, lista->comandos[0].args[0]);
		lista = lista->next;
	}

	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
 
#define STACKSIZE 32768		/* tamanho de pilha das threads */
 
/* VARIÁVEIS GLOBAIS */
ucontext_t cPing, cPong, cMain;
 
/* Funções-comportamento das Tarefas */
void f_ping(void * arg) { //Toda troca de contexto é feito explicitamente.
   int i;
 
   printf("%s iniciada\n", (char *) arg);
 
   for (i=0; i<4; i++) {
      printf("%s %d\n", (char *) arg, i);
      //Chavemaneto de contexto. Salva o contexto de um e restaura de outro.	
      swapcontext(&cPing, &cPong); 
   }
   printf("%s FIM\n", (char *) arg);
 
   swapcontext(&cPing, &cMain); //Passa o contexto do main.
}
 
void f_pong(void * arg) {
   int i;
 
   printf("%s iniciada\n", (char *) arg);
 
   for (i=0; i<4; i++) {
      printf("%s %d\n", (char *) arg, i);
      swapcontext(&cPong, &cPing);
   }
   printf("%s FIM\n", (char *) arg);
 
   swapcontext(&cPong, &cMain);
}
 
/* MAIN */
int main(int argc, char *argv[]) {
   char *stack;
 
   printf ("Main INICIO\n");
 //----------------------------------------------------------------------------------------------- Ping
   getcontext(&cPing); //Inicialização da estrutura cPing, primeiro salvamento.
   stack = malloc(STACKSIZE); //Alocar uma área dinâmica para ser a pilha do Ping (Heap).
   if(stack) {
      cPing.uc_stack.ss_sp = stack ; //Setaram a pilha que será usada.
      cPing.uc_stack.ss_size = STACKSIZE; //Tamanho da pilha.
      cPing.uc_stack.ss_flags = 0;
      cPing.uc_link = 0;
   }
   else {
      perror("Erro na criação da pilha: ");
      exit(1);
   }
 
   // makecontext(Struct do contexto (cPing), endereço da função (programming counter), arg c, arg v).	
   makecontext(&cPing, (void*)(*f_ping), 1, "\tPing"); 

 //----------------------------------------------------------------------------------------------- Pong
   getcontext(&cPong);
   stack = malloc(STACKSIZE);
   if(stack) {
      cPong.uc_stack.ss_sp = stack ;
      cPong.uc_stack.ss_size = STACKSIZE;
      cPong.uc_stack.ss_flags = 0;
      cPong.uc_link = 0;
   }
   else {
      perror("Erro na criação da pilha: ");
      exit(1);
   }
 
   makecontext (&cPong, (void*)(*f_pong), 1, "\tPong");
 //----------------------------------------------------------------------------------------------- 
   //Chaveamento de contexto. Todo contexto de main é salvo em cMain restaurado cPing.
   swapcontext(&cMain, &cPing); 
   swapcontext(&cMain, &cPong);
 
   printf("Main FIM\n");
 
   exit(0);
}

//Executando:
/*Main INICIO
	Ping iniciada
	Ping 0
	Pong iniciada
	Pong 0
	Ping 1
	Pong 1
	Ping 2
	Pong 2
	Ping 3
	Pong 3
	Ping FIM
	Pong FIM
Main FIM*/


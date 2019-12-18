/*
	CHAVEAMENTO ATRAVÉS DO SINAL
*/

#include<stdio.h>
#include <stdlib.h>
#include<signal.h>
#include<unistd.h>
#include <ucontext.h>
 
#define STACKSIZE 32768		/* tamanho de pilha das threads */
 
#define PING_ID 1
#define PONG_ID 2
 
/* VARIÁVEIS GLOBAIS */
ucontext_t cPing, cPong, cMain;
 
int curr_thread;
//-------------------------------------------------------------------------- 
/* Handler para tratar o sinal */
void sig_handler(int signo)
{
  printf("Turma de SOP: recebido SIGUSR1\n");   
  if (curr_thread==PING_ID) { //Vai ver qual thread está em execução.
     curr_thread=PONG_ID;     //Troca a thread em execução.	
     swapcontext(&cPing, &cPong); //Troca de contexto.   
  } else {
     curr_thread=PING_ID;
     swapcontext(&cPong, &cPing);
  }
}
//--------------------------------------------------------------------------  
/* Funções-comportamento das Tarefas */
void f_ping(void * arg) {
   int i=0;
 
   printf("%s iniciada\n", (char *) arg);
 
   for (;;) {
      printf("%s %d\n", (char *) arg, i++);
      sleep(1);
   }
   printf("%s FIM\n", (char *) arg);
 
 
}
//--------------------------------------------------------------------------  
void f_pong(void * arg) {
   int i=0;
 
   printf("%s iniciada\n", (char *) arg);
 
   for (;;) {
      printf("%s %d\n", (char *) arg, i++);
      sleep(1);
   }
   printf("%s FIM\n", (char *) arg);
}
//--------------------------------------------------------------------------  
void prepara_contexto_ping()
{
   char *stack;
 
   getcontext(&cPing);
   stack = malloc(STACKSIZE);
   if(stack) {
      cPing.uc_stack.ss_sp = stack ;
      cPing.uc_stack.ss_size = STACKSIZE;
      cPing.uc_stack.ss_flags = 0;
      cPing.uc_link = 0;
   }
   else {
      perror("Erro na criação da pilha: ");
      exit(1);
   }
   makecontext(&cPing, (void*)(*f_ping), 1, "\tPing");
}
//--------------------------------------------------------------------------  
void prepara_contexto_pong()
{
   char *stack;
 
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
}
//--------------------------------------------------------------------------  
int main(void)
{
  int x;
 
  printf ("Main INICIO\n");
  prepara_contexto_ping();
  prepara_contexto_pong();
 
  if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
      printf("\nProblemas com SIGUSR1\n");
      exit(-1);
  }
 
  curr_thread=PING_ID; //Ping associado.
  swapcontext(&cMain, &cPing);
  // A long long wait so that we can easily issue a signal to this process
  while(1) {
    sleep(1);
  }
  return 0;
}
//-------------------------------------------------------------------------- 

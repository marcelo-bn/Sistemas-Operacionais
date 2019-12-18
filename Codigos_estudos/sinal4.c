#include<stdio.h>
#include <stdlib.h>
#include<signal.h>
#include<unistd.h>
#include <ucontext.h>
#include <sys/time.h>
 
#define STACKSIZE 32768		/* tamanho de pilha das threads */
 
// number of seconds for setting the interval used by the timer
#define QUANTUM_SEC 0
// number of microseconds for setting the interval used by the timer (0 - 999999)
#define QUANTUM_MICRO_SEC 100000
 
#define PING_ID 1
#define PONG_ID 2
#define PUNG_ID 3
#define TIME_SLICE 2
 
/* VARIÁVEIS GLOBAIS */
ucontext_t cPing, cPong, cPung, cMain;
int curr_thread;
//------------------------------------------------------  
/* Handler para tratar o sinal */
void sig_handler(int signo)
{
  printf("SOP da Turma 2019-2: recebido SIGALRM\n");
  alarm(TIME_SLICE); //TIME_SLICE zera   
  if (curr_thread==PING_ID) {
     curr_thread=PONG_ID;
     swapcontext(&cPing, &cPong);    
  }
  if (curr_thread==PONG_ID){
     curr_thread=PUNG_ID;
     swapcontext(&cPong, &cPung);
  } 
  else if (curr_thread==PUNG_ID){
     curr_thread=PING_ID;
     swapcontext(&cPung, &cPing);		
  }

}
//------------------------------------------------------  
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
//------------------------------------------------------   
void f_pong(void * arg) {
   int i=0;
 
   printf("%s iniciada\n", (char *) arg);
 
   for (;;) {
      printf("%s %d\n", (char *) arg, i++);
      sleep(1);
   }
   printf("%s FIM\n", (char *) arg);
}
//------------------------------------------------------   
void f_pung(void * arg) {
   int i=0;
 
   printf("%s iniciada\n", (char *) arg);
 
   for (;;) {
      printf("%s %d\n", (char *) arg, i++);
      sleep(1);
   }
   printf("%s FIM\n", (char *) arg);
 
 
}
//------------------------------------------------------  
void preparar_contexto_ping()
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
//------------------------------------------------------ 
void preparar_contexto_pung()
{
   char *stack;
 
   getcontext(&cPung);
   stack = malloc(STACKSIZE);
   if(stack) {
      cPung.uc_stack.ss_sp = stack ;
      cPung.uc_stack.ss_size = STACKSIZE;
      cPung.uc_stack.ss_flags = 0;
      cPung.uc_link = 0;
   }
   else {
      perror("Erro na criação da pilha: ");
      exit(1);
   }
   makecontext(&cPung, (void*)(*f_pung), 1, "\tPung");
}
//------------------------------------------------------   
void preparar_contexto_pong()
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
//------------------------------------------------------  
void preparar_handler()
{
  if (signal(SIGALRM, sig_handler) == SIG_ERR) { //Troca do tipo de sinal (SIGALRM)
      printf("\nProblemas com SIGUSR1\n");
      exit(-1);
  }
  alarm(TIME_SLICE); //Chamada ao sistema que contará e enviará o sinal.
}
//----------------------------------------------------- 
int main(void)
{
  int x;
 
  printf ("Main INICIO\n");
  preparar_contexto_ping();
  preparar_contexto_pong();
  preparar_contexto_pung();
  preparar_handler();
  curr_thread=PING_ID; //ajusta primeiro thread
  swapcontext(&cMain, &cPing); //nunca mais volta...
  return 0;
}
//-------------------------------------------------- 

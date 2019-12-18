#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h> /* for makecontext/swapcontext routines */
#include <queue> /* C++ STL queue structure */
#include <vector>
 
#include<signal.h>
#include<unistd.h>
#include <ucontext.h>
#include <sys/time.h>
 
#define TIME_SLICE 5
 
typedef void (*threadFn)(void);
 
class thread_cb {
   int id_thread;
   public:
   ucontext_t contexto;
   thread_cb(threadFn p, int id)
   {
	  getcontext(&contexto);
	  int stackLen=32*1024;
	  char *stack=new char[stackLen];
	  contexto.uc_stack.ss_sp=stack;
	  contexto.uc_stack.ss_size=stackLen;
	  contexto.uc_stack.ss_flags=0;      
     id_thread = id;
     makecontext(&contexto,p,0);
   };
   ucontext_t *get_context() {
     return &contexto;
   };
};
 
std::queue<class thread_cb *> ready_pool;
 
int id_thread = 0;
 
 
class thread_cb *curr_thread=NULL;
 
void add_thread(threadFn func)
{
  class thread_cb *p = new thread_cb(func, ++id_thread);
  ready_pool.push(p);
}
 
 
void dispatcher(ucontext_t *old_task, ucontext_t *new_task)
{
  if (old_task!=NULL)
      swapcontext(old_task, new_task);
  else
      setcontext(new_task);
}
 
void scheduler_rr()
{
    class thread_cb *next,*last;
 
 
   if(curr_thread!=NULL) {
      printf("Aqui\n");
      ready_pool.push(curr_thread);
      last=curr_thread;
	   next=ready_pool.front();
	   ready_pool.pop();
      curr_thread=next;    
      dispatcher(last->get_context(), curr_thread->get_context()); 
   } else { //Não tem nada executando
	   next=ready_pool.front(); // Salva o ponteiro do primeiro da fila
   	ready_pool.pop();//Tira ele da fila
      curr_thread = next;//Associa a quem está executando
      dispatcher(NULL, next->get_context()); //Só volta pra fila quando troca de contexto
   }
}
 
void sig_handler(int signo)
{
 
  printf("SOP da Turma 2019-2: recebido SIGALRM\n");
  alarm(TIME_SLICE);   
 
	if (ready_pool.empty()) {
		printf("Nothing more to run!\n");
		exit(0);
	}
   scheduler_rr();
}
 
void preparar_handler()
{
  if (signal(SIGALRM, sig_handler) == SIG_ERR) {
      printf("\nProblemas com SIGUSR1\n");
      exit(-1);
  }
  alarm(TIME_SLICE);
}
 
void runA(void) {
	for (;;) {
      printf("running A\n");
      sleep(1);
   }
}
 
void runB(void) {
	for (;;) {
      printf("running B\n");
      sleep(1);
   }
}
 
main()
{
  add_thread(runA);
  add_thread(runB);
  preparar_handler();
  for(;;);
}


 #include<stdio.h>
 #include <stdlib.h>
 #include<signal.h>
 #include<unistd.h>
 
 void sig_handler(int signo)
 {
   int x;
   printf("Turma de SOP: recebido SIGUSR1\n");
 }
 
 int main(void)
 {
   int x;
 
   if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
       printf("\nProblemas com SIGUSR1\n");
       exit(-1);
   }
   // Loop  eterno dormindo 1s
   while(1) {
     printf("Esperando sinal\n");
     sleep(1);
   }
   return 0;
 }

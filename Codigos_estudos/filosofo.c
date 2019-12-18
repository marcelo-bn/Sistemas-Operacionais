#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
 
#define NUM_FILOSOFOS 5
 
sem_t chop_stick[NUM_FILOSOFOS];
pthread_mutex_t mutex_cont;
int contador_jantar = 0;
 
int filosofo(int n)
{
	while(true){
		sem_wait(&chop_stick[n]);
		sem_wait(&chop_stick[(n+1)%NUM_FILOSOFOS]);

		//comendo
		pthread_mutex_lock(&mutex_cont);	
		contador_jantar++;
		pthread_mutex_unlock(&mutex_cont);	
	
		sem_post(&chop_stick[n]);
		sem_post(&chop_stick[(n+1)%NUM_FILOSOFOS]);
	}
}
	
 
int main()
{
	pthread_t threads[NUM_FILOSOFOS];
        int i,ret;
 
	// Cria cinco threads que executarão a mesma função
	for(i=0; i<NUM_FILOSOFOS; ++i){
		sem_init(&chop_stick[i],0,1);
		ret = pthread_create(&threads[i], NULL, (void*(*)(void*))filosofo,(void*)((long)i+1));
		if(ret != 0){
			printf("erro\n");
			exit(EXIT_FAILURE);
		}
	}
	// Aguarda o fim das threads
	sleep(15);	
	printf("Valor do contador: %d\n", contador_jantar);
	for(i=0; i<5; ++i){
		//pthread_join(threads[i], NULL);
		sem_destroy(&chop_stick[i]);
 	}
	exit(0);
	
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 5

/* Variaveis globais */
int controle = 0;
pthread_mutex_t mutex;
pthread_cond_t condicao1; /* Condição de bloqueio para as threads 2, 3 e 4. */
pthread_cond_t condicao2; /* Condição de bloqueio para a thread 4. */

/* Thread 5 */
void *thread_5 (void *arg){
	fprintf(stderr, "Seja Bem-vindo!\n");
	pthread_mutex_lock(&mutex);
	controle++;
	pthread_mutex_unlock(&mutex);
	pthread_cond_broadcast(&condicao1);
	pthread_exit(NULL);
}

/* Threads 2, 3 e 4 */
void *threads (void *arg) {
	char *frase = (char *)arg;
	pthread_mutex_lock(&mutex);
	if(controle == 0){
		pthread_cond_wait(&condicao1, &mutex);
	}
	fprintf(stderr, "%s\n", frase);
	controle++;
	if(controle == 4){
		pthread_cond_signal(&condicao2);
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

/* Thread 1 */
void *thread_1 (void *arg){
	pthread_mutex_lock(&mutex);
	if(controle < 4){
		pthread_cond_wait(&condicao2, &mutex);
	}
	fprintf(stderr, "Volte sempre!\n");
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}



/* Funcao principal */
int main(void) {
	
	pthread_t id_threads[NTHREADS];
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init (&condicao1, NULL);
	pthread_cond_init (&condicao2, NULL);

	/* Cria as threads */
	pthread_create(&id_threads[4], NULL, thread_5, NULL);
	pthread_create(&id_threads[3], NULL, threads, (void *)"Aceita um copo d’agua? ");
	pthread_create(&id_threads[2], NULL, threads, (void *)"Sente-se por favor. ");
	pthread_create(&id_threads[1], NULL, threads, (void *)"Fique a vontade. ");
	pthread_create(&id_threads[0], NULL, thread_1, NULL);

	/* Espera todas as threads completarem */
	for (int i = 0; i < NTHREADS; i++) {
		pthread_join(id_threads[i], NULL);
	}

	/* Desaloca variaveis e termina */
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&condicao1);
	pthread_cond_destroy(&condicao2);

	return 0;
}

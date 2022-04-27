#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "timer.h"


typedef struct {
    int id;
    int dimensao;
} threadsArgs;


float *matrizA;
float *matrizB;
float *matrizResultadoSequencial;
float *matrizResultadoConcorrente;
int numeroThreads;

int compararMatrizes(int dimensao){

    for(int i = 0; i < dimensao; i++){
        for(int j = 0; j < dimensao; j++){
            if(matrizResultadoSequencial[i * dimensao + j] != matrizResultadoConcorrente[i * dimensao + j])
                return 1;
        }
    }
    return 0;
}



void * tarefa(void *arg) {

    threadsArgs *args = (threadsArgs *) arg;

    for(int i = args->id; i < args->dimensao; i += numeroThreads){
        for(int j = 0; j < args->dimensao; j++){
            for(int k = 0; k < args->dimensao; k++){
                matrizResultadoConcorrente[i * args->dimensao + j] += matrizA[i * args->dimensao + k] * matrizB[k * args->dimensao + j];
            }
        }
    }
    pthread_exit(NULL);
}


int main (int argc, char *argv[]) {

    double inicio, fim, deltaSequencial, deltaConcorrente; // Para medição de tempo de execução

    int dimensao;
    if(argc < 3) {
        printf("Digite: %s <dimensao das matrizes> <numero de threads>\n", argv[0]);
        return 1;
    }
    dimensao = atoi(argv[1]);
    numeroThreads = atoi(argv[2]);

    if(dimensao < numeroThreads) numeroThreads = dimensao;



    matrizA = (float *)malloc(sizeof(float) * dimensao * dimensao);
    if(matrizA == NULL) {
        printf("ERRO -- malloc\n");
        return 2;
    }
    matrizB = (float *)malloc(sizeof(float) * dimensao * dimensao);
    if(matrizB == NULL) {
        printf("ERRO -- malloc\n");
        return 2;
    }
    matrizResultadoSequencial = (float *)malloc(sizeof(float) * dimensao * dimensao);
    if(matrizResultadoSequencial == NULL) {
        printf("ERRO -- malloc\n");
        return 2;
    }
    matrizResultadoConcorrente = (float *)malloc(sizeof(float) * dimensao * dimensao);
    if(matrizResultadoConcorrente == NULL) {
        printf("ERRO -- malloc\n");
        return 2;
    }



    for(int i = 0; i < dimensao; i++) {
        for(int j = 0; j < dimensao; j++) {
            matrizA[i * dimensao + j] = 10;
            matrizB[i * dimensao + j] = 3;
            matrizResultadoSequencial[i * dimensao + j] = 0;
            matrizResultadoConcorrente[i * dimensao + j] = 0;
        }
    }


    GET_TIME(inicio);
    /* ------------------ Multiplicação de Matrizes SEQUENCIAL ------------------ */

    for(int i = 0; i < dimensao; i++){
        for(int j = 0; j < dimensao; j++){
            for(int k = 0; k < dimensao; k++){
                matrizResultadoSequencial[i * dimensao + j] += matrizA[i * dimensao + k] * matrizB[k * dimensao + j];
            }
        }
    }

    /* ----------------------------------------------------------------------------------------------------- */
    GET_TIME(fim);
    deltaSequencial = fim - inicio;




    GET_TIME(inicio);
    /* ------------------ Multiplicação de Matrizes CONCORRENTE ------------------ */

	pthread_t *tid;
	threadsArgs *args;

	tid = (pthread_t *)malloc(sizeof(pthread_t) * dimensao);
	if(tid == NULL){
		puts("ERRO -- malloc");
		return 2;
	}
	args = (threadsArgs *)malloc(sizeof(threadsArgs) * dimensao);
	if(args == NULL){
		puts("ERRO -- malloc");
		return 2;
	}

    for(int i = 0; i < numeroThreads; i++){
		(args + i)->id = i;
		(args + i)->dimensao = dimensao;

		pthread_create(tid + i, NULL, tarefa, (void *)(args + i));

	}

	for(int i = 0; i < numeroThreads; i++){
		pthread_join(*(tid + i), NULL);
	}

    /* ----------------------------------------------------------------------------------------------------- */
    GET_TIME(fim);
    deltaConcorrente = fim - inicio;




    int resultado = compararMatrizes(dimensao);
    if(resultado == 0) printf("Saida sequencial é IGUAL a saida concorrente!\n");
    else printf("ERRO: Saida sequencial é DIFERENTE da saida concorrente!\n");

    printf("\nTempo de execução SEQUENCIAL: %lf\n", deltaSequencial);
    printf("Tempo de execução Concorrente: %lf\n", deltaConcorrente);
    printf("Valor do Ganho: %lf\n", deltaSequencial / deltaConcorrente);



    free(matrizA);
    free(matrizB);
    free(matrizResultadoSequencial);
    free(matrizResultadoConcorrente);
    free(args);
    free(tid);

    return 0;
}

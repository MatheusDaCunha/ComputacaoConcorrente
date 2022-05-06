#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define NUMERO_DE_ELEMENTOS 0
#define NUMERO_DE_THREADS 0

/* Essa definição serve para passar todos argumentos necessários para cada thread. */
typedef struct{
    int id;
    float menor;
    float maior;
}ArgumentosDaThreads;


/* Descritores de funções */
void inicializarVetor();
void exibirSaida();
void buscaMenorMaior_Sequencial();
void buscaMenorMaior_Concorrente();
void *tarefa(void *);


/* Variáveis Globais */
float *vetorDeNumerosReais;
long long int numeroDeElementos = NUMERO_DE_ELEMENTOS;
int numeroDeThreads = NUMERO_DE_THREADS;
double inicio, fim, deltaSequencial = 0.0, deltaConcorrente = 0.0; /* São utilizadas para a medição do tempo de execução. */
long double vetorDeRetorno[4]; /* Recebe o resultado gerado pela função 'buscaMenorMaior_Sequencial' e 'buscaMenorMaior_Concorrente'. */



int main(int argc, char *argv[]){

    /* Garantia de confiabilidade dos parâmetros de entrada */
    if(argc < 3){
        printf("Digite: %s <número de elementos> <número de threads>\n", argv[0]);
        return 1;
    }
    numeroDeElementos = atoll(argv[1]);
    numeroDeThreads = atoi(argv[2]);
    if(numeroDeThreads > numeroDeElementos) numeroDeThreads = numeroDeElementos;

    /* Alocação de memória para o vetor e verificação de sucesso */
    vetorDeNumerosReais = (float *)malloc(sizeof(float) * numeroDeElementos);
    if(vetorDeNumerosReais == NULL){
        printf("ERRO: Memória insuficiente. Tente um N menor.\n");
        return 2;
    }

    /* Chamada da função que inicializa o vetor por completo */
    inicializarVetor();
    vetorDeRetorno[0] = vetorDeRetorno[1] = vetorDeRetorno[2] = vetorDeRetorno[3] = vetorDeNumerosReais[0];

    /* Chamada da função SEQUENCIAL que busca o menor e o maior elemento do vetor */
    buscaMenorMaior_Sequencial();

    /* Chamada da função CONCORRENTE que busca o menor e o maior elemento do vetor */
    buscaMenorMaior_Concorrente();

    /* Chamada da função que exibe o vetor */
    exibirSaida();

    /* Liberação de memória usada pelo programa */
    free(vetorDeNumerosReais);

    return 0;
}


/* Inicializa o vetor y com x elementos aleatórios do tipo float. */
void inicializarVetor(){
    for(long long int i = 0; i < numeroDeElementos; i++)
        vetorDeNumerosReais[i] = (102000.13 / rand()) + (1000.34 / (i + 1)); /* Gera um float aleatório. */
    return;
}


/*
* Exibe os elementos do vetor na saída padrão.
* Exibe os resultados do maior e menor encontrados.
* Exibe os tempos de execução.
*/
void exibirSaida(){
    printf("\nSEQUENCIAL -\n\tO MENOR número encontrado é %.7Lf\n", vetorDeRetorno[0]);
    printf("\tO MAIOR número encontrado é %.7Lf\n", vetorDeRetorno[1]);
    printf("\nCONCORRENTE -\n\tO MENOR número encontrado é %.7Lf\n", vetorDeRetorno[2]);
    printf("\tO MAIOR número encontrado é %.7Lf\n\n", vetorDeRetorno[3]);
    printf("Tempo SEQUENCIAL: %lf segundos\n", deltaSequencial);
    printf("Tempo CONCORRENTE: %lf segundos\n", deltaConcorrente);
    printf("Aceleração: %lf\n", deltaSequencial / deltaConcorrente);
    return;
}


/*
* Aqui a função conpara elemento por elemento e verifica o menor e o maior usando força bruta.
* Aqui é usado a abordagem sequencial.
* O vetor 'vetorDeNumerosReais' guarda o menor e o maior valores na posição 0 e 1, respectivamente.
*/
void buscaMenorMaior_Sequencial(){
    GET_TIME(inicio);
    for(long long int i = 0; i < numeroDeElementos; i++){
        if(vetorDeNumerosReais[i] < vetorDeRetorno[0]){ /* Comparação para encontrar o MENOR */
            vetorDeRetorno[0] = vetorDeNumerosReais[i];
        }
        if(vetorDeNumerosReais[i] > vetorDeRetorno[1]){ /* Comparação para encontrar o MAIOR */
            vetorDeRetorno[1] = vetorDeNumerosReais[i];
        }
    }
    GET_TIME(fim);
    deltaSequencial = fim - inicio;

    return;
}


/*
* Aqui a função conpara elemento por elemento e verifica o menor e o maior usando força bruta.
* Aqui é usado a abordagem concorrente.
* O vetor 'vetorDeNumerosReais' guarda o menor e o maior valores na posição 2 e 3, respectivamente.
*/
void buscaMenorMaior_Concorrente(){
    /* Variáveis locais de 'buscaMenorMaior_Concorrente' */
    pthread_t *tid; /* Identificadores que o sistema usa para as threads  */
    ArgumentosDaThreads *argumentos; /* Contém os identificadores que o software usa para as threads */
    ArgumentosDaThreads *retornoLocalDaThread;

    /* Alocação de memória dos ponteiros usados localmente */
    tid = (pthread_t *)malloc(sizeof(pthread_t) * numeroDeThreads);
    if(tid == NULL){
        printf("ERRO: Memória insuficiente. Tente um número menor de threads.\n");
        exit(2);
    }
    argumentos = (ArgumentosDaThreads *)malloc(sizeof(ArgumentosDaThreads) * numeroDeThreads);
    if(argumentos == NULL){
        printf("ERRO: Memória insuficiente. Tente um número menor de threads.\n");
        exit(2);
    }

    GET_TIME(inicio);
    /* Criação de cada thread */
    for(int i = 0; i < numeroDeThreads; i++){
        (argumentos + i)->id = i; /* Define o ID específico de cada thread para o software. */
        (argumentos + i)->menor = vetorDeNumerosReais[0];
        (argumentos + i)->maior = vetorDeNumerosReais[0];
        if(pthread_create((tid + i) ,NULL, tarefa, (void *)(argumentos + i))){
            printf("ERRO: Falha ao criar a thread %d\n", i);
            exit(3);
        }
    }

    /* Espera o término de todas as threads para prosseguir o fluxo principal. */
    for(int i = 0; i < numeroDeThreads; i++){
        if(pthread_join(*(tid + i), (void **)&retornoLocalDaThread)){
            printf("ERRO: Falha ao terminar a thread %d\n", i);
            exit(3);
        }
        if(retornoLocalDaThread->menor < vetorDeRetorno[2]) /* Comparação para encontrar o MENOR */
            vetorDeRetorno[2] = retornoLocalDaThread->menor;
        if(retornoLocalDaThread->maior > vetorDeRetorno[3]) /* Comparação para encontrar o MAIOR */
            vetorDeRetorno[3] = retornoLocalDaThread->maior;
    }
    GET_TIME(fim);
    deltaConcorrente = fim - inicio;

    /* Liberação de memória usada pelo programa */
    free(tid);
    free(argumentos);
    return;
}


/* Cada thread fica responsável por um bloco do vetor e deve retornar o maior e menor daquele bloco. */
void *tarefa(void *arg){
    ArgumentosDaThreads *argumentos = (ArgumentosDaThreads *)arg;
    int id = argumentos->id;
    long long int tamanhoDoBloco = numeroDeElementos / numeroDeThreads;
    long long int inicioDoBloco = id * tamanhoDoBloco;
    long long int fimDoBloco;

    if(id == numeroDeThreads - 1) fimDoBloco = numeroDeElementos;
    else fimDoBloco = inicioDoBloco + tamanhoDoBloco;

    for(int i = inicioDoBloco; i < fimDoBloco; i++){
        if(vetorDeNumerosReais[i] < argumentos->menor){
            argumentos->menor = vetorDeNumerosReais[i];
        }
        if(vetorDeNumerosReais[i] > argumentos->maior){
            argumentos->maior = vetorDeNumerosReais[i];
        }
    }

    pthread_exit((void *)argumentos);
}

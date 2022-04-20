#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void EncherVetor(int *valor){
    for(int i = 0; i < 10000; i++){
        valor[i] = 2;
    }
}

void MostrarResultadoProcessado(int *vetor){
    int i = 0;
    for(i = 0; i < 10000; i++){
        if(vetor[i] == 4){
            continue;
        }
        else{
            break;
        }
    }

    if(i == 10000){
        printf("Primeira e Segunda metades Calculada com Sucesso!\n");
    }
}

void *ElevarAoQuadradoPrimeiraMetade(void *valor){
    int *vetor = (int *)valor;

    for(int i = 0; i < 5000; i++){
        vetor[i] = vetor[i] * vetor[i];
    }
    pthread_exit(NULL);
}

void *ElevarAoQuadradoSegundaMetade(void *valor){
    int *vetor = (int *)valor;

    for(int i = 5000; i < 10000; i++){
        vetor[i] = vetor[i] * vetor[i];
    }
    pthread_exit(NULL);
}






int main(void) {
    int vetor[10000];
    pthread_t identificadorPrimeiraMetade, identificadorSegundaMetade;

    EncherVetor(vetor);

    if(pthread_create(&identificadorPrimeiraMetade, NULL, ElevarAoQuadradoPrimeiraMetade, (void *) &vetor)){
        printf("--- ERRO no 'pthread_create' ---\n");
    }
    if(pthread_create(&identificadorSegundaMetade, NULL, ElevarAoQuadradoSegundaMetade, (void *) &vetor)){
        printf("--- ERRO no 'pthread_create' ---\n");
    }

    
    if(pthread_join(identificadorSegundaMetade, NULL)){
        printf("--- ERRO no 'pthread_join' ---\n");
    }
    if(pthread_join(identificadorPrimeiraMetade, NULL)){
        printf("--- ERRO no 'pthread_join' ---\n");
    }
    MostrarResultadoProcessado(vetor);


    printf("Teste do término fluxo principal\n");
    pthread_exit(NULL);
    return 0;
}


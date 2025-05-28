#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_VERSOES 3
#define NUM_CANAIS 6

int canal[NUM_CANAIS] = {-1, -1, -1, -1, -1, -1};

void send_async(int *buf, int c) {
    canal[c] = *buf;
    return;
}

void receive(int *buf, int c) {
    while (canal[c] == -1);
    *buf = canal[c];
    canal[c] = -1;
    return;
}

int compara(int vetor[], int *versao_erro) {
   
    if((vetor[0]==vetor[1]) && (vetor[1] != vetor[2])){
      *versao_erro = 2;
      return vetor[0];
    }
   
    else if((vetor[0]==vetor[2]) && (vetor[0] != vetor[1])){
      *versao_erro = 1;
      return vetor[0];
    }
   
    else if((vetor[1]==vetor[2]) && (vetor[1] != vetor[0])){
      *versao_erro = 0;
      return vetor[1];
    }
   
    else {
      *versao_erro = -1;
      return 10;
    }
}

void *versao_a(void *arg) {
    int voto = 10;
    int status;

    send_async(&voto, 0);
    receive(&status, 3);

    if (status == 0)
        printf("Versão A correta\n");
    else
        printf("Versão A errada\n");

    return NULL;
}

void *versao_b(void *arg) {
    int voto = 5;
    int status;

    send_async(&voto, 1);
    receive(&status, 4);

    if (status == 0)
        printf("Versão B correta\n");
    else
        printf("Versão B errada\n");

    return NULL;
}

void *versao_c(void *arg) {
    int voto = 10;
    int status;

    send_async(&voto, 2);
    receive(&status, 5);

    if (status == 0)
        printf("Versão C correta\n");
    else
        printf("Versão C errada\n");

    return NULL;
}

void *driver(void *arg) {
    int votos[NUM_VERSOES];
    int versao_erro;

    for (int i = 0; i < NUM_VERSOES; i++) {
        receive(&votos[i], i);
    }

    int voto_majoritario = compara(votos, &versao_erro);
    printf("Voto majoritário: %d\n", voto_majoritario);

    if (versao_erro != -1)
        printf("Versão %c votou errado\n", 'A' + versao_erro);
    else
        printf("Nenhuma versão votou errado\n");

    for (int i = 0; i < NUM_VERSOES; i++) {
        int status = (i == versao_erro) ? 1 : 0;
        send_async(&status, i + 3);
    }

    return NULL;
}

int main() {
    pthread_t t_a, t_b, t_c, t_driver;

    pthread_create(&t_a, NULL, versao_a, NULL);
    pthread_create(&t_b, NULL, versao_b, NULL);
    pthread_create(&t_c, NULL, versao_c, NULL);
    pthread_create(&t_driver, NULL, driver, NULL);

    pthread_join(t_a, NULL);
    pthread_join(t_b, NULL);
    pthread_join(t_c, NULL);
    pthread_join(t_driver, NULL);

    return 0;
}


int compara(int votos[], int *versao_erro) {
    if (votos[0] == votos[1]) {
        if (votos[0] != votos[2]) *versao_erro = 2;
        else *versao_erro = -1;
        return votos[0];
    } else if (votos[0] == votos[2]) {
        *versao_erro = 1;
        return votos[0];
    } else if (votos[1] == votos[2]) {
        *versao_erro = 0;
        return votos[1];
    } else {
        *versao_erro = -1;
        return -1;
    }
}

  int compara(int vetor[], int *versao_erro){
   
    if((vetor[0]==vetor[1]) && (vetor[1] != vetor[2])){
      *versao_erro = 2;
      return vetor[0];
    }
   
    else if((vetor[0]==vetor[2]) && (vetor[0] != vetor[1])){
      *versao_erro = 1;
      return vetor[0];
    }
   
    else if((vetor[1]==vetor[2]) && (vetor[1] != vetor[0])){
      *versao_erro = 0;
      return vetor[1];
    }
   
    else {
      return vetor[0];
    }
  }
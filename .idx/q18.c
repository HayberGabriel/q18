#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

#define TAMANHO_MENSAGEM 100

char channel[TAMANHO_MENSAGEM];

sem_t sem_espaco_livre;
sem_t sem_item_disponivel;
void enviar_sincrono(const char* dado_para_enviar) {
    if (sem_wait(&sem_espaco_livre) != 0) {
        perror("Erro em sem_wait(sem_espaco_livre)");
        pthread_exit(NULL);
    }

    strncpy(channel, dado_para_enviar, TAMANHO_MENSAGEM -1);
    channel[TAMANHO_MENSAGEM - 1] = '\0';
    printf("Remetente: Dado '%s' enviado para o canal.\n", dado_para_enviar);
    fflush(stdout);
    if (sem_post(&sem_item_disponivel) != 0) {
        perror("Erro em sem_post(sem_item_disponivel)");
        pthread_exit(NULL);
    }
}

char* receber_sincrono() {
    if (sem_wait(&sem_item_disponivel) != 0) {
        perror("Erro em sem_wait(sem_item_disponivel)");
        pthread_exit(NULL);
    }

    char* dado_recebido = (char*)malloc(TAMANHO_MENSAGEM * sizeof(char));
    if (dado_recebido == NULL) {
        perror("Erro ao alocar memória para dado_recebido");
        sem_post(&sem_espaco_livre);
        pthread_exit(NULL);
    }
    strncpy(dado_recebido, channel, TAMANHO_MENSAGEM -1);
    dado_recebido[TAMANHO_MENSAGEM - 1] = '\0';
    printf("Destinatário: Dado '%s' recebido do canal.\n", dado_recebido);
    fflush(stdout);
    if (sem_post(&sem_espaco_livre) != 0) {
        perror("Erro em sem_post(sem_espaco_livre)");
        free(dado_recebido);
        pthread_exit(NULL);
    }
    return dado_recebido;
}

void* thread_remetente(void* arg) {
    const char* mensagens[] = {"Olá Mundo!", "Teste de Sincronização", "Terceira Mensagem", "Fim"};
    int num_mensagens = sizeof(mensagens) / sizeof(mensagens[0]);

    for (int i = 0; i < num_mensagens; i++) {
        printf("Remetente: Tentando enviar '%s'...\n", mensagens[i]);
        fflush(stdout);
        enviar_sincrono(mensagens[i]);
        sleep(1);
    }
    return NULL;
}

void* thread_destinatario(void* arg) {
    char* msg_recebida;    int mensagens_esperadas = 4;

    for (int i = 0; i < mensagens_esperadas; i++) {
        printf("Destinatário: Esperando para receber...\n");
        fflush(stdout);
        msg_recebida = receber_sincrono();
        if (msg_recebida != NULL) {
            printf("Destinatário: Processando '%s'.\n", msg_recebida);
            fflush(stdout);            sleep(2);
            free(msg_recebida);
        }
    }
    return NULL;
}

int main() {
    pthread_t id_remetente, id_destinatario;

    if (sem_init(&sem_espaco_livre, 0, 1) != 0) {
        perror("Falha ao inicializar sem_espaco_livre");
        return 1;
    }
    if (sem_init(&sem_item_disponivel, 0, 0) != 0) {
        perror("Falha ao inicializar sem_item_disponivel");
        sem_destroy(&sem_espaco_livre);
        return 1;
    }

    if (pthread_create(&id_destinatario, NULL, thread_destinatario, NULL) != 0) {
        perror("Falha ao criar thread destinatário");
        sem_destroy(&sem_espaco_livre);
        sem_destroy(&sem_item_disponivel);
        return 1;
    }

    if (pthread_create(&id_remetente, NULL, thread_remetente, NULL) != 0) {
        perror("Falha ao criar thread remetente");
        sem_destroy(&sem_espaco_livre);
        sem_destroy(&sem_item_disponivel);
        return 1;
    }

    if (pthread_join(id_remetente, NULL) != 0) {    printf("Iniciando threads remetente e destinatário...\n");
    fflush(stdout);
        perror("Falha ao juntar thread remetente");
    }
    printf("Thread remetente finalizada.\n");
    fflush(stdout);

    if (pthread_join(id_destinatario, NULL) != 0) {    pthread_cancel(id_destinatario);
        perror("Falha ao juntar thread destinatário");
    }
    printf("Thread destinatário finalizada.\n");
    fflush(stdout);

    printf("Programa finalizado com sucesso.\n");
    fflush(stdout);

    return 0;
}
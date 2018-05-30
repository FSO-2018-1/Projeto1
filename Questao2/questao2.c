#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <semaphore.h>
 #include <unistd.h>


void *runner_alunos(void *param);
void *runner_monitor(void *param);

#define PROGRAMANDO 0
#define AJUDA 1
#define DORMINDO 2
#define SENTADO 3
#define FINALIZADO 4
#define OCUPADA 1
#define VAZIA 0
#define NAOEXISTE -1

int cadeiras[20], qtdCadeiras;
int estadoMonitor = DORMINDO;

sem_t sem;

int arredondamento(double num){
    int num_int = num;
    if( (num - num_int) >= 0.5) return (num_int +1);
    else return num_int;
}

int main(int argc, char *argv[]){
  int i, j, alunos;
  srand(time(NULL));

  alunos = (rand() % 37 + 3);
  qtdCadeiras = arredondamento(alunos/2.0);


  for(i = 0; i < 20; i++){
    cadeiras[i] = NAOEXISTE;
  }

  for(i = 0; i < qtdCadeiras; i++){
    cadeiras[i] = VAZIA;
  }


  pthread_t tid_alunos[alunos];
  pthread_attr_t attr_alunos[alunos];
  pthread_t tid_monitor;
  pthread_attr_t attr_monitor;

  sem_init(&sem, 0, 1);

  //Inicializa a thread do assistente
  pthread_attr_init(&attr_monitor);
  pthread_create(&tid_monitor, &attr_monitor, runner_monitor, NULL);

  //Inicializa as threads de alunos
  for(i=0; i<alunos;i++){
    pthread_attr_init(&attr_alunos[i]);
    pthread_create(&tid_alunos[i], &attr_alunos[i], runner_alunos, NULL);
  }

  //Espera a finalizacao das threads
  for(i=0; i<alunos;i++){
    pthread_join(tid_alunos[i], NULL);
  }

  return 0;
}

void *runner_alunos(void *param){
  int estadoAluno = PROGRAMANDO;
  int nAjuda, posicaoFila = 0;

  while(estadoAluno != FINALIZADO){
    estadoAluno = PROGRAMANDO;
    sleep(rand() % 11);

    if(estadoAluno == PROGRAMANDO){
      estadoAluno = rand() % 2;
    }

    if(estadoAluno == AJUDA){
      sem_wait(&sem);
      if(estadoMonitor == DORMINDO){
        estadoMonitor = AJUDA;
        nAjuda = nAjuda + 1;
        sem_post(&sem);
      } else if(estadoMonitor == AJUDA){
        if(cadeiras[posicaoFila] == VAZIA){
          cadeiras[posicaoFila] = OCUPADA;
          posicaoFila = posicaoFila + 1;
          if(posicaoFila > qtdCadeiras){
            posicaoFila = 0;
          }
          estadoAluno = SENTADO;
        } else{
          estadoAluno = PROGRAMANDO;
        }
      }
    }
    if(nAjuda == 3){
      estadoAluno = FINALIZADO;
    }
  }

}

void *runner_monitor(void *param){



}

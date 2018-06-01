#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>


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

int cadeiras[20], qtdCadeiras, Nalunos = 1, posicaoFila = 0;
int estadoMonitor = DORMINDO;

sem_t sem, sem_alunos;

int tempo(){
  int temp;
  time_t t = time(NULL); // Função time que retorna o tempo exato do computador
  struct tm tm = *localtime(&t); // *localtime == tempo local

  temp = (tm.tm_hour*3600) + (tm.tm_min*60) + (tm.tm_sec); // Tempo total retornado em segundos

  return temp;
}

int arredondamento(double num){
    int num_int = num;
    if( (num - num_int) >= 0.5) return (num_int +1);
    else return num_int;
}

int main(int argc, char *argv[]){
  int i, j, alunos;
  srand(time(NULL));

  alunos = 3;
  // alunos = (rand() % 37 + 3);
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
  sem_init(&sem_alunos, 0, 1);

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
  int numeroAluno, estadoAluno = PROGRAMANDO, i, horario;
  int nAjuda, seg;

  sem_wait(&sem_alunos);
    numeroAluno = Nalunos;
    Nalunos += 1;
  sem_post(&sem_alunos);

  printf("Aluno %d programando!\n", numeroAluno);

  while(estadoAluno != FINALIZADO){
    estadoAluno = PROGRAMANDO;
    seg = rand() % 3;
    sleep(seg);

    if(estadoAluno == PROGRAMANDO){
      estadoAluno = rand() % 2;
      printf("Estado do aluno %d e: %d !\n", numeroAluno, estadoAluno);
    }

    if(estadoAluno == AJUDA){
      sem_wait(&sem);
      if(estadoMonitor == DORMINDO && cadeiras[0] == VAZIA){
        estadoMonitor = AJUDA;
        nAjuda = nAjuda + 1;
        sem_post(&sem);
        printf("Monitor está atendendo o aluno %d!\n", numeroAluno);
        estadoAluno = PROGRAMANDO;
        horario = tempo();
        // printf("Monitor esta livre!\n");
      } else if(estadoMonitor == AJUDA){
        if(cadeiras[posicaoFila] == VAZIA && tempo() - horario < 10){
          cadeiras[posicaoFila] = OCUPADA;
          posicaoFila = posicaoFila + 1;
          printf("Aluno %d está sentado na posicao %d!\n", numeroAluno, posicaoFila);
          estadoAluno = SENTADO;
          if(qtdCadeiras <= posicaoFila ){
            posicaoFila = 0;
          }
          sem_post(&sem);
        } else if(cadeiras[posicaoFila] == OCUPADA && tempo() - horario < 10){
          printf("Aluno %d voltou a programar!\n", numeroAluno);
          estadoAluno = PROGRAMANDO;
          sem_post(&sem);
        }
        else if(cadeiras[posicaoFila] == OCUPADA &&  tempo() - horario > 10){
          for(i = 0; i < qtdCadeiras; i++){
            if(cadeiras[0] == VAZIA){
              cadeiras[0] = VAZIA;
              printf("Cadeira %d esta liberada!\n", i);
            } else if(cadeiras[i] == VAZIA){
              cadeiras[i-1] = VAZIA;
              printf("Cadeira %d esta liberada!\n", i-1);
              estadoAluno = AJUDA;
              horario = tempo();
            } else if(cadeiras[qtdCadeiras] == OCUPADA){
              cadeiras[qtdCadeiras] = VAZIA;
              printf("Cadeira %d esta liberada!\n", qtdCadeiras);
              estadoAluno = AJUDA;
              horario = tempo();
            }
          }
          sem_post(&sem);
        }
        else if(cadeiras[posicaoFila] == VAZIA &&  tempo() - horario > 10){
          sem_post(&sem);
          estadoMonitor = DORMINDO;
          printf("Monitor esta livre!\n");
        }
      }
    }

    if(nAjuda == 3){
      estadoAluno = FINALIZADO;
    }
  }

  printf("Aluno %d foi atendido 3 vezes!\n", numeroAluno);

}

void *runner_monitor(void *param){
  int i, seg;

}

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

void *runner_alunos(void *param);
void *runner_monitor(void *param);

#define PROGRAMANDO 0
#define AJUDA 1
#define DORMINDO 2
#define FINALIZADO 3
#define OCUPADA 1
#define VAZIA 0
#define NAOEXISTE -1

int cadeiras[20], qtdCadeiras, nAlunos = 1;
int estadoMonitor = DORMINDO;
int alunosEspera = 0;
int proxCadeira = 0, proxAjuda = 0;
int alunosProgramando = 0;
int nAjudas[40];

sem_t sem_geral, sem_alunos, sem_monitor, sem_ajuda ;

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

  for(i = 0; i < 40; i++){
    nAjudas[i] = NAOEXISTE;
  }

  for(i = 0; i < alunos; i++){
    nAjudas[i] = 0;
  }

  pthread_t tid_alunos[alunos];
  pthread_attr_t attr_alunos[alunos];
  pthread_t tid_monitor;
  pthread_attr_t attr_monitor;

  sem_init(&sem_geral, 0, 1);
  sem_init(&sem_alunos, 0, 0);
  sem_init(&sem_monitor, 0, 1);
  sem_init(&sem_ajuda, 0, 1);

  //Inicializa a thread do assistente
  pthread_attr_init(&attr_monitor);
  pthread_create(&tid_monitor, &attr_monitor, runner_monitor, NULL);

  //Inicializa as threads de alunos
  for(i=0; i<alunos;i++){
    alunosProgramando = alunos;
    pthread_attr_init(&attr_alunos[i]);
    pthread_create(&tid_alunos[i], &attr_alunos[i], runner_alunos, NULL);
  }


  //Espera a finalizacao das threads

  for(i=0; i<alunos;i++){
    pthread_join(tid_alunos[i], NULL);
  }

  pthread_join(tid_monitor, NULL);

  return 0;
}

void *runner_alunos(void *param){
  int estadoAluno = PROGRAMANDO;
  int idAluno;

  sem_wait(&sem_geral);
    idAluno = nAlunos;
    nAlunos += 1;
  sem_post(&sem_geral);

  printf("Aluno %d iniciou sua programação!\n", idAluno);

  //seg = rand() % 3;

  while (estadoAluno != FINALIZADO){

    sleep(1);
    estadoAluno = rand() % 2;
    // printf("Estado do aluno %d e: %d !\n", idAluno, estadoAluno);


    if (nAjudas[idAluno-1] == 3) {
      estadoAluno = FINALIZADO;
    } else{
      sem_wait(&sem_ajuda);
      if (estadoAluno == AJUDA) {
        if (alunosEspera < qtdCadeiras) {
          cadeiras[proxCadeira] = idAluno;
          alunosEspera++;

          printf("Aluno %d pediu ajuda e está esperando!\n", idAluno);
          //Situacao das cadeiras
          sem_wait(&sem_geral);
          printf("-----------------------------------------------------------\n");
          printf("Situação das cadeiras: ");
          for(int i = 0;i<qtdCadeiras; i++){
            if(cadeiras[i] == VAZIA){
              printf("[%d]-Vazia ", i+1 );
            }else{
              printf("[%d]-Aluno %d ", i+1, cadeiras[i] );
            }
          }
          printf("\n");
          printf("-----------------------------------------------------------\n");
          sem_post(&sem_geral);

          proxCadeira = (proxCadeira+1) % qtdCadeiras;
          sem_post(&sem_ajuda);

          //Acordar o Monitor
          sem_post(&sem_alunos);
          sem_wait(&sem_monitor);
        }else{
          sem_post(&sem_ajuda);
          printf("Aluno %d pediu ajuda mas não há cadeiras sobrando, voltou a programar!\n", idAluno);
        }
      } else{
        sem_post(&sem_ajuda);
        printf("Aluno %d está programando!\n", idAluno);
      }
    }
  }
  printf("O aluno %d finalizou seu programa!\n", idAluno);
  alunosProgramando--;
}

void *runner_monitor(void *param){

  while (alunosProgramando > 0) {
    sem_wait(&sem_alunos);

    sem_wait(&sem_ajuda);

    printf("Monitor ajudando o aluno %d!\n", cadeiras[proxAjuda]);
    sem_wait(&sem_geral);
    nAjudas[cadeiras[proxAjuda]-1]++;
    sem_post(&sem_geral);
    printf("Numero de ajudas do aluno %d: %d\n", cadeiras[proxAjuda], nAjudas[cadeiras[proxAjuda]-1]);

    cadeiras[proxAjuda] = VAZIA;
    alunosEspera--;

    sem_wait(&sem_geral);
    printf("-----------------------------------------------------------\n");
    printf("Situação das cadeiras: ");
    for(int i = 0;i<qtdCadeiras; i++){
      if(cadeiras[i] == VAZIA){
        printf("[%d]-Vazia ", i+1 );
      }else{
        printf("[%d]-Aluno %d ", i+1, cadeiras[i]);
      }
    }
    printf("\n");
    printf("-----------------------------------------------------------\n");

    sem_post(&sem_geral);
    proxAjuda = (proxAjuda+1) % qtdCadeiras;

    sleep(1);
    if(alunosEspera == 0){
      printf("Monitor terminou de ajudar e voltou a dormir!\n");
      estadoMonitor = DORMINDO;
    }else{
      printf("Monitor terminou de ajudar e vai ajudar o próximo aluno!\n");
    }

    sem_post(&sem_ajuda);

    sem_post(&sem_monitor);

  }

  printf("Monitor encerrou suas atividades!\n");

}

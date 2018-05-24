#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>

#define LINHAS 9
#define COLUNAS 9
#define VALIDO 1
#define INVALIDO 0

int matrix[LINHAS][COLUNAS];
int val[11];
void *runner_linha(void *param);
void *runner_coluna(void *param);

int main(int argc, char *argv[]){
  int i, j;
  FILE *file;
  file=fopen("../sudokus/teste.txt", "r");

  pthread_t tid_linha;
  pthread_t tid_coluna;
  pthread_t tid_grid[9];
  pthread_attr_t attr_linha;
  pthread_attr_t attr_coluna;
  pthread_attr_t attr_grid[9];

  // Coloca todas as posicoes do vetor de validacao como VALIDO(1)
  for(i = 0; i < 11; i++){
    val[i] = VALIDO;
  }

  // Le o arquivo e coloca o sudoku na matrix
  for(i = 0; i < LINHAS; i++){
    for(j = 0; j < COLUNAS; j++){
      if (!fscanf(file, "%d", &matrix[i][j]))
        break;
    }
  }

  //Inicializa a thread de analise de linhas
  pthread_attr_init(&attr_linha);
  pthread_create(&tid_linha, &attr_linha, runner_linha, NULL);

  //Inicializa a thread de analise de colunas
  pthread_attr_init(&attr_coluna);
  pthread_create(&tid_coluna, &attr_coluna, runner_coluna, NULL);

  /*
  for(i=0; i<9;i++){
    pthread_attr_init(&attr_grid[i]);
    pthread_create(&tid_grid[i], &attr_grid[i], runner_grid, NULL);
  }
  */

  pthread_join(tid_linha, NULL);
  pthread_join(tid_coluna, NULL);

  // Printa o vetor de validacao
  printf("\n\n\n\n");
  for(i = 0; i < 9; i++){
    printf("%d ", val[i]);
  }
  return 0;
}

void *runner_linha(void *param){
  int analise[LINHAS][COLUNAS], i, j;

  // Coloca todas as posicoes do vetor de analise como INVALIDO(0)
  for (i = 0; i < LINHAS; i++){
    for(j = 0;j < COLUNAS; j++){
      analise[i][j] = INVALIDO;
    }
  }

  /* Confere a quantidade de vezes que cada numero de 0 a 9
     aparece na linha */
  for (i = 0; i < LINHAS; i++){
    for(j = 0;j < COLUNAS; j++){
      analise[i][matrix[i][j]-1] += 1;
    }
  }

  /* Se algum numero tiver quantidade diferente de 1
     coloca INVALIDO(0) na posicao 0(linhas) vetor de validacao */
  for (i = 0; i < LINHAS; i++){
    for(j = 0;j < COLUNAS; j++){
      if(analise[i][j] != 1)
      val[0]=INVALIDO;
    }
  }
}

void *runner_coluna(void *param){
  int analise[LINHAS][COLUNAS], i, j;

  // Coloca todas as posicoes do vetor de analise como INVALIDO(0)
  for (i = 0; i < LINHAS; i++){
    for(j = 0;j < COLUNAS; j++){
      analise[i][j] = INVALIDO;
    }
  }

  /* Confere a quantidade de vezes que cada numero de 0 a 9
     aparece na coluna */
  for (i = 0; i < LINHAS; i++){
    for(j = 0;j < COLUNAS; j++){
      analise[matrix[j][i]-1][i] += 1;
    }
  }

  /* Se algum numero tiver quantidade diferente de 1
     coloca INVALIDO(0) na posicao 1(colunas) vetor de validacao */
  for (i = 0; i < LINHAS; i++){
    for(j = 0;j < COLUNAS; j++){
      if(analise[i][j] != 1)
      val[1]=INVALIDO;
    }
  }
}

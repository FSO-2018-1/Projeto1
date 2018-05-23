#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>

int matrix[9][9];
int val[9];
void *runner_linha(void *param);
void *runner_coluna(void *param);

int main(int argc, char *argv[]){
  int i, j;
  FILE *file;
  file=fopen("./sudokus/teste.txt", "r");

  pthread_t tid_linha;
  pthread_t tid_coluna;
  pthread_t tid_grid[9];
  pthread_attr_t attr_linha;
  pthread_attr_t attr_coluna;
  pthread_attr_t attr_grid[9];

  for(i = 0; i < 9; i++){
    val[i] = 1;
  }

 for(i = 0; i < 9; i++){
  for(j = 0; j < 9; j++){
    if (!fscanf(file, "%d", &matrix[i][j]))
       break;
    }
  }

  pthread_attr_init(&attr_linha);
  pthread_create(&tid_linha, &attr_linha, runner_linha, NULL);

  pthread_attr_init(&attr_coluna);
  pthread_create(&tid_coluna, &attr_coluna, runner_coluna, NULL);
  //
  // for(i=0; i<9;i++){
  //   pthread_attr_init(&attr_grid[i]);
  //   pthread_create(&tid_grid[i], &attr_grid[i], runner_grid, NULL);
  // }

  pthread_join(tid_linha, NULL);
  pthread_join(tid_coluna, NULL);

  printf("\n\n\n\n");
  for(i = 0; i < 9; i++){
    printf("%d ", val[i]);
  }
  return 0;
}

void *runner_linha(void *param){
  int analise[9][9], i, j;

  for (i = 0; i < 9; i++){
    for(j = 0;j < 9; j++){
      analise[i][j] = 0;
    }
  }

  for (i = 0; i < 9; i++){
    for(j = 0;j < 9; j++){
      analise[i][matrix[i][j]-1] += 1;
    }
  }

  for (i = 0; i < 9; i++){
    for(j = 0;j < 9; j++){
      if(analise[i][j] != 1)
      val[0]=0;
    }
  }
}

void *runner_coluna(void *param){
  int analise[9][9], i, j;

  for (i = 0; i < 9; i++){
    for(j = 0;j < 9; j++){
      analise[i][j] = 0;
    }
  }

  for (i = 0; i < 9; i++){
    for(j = 0;j < 9; j++){
      analise[matrix[j][i]-1][i] += 1;
    }
  }

  for (i = 0; i < 9; i++){
    for(j = 0;j < 9; j++){
      if(analise[i][j] != 1)
      val[1]=0;
      printf("%d ", analise[i][j]);
    }
    printf("\n");
  }
}

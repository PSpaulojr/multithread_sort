
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


#define WORKERS 4  
#define NUMEROS  100000

pthread_mutex_t trava ;

/* Definicao do problema */

int qntd_numeros = 0;
int global = 0;

int completos[5] = {0, 0, 0, 0, 0};  /* Instancias ja solucionadas*/
int vetor_numeros[NUMEROS];

void merge(int l, int m, int r){

    int i, j, k;


    int n1 = m - l + 1;
    int n2 = r - m;

    int L[n1], R[n2];


    for (i = 0; i < n1; i++){
        L[i] = vetor_numeros[l+i];
    }
    for(j = 0; j < n2; j++){
        R[j] = vetor_numeros[m+1+j];
    }

    i = 0;
    j = 0; 
    k = l;

    while(i < n1 && j < n2){
        if (L[i] <= R[j]){
            vetor_numeros[k] = L[i];
            i++;

        }
        else {
            vetor_numeros[k] = R[j];
            j++;

        }
        k++;
    }

    while(i < n1){
        vetor_numeros[k] = L[i];
        i++;
        k++;
    }

    while(j < n2){
        vetor_numeros[k] = R[j];
        j++;
        k++;
    }

    return;
}

void mergeSort( int l, int r){
    
    if(l < r){
        int m = l+ ((r-l)/2);

        mergeSort(l, m);
        mergeSort(m+1, r);

        
        merge(l, m, r);
    }

    return; 
}


void printvetor_numeros(int size){
    int i;
    for(i = 0; i < size; i++){
        printf("%d", vetor_numeros[i]);
        if(i == size-1 ) {
          break;
        }
        printf(" ");
    }
    printf("\n");
}

void print_parcial(int l, int r){
  for(int i = l; i < r+1; i++){
    printf("%d ", vetor_numeros[i]);
  }
  printf("\n");
}


void* worker(void *arg) {
  /* Inicializacao: guardar o proprio numero da thread */
  int *N = (int*)(arg);
  int M = (*N);

  int t_parte = 0;
  int l, m ,r;
  int total = qntd_numeros;



  int instancia = 0; /* A instancia do problema com a qual vou trabalhar */

  while (1) {
    /* Busca por uma instancia do problema que ainda nao foi resolvida */
    pthread_mutex_lock(&trava);

    while ( (completos[instancia]!=0) ) {
      instancia++;
    }
    if (instancia > 4) { /* Acabaram as instancias (problema inteiro
                                    foi finalizado) */
      /* Antes de sair do loop, libero a trava! */
      pthread_mutex_unlock(&trava);
      break;
    }
    completos[instancia] = 1; /* Proponho-me a resolver a
                                 instancia que encontrei */
    pthread_mutex_unlock(&trava);

    pthread_mutex_lock(&trava);
    t_parte = global;
    global ++;
    pthread_mutex_unlock(&trava);

    if( instancia == 4 ){
        l = 3 * (total/4);
        r = total - 1;
    } 
    else{

        if(qntd_numeros < 16){
          l = t_parte * (total/2);
          r = (t_parte + 1) * (total/2) - 1;
        }
        else{
            if(r < qntd_numeros){
            l = t_parte * (total/4);
            r = (t_parte + 1) * (total/4) - 1;

          }
          else {
            break;
          }
        } 

    }

    if( (l < r) && (r < (qntd_numeros)) ){
        
        m = l + ((r-l)/2);
        
        mergeSort(l, m);
    
        mergeSort(m+1, r);

        merge(l, m, r);
    }

    else if(r == l){
      return NULL;
    }
    
  }

  return NULL;
}


int main(int argc, char **argv) {
  pthread_t workers[WORKERS];
  int thread_id[WORKERS];

  /* Identificadores de thread */
  for (int i = 0; i < WORKERS; i++) {
    thread_id[i] = i;
  }

  while(scanf("%d ", &vetor_numeros[qntd_numeros]) != EOF ){  //Conto quantos numeros que precisaram ser analisados//
    qntd_numeros++;
  }


  if(qntd_numeros % 4 == 0){
      completos[4] = 1;
  }
  if (qntd_numeros < 16){
    completos[4] = 1;
    completos[3] = 1;
  }

  /* Disparando threads */
  for (int i=0; i < WORKERS; i++) {
    pthread_create(&(workers[i]), NULL, worker, (void*) (&thread_id[i]));
  }

  /* Esperando threads */
  for (int i=0; i < WORKERS; i++) {
    pthread_join(workers[i], NULL);
  }

  /* Escrevendo vetor */
  merge(0, ( (qntd_numeros/2) - 1 ) / 2 , (qntd_numeros / 2) -1 ) ;

  merge(qntd_numeros/2, ( ((qntd_numeros/2)-1) + ( (qntd_numeros - ( qntd_numeros/2 )) / 2 ) ), (qntd_numeros)-1 );


  merge(0, ((qntd_numeros / 2) -1 ), qntd_numeros - 1);

  printvetor_numeros(qntd_numeros);

  return 0;
}

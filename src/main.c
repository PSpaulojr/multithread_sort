
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


#define WORKERS 4  
#define NUMEROS  100000

pthread_mutex_t trava ;
pthread_mutex_t trava2 ;

/* Definicao do problema */
//int NUMEROS = 10100;
int qntd_numeros = 0;
int global = 0;

int completos[5] = {0, 0, 0, 0, 0};  /* Instancias ja solucionadas*/
int vetor_numeros[NUMEROS];

void merge(int l, int m, int r){
    ////printf("\nMerge:\n");

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
  int primeiro = 0;
  int l, m ,r;
  int total = qntd_numeros;


  //printf("Iniciando thread %d\n", M);

  int instancia = 0; /* A instancia do problema com a qual vou trabalhar */

  while (1) {
    /* Busca por uma instancia do problema que ainda nao foi resolvida */
    pthread_mutex_lock(&trava);

    while ( (completos[instancia]!=0) ) {
      //printf("Thread %d testou instancia %d\n", M, instancia);
      instancia++;
    }
    //printf("Thread %d tentando tomar instancia %d\n", M, instancia);
    if (instancia > 4) { /* Acabaram as instancias (problema inteiro
                                    foi finalizado) */
      /* Antes de sair do loop, libero a trava! */
      pthread_mutex_unlock(&trava);
      break;
    }
    completos[instancia] = 1; /* Proponho-me a resolver a
                                 instancia que encontrei */
    pthread_mutex_unlock(&trava);

    //printf("Instancia %d acessada por thread %d\n", instancia, M);
    
    pthread_mutex_lock(&trava);
    t_parte = global;
    global ++;
    //printf("Global: %d, t_parte: %d\n", global, t_parte);
    pthread_mutex_unlock(&trava);

    if( instancia == 4 ){
        l = 3 * (total/4);
        //printf("\nl thread''%d: %d\n", M, l);
        r = total - 1;
        //printf("\nr thread''%d: %d\n", M, r);
    } 
    else{
      /* if(primeiro){
        break;
        
         l = (t_parte+global)*(total/4);
        //printf("\nl thread (pri)%d: %d\n", t_parte, l);
        r = (t_parte+global+1) * (total/4) - 1  ;
        //printf("\nr thread (pri)%d: %d\n", t_parte, r); 
    }  */
        if(qntd_numeros < 16){
          l = t_parte * (total/2);
          //printf("\nl thread %d: %d\n", M, l);
          r = (t_parte + 1) * (total/2) - 1;
          //printf("\nr thread %d: %d\n", M, r);
        }
        else{
            if(r < qntd_numeros){
            l = t_parte * (total/4);
            //printf("\nl thread %d: %d\n", M, l);
            r = (t_parte + 1) * (total/4) - 1;
            //printf("\nr thread %d: %d\n", M, r);

          }
          else {
            break;
          }
        } 

    }

    


    if( (l < r) && (r < (qntd_numeros)) ){
        
        m = l + ((r-l)/2);
        
        mergeSort(l, m);
        //printf("Metade l thread %d:\n", M);
        //print_parcial(l, m);
        //printf("\n");

    
        
        mergeSort(m+1, r);
        //printf("Metade r thread %d:\n", M);
        //print_parcial(m+1, r);
        //printf("\n");


        merge(l, m, r);
        //printf("merging metades thread %d:\n", M);
        //print_parcial(l,r);
        //printf("\n");
    }
    else if(r == l){
      return NULL;
    }

    /*resultados[instancia] = fibo(numeros[instancia]);
    //printf("Fibo(%d) = %d\n", numeros[instancia], resultados[instancia]);*/
  }
  //printf("Saindo de thread %d\n", M);
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
  //printf("Qntd de numeros: %d\n\n", qntd_numeros);
  //printf("Original: \n");
  //printvetor_numeros(qntd_numeros);
  //printf("\n\n");


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
  

  //printf("\nSeparados:\n");
  //printvetor_numeros(qntd_numeros);

  //printf("\nm primeira metade: %d\n",(( (qntd_numeros/2) - 1 ) / 2) );
  merge(0, ( (qntd_numeros/2) - 1 ) / 2 , (qntd_numeros / 2) -1 ) ;
  //printf("Primeira metade sort:\n");
  //printvetor_numeros(qntd_numeros);

  //printf("\nm segunda metade: %d\n\n", ( ((qntd_numeros/2)-1) + ( (qntd_numeros - ( qntd_numeros/2 )) / 2 )));
  merge(qntd_numeros/2, ( ((qntd_numeros/2)-1) + ( (qntd_numeros - ( qntd_numeros/2 )) / 2 ) ), (qntd_numeros)-1 );
  //printf("Segunda metade sort:\n");
  //printvetor_numeros(qntd_numeros);

  merge(0, ((qntd_numeros / 2) -1 ), qntd_numeros - 1);
  //printf("Os dois sort:\n");
  //printvetor_numeros(qntd_numeros);
  //printf("\n");

  //printf("Final: \n");
  printvetor_numeros(qntd_numeros);

  return 0;
}

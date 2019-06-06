#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NMAX         10100
#define N_THREADS    4

//pthread_mutex_t trava;
int vetor_numeros[NMAX];
int parte = 0;
int qntd_numeros = 0;
int abrir_threads = 0;
//int MAX = 0;
pthread_mutex_t trava;


void merge(int l, int m, int r);
void mergeSort(int l, int r);
void printvetor_numeros(int size);
void* funcao_thread(void *arg);
void print_parcial(int l, int r);

int main() {
  
  //int thread_args[N_THREADS];
  
  /*for(int b = 0; b < NMAX; b++) {   //forma de 'zerar' meu vetor que tem os numeros a serem analisados//
    vetor_numeros[b] = 0;
  }*/
    
    
  while(scanf("%d ", &vetor_numeros[qntd_numeros]) != EOF ){  //Conto quantos numeros que precisaram ser analisados//
    qntd_numeros++;
  }
  printf("Qntd de numeros: %d\n\n",qntd_numeros);
  printf("Original: \n");
  printvetor_numeros(qntd_numeros);
  printf("\n\n");

  //int vetor_numeros_size = sizeof(vetor_numeros)/sizeof(vetor_numeros[0]);

  pthread_t threads[N_THREADS];

  if(qntd_numeros < 16){
    abrir_threads = 2;
  }
  else{
    abrir_threads = N_THREADS;
  }

/* Inicializo minhas threads */
  for (int i = 0; i < abrir_threads ; i++) {
    //thread_args[i] = i;
    pthread_create(&(threads[i]), NULL, funcao_thread, NULL );
  }

/*Espero minha threads finalizarem */
  for (int i = 0; i < abrir_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("\nSeparados:\n");
  printvetor_numeros(qntd_numeros);

  printf("\nm primeira metade: %d\n",(( (qntd_numeros/2) - 1 ) / 2) );
  merge(0, ( (qntd_numeros/2) - 1 ) / 2 , (qntd_numeros / 2) -1 ) ;
  printf("Primeira metade sort:\n");
  printvetor_numeros(qntd_numeros);

  printf("\nm segunda metade: %d\n\n", ( ((qntd_numeros/2)-1) + ( (qntd_numeros - ( qntd_numeros/2 )) / 2 )));
  merge(qntd_numeros/2, ( ((qntd_numeros/2)-1) + ( (qntd_numeros - ( qntd_numeros/2 )) / 2 ) ), (qntd_numeros)-1 );
  printf("Segunda metade sort:\n");
  printvetor_numeros(qntd_numeros);

  merge(0, (qntd_numeros)/2,qntd_numeros - 1);
  printf("Os dois sort:\n");
  printvetor_numeros(qntd_numeros);
  printf("\n");

  printf("Final: \n");
  printvetor_numeros(qntd_numeros);

  return 0;
}

void merge(int l, int m, int r){
    //printf("\nMerge:\n");

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
            //printvetor_numeros(qntd_numeros);
            //printf("\n");
        }
        else {
            vetor_numeros[k] = R[j];
            j++;
            //printvetor_numeros(qntd_numeros);
            //printf("\n");
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
        int m = l+(r-l)/2;
        //printf ("m vale %d\n", m);

        mergeSort(l, m);
        mergeSort(m+1, r);

        
        merge(l, m, r);
    }

    return; 
}


void* funcao_thread(void *arg) {
  int t_parte = parte ++;
  int total, l, r;

  total = qntd_numeros;
  
    
  printf("TREAD %d",t_parte);
  l = t_parte * (total/4);
  r = (t_parte + 1) * ( total/4) - 1;


  int m = l + (r-l)/2;

  if(l < r){
    
    mergeSort(l, m);
    printf("Metade l thread %d:\n", t_parte);
    print_parcial(l, m);
    printf("\n");

  
    
    mergeSort(m+1, r);
    printf("Metade r thread %d:\n", t_parte);
    print_parcial(m+1, r);
    printf("\n");


    merge(l, m, r);
    printf("merging metades thread %d:\n", t_parte);
    printvetor_numeros(qntd_numeros);
    printf("\n");
  }


  return 0;
}


void printvetor_numeros(int size){
    int i;
    for(i = 0; i < size; i++){
        printf("%d ", vetor_numeros[i]);
    }
    printf("\n");
}

void print_parcial(int l, int r){
  for(int i = l; i <= r; i++){
    printf("%d ", vetor_numeros[i]);
  }
  printf("\n");
}

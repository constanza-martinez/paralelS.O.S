#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#define MAXIMO 99999
#define MINIMO -300000
double Log2( double n )
{
    // log(n)/log(2) is log2.
    return log( n ) / log( 2. );
}
//Dimension array
int *A;
int *subarreglos;
int THREADS;
int ELEMENTOS, N;


//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

void * mergesort (void * ptr);

int main(int argc,char*argv[]){
 int i;
 double timetick;
 time_t t;
 srand((unsigned) time(&t));

 //Controla los argumentos al programa
 if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((THREADS = atoi(argv[2])) <= 0) )
  {
    printf("\nUsar: %s n\n  potencia de dos \n", argv[0]);
    printf("\nUsar: %s n\n  Threads\n", argv[0]);
    exit(1);
  }

  A=(int*)malloc(sizeof(int)*N);
  subarreglos=(int*)malloc(sizeof(int)*N);
  srand((unsigned) time(&t));
  for(i=0;i<N;i++){
   A[i]=rand() % 5000;
  }
  A[N/4]=MAXIMO;
  A[N/2]=MINIMO;


// Crear los threads y enviarle sus IDs.
  int ids[THREADS];
  ELEMENTOS = N / THREADS;
  pthread_t threads [THREADS];
  timetick = dwalltime();
  printf("Cantidad de elementos por proceso %d\n",ELEMENTOS);

  for (i = 0; i<THREADS;i++){
      ids[i] = i;
      pthread_create(&threads[i], NULL, &mergesort,&ids[i]);
      printf("Se instanció el proceso %d\n",i);
  }

  for (i=0; i < THREADS;i++){
      pthread_join(threads[i],NULL);
  }

 printf("Tiempo en segundos %f\n", dwalltime() - timetick);
 printf("UNIT TESTING: %d es el mínimo\n",A[0]);
 printf("UNIT TESTING: %d es el máximo\n",A[N-1]);
 free(A);

  }
int[] mergesort_recv(int lb, int up, int tam){
 int[tam] retornar;
  if(tam>1){

    int[] derecha=mergesort_recv(lb,up-tam/2,tam/2);
    int[] izquiera=mergesort_recv(lb+tam/2,up,tam/2);
    int index_derecha=0;
    int index_izquierda=0;
    for (int i=0;i<tam; i++) {
      if(derecha[index_derecha]!=NULL){
        if(izquiera[index_izquierda!=NULL]){
          if(derecha[index_derecha]>izquiera[index_izquierda]){
            retornar[i]=izquiera[index_izquierda];
            index_izquierda++;
          else{
            retornar[i]=derecha[index_derecha];
            index_derecha++;
            }
          }
        } else {
          retornar[i]=derecha[index_derecha];
          index_derecha++;
        }
      } else {
        retornar[i]=izquiera[index_izquierda];
        index_izquierda++;
      }
        // SEGUIR PLS
    }

  } else {
    retornar[tam]=A[lb];
  }
  return retornar;
}


void * mergesort (void * ptr) {
  int id = *((int *)ptr);
  int[] subarrelo;
  int lowerBound = ELEMENTOS*id;
  int upperBound = ELEMENTOS*(id+1);
  int iteraciones = Log2(N);
  subarrelo=mergesort_recv(lowerBound,upperBound,upperBound-lowerBound);
  for(int i=1;i<=iteraciones;i=i*2){
    for(int j=0;j<N;j=j+i*2){

    }
  }
  pthread_exit(NULL);
}

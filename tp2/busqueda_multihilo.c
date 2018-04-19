#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define ORDENXFILAS 0
#define ORDENXCOLUMNAS 1

//Dimension array
int *A;
int buscar, THREADS;
int ELEMENTOS, N;
int total = 0;
pthread_mutex_t mutexito;



//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

void * busqueda (void * ptr);

int main(int argc,char*argv[]){
 int i;
 double timetick;
 time_t t;
 pthread_mutex_init(&mutexito,NULL);
 buscar = rand() % 50;
 srand((unsigned) time(&t));

 //Controla los argumentos al programa
 if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((THREADS = atoi(argv[2])) <= 0) )
  {
    printf("\nUsar: %s n\n  potencia de dos \n", argv[0]);
    printf("\nUsar: %s n\n  Threads\n", argv[0]);
    exit(1);
  }

 //Aloca memoria para las matrices
  A=(int*)malloc(sizeof(int)*N);

 //Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
  for(i=0;i<N;i++){
   A[i]=rand() % 50;
  }


// Crear los threads y enviarle sus IDs.
  int ids[THREADS];
  ELEMENTOS = N / THREADS;
  pthread_t threads [THREADS];
  timetick = dwalltime();
  printf("Cantidad de elementos por proceso %d\n",ELEMENTOS);

  for (i = 0; i<THREADS;i++){
      ids[i] = i;
      pthread_create(&threads[i], NULL, &busqueda,&ids[i]);
      printf("Se instanció el proceso %d\n",i);
  }

  for (i=0; i < THREADS;i++){
      pthread_join(threads[i],NULL);
  }

 printf("Tiempo en segundos %f\n", dwalltime() - timetick);
 printf("Se buscó: %d\n",buscar);
 printf("Se encontró: %d\n", total);
 free(A);

  }


void * busqueda (void * ptr) {
  int id = *((int *)ptr);
  int i;
  int ocurrencias = 0;
  for(i=ELEMENTOS*id;i<ELEMENTOS*(id+1);i++){
    if(A[i]==buscar){
      ocurrencias++;
    }
  }
  pthread_mutex_lock(&mutexito);
  total+=ocurrencias;
  pthread_mutex_unlock(&mutexito);
  pthread_exit(NULL);
}

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

//Dimension array
int *A;
int *PARCIALES;
int THREADS;
int ELEMENTOS, N;
double prom=0.0;
pthread_mutex_t mutexito;



//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

void * promedio (void * ptr);

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

 //Aloca memoria para las matrices
  A=(int*)malloc(sizeof(int)*N);
  PARCIALES=(int*)malloc(sizeof(int)*THREADS);
 //Inicializa array
  srand((unsigned) time(&t));
  for(i=0;i<N;i++){
   A[i]=rand() % 5000;
  }


// Crear los threads y enviarle sus IDs.
  int ids[THREADS];
  ELEMENTOS = N / THREADS;
  pthread_t threads [THREADS];
  timetick = dwalltime();
  printf("Cantidad de elementos por proceso %d\n",ELEMENTOS);

  for (i = 0; i<THREADS;i++){
      ids[i] = i;
      pthread_create(&threads[i], NULL, &promedio,&ids[i]);
      printf("Se instanció el proceso %d\n",i);
  }

  for (i=0; i < THREADS;i++){
      pthread_join(threads[i],NULL);
  }
  //SUMA TOTAL
  for (i=0;i < THREADS;i++){
    prom+=PARCIALES[i];
  }
  prom/=N;
 printf("Tiempo en segundos %f\n", dwalltime() - timetick);
 printf("Promedio: %f\n",prom);
 free(A);

  }


void * promedio (void * ptr) {
  int id = *((int *)ptr);
  int i,sum;
  sum=0;
  for(i=ELEMENTOS*id;i<ELEMENTOS*(id+1);i++){
    sum+=A[i];
  }
  PARCIALES[id]=sum;
  pthread_exit(NULL);
}

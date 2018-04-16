#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#define ORDENXFILAS 0
#define ORDENXCOLUMNAS 1

//Dimension por defecto de las matrices
int N=100;
int THREADS = 1;
int STRIPES;
double *A,*B,*C;
//Retorna el valor de la matriz en la posicion fila y columna segun el orden que este ordenada
double getValor(double *matriz,int fila,int columna,int orden){
 if(orden==ORDENXFILAS){
  return(matriz[fila*N+columna]);
 }else{
  return(matriz[fila+columna*N]);
 }
}

//Establece el valor de la matriz en la posicion fila y columna segun el orden que este ordenada
void setValor(double *matriz,int fila,int columna,int orden,double valor){
 if(orden==ORDENXFILAS){
  matriz[fila*N+columna]=valor;
 }else{
  matriz[fila+columna*N]=valor;
 }
}

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

void * multiplicar (void * ptr);

int main(int argc,char*argv[]){
 int i,j,k;
 int check=1;
 double timetick;

 //Controla los argumentos al programa
 if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((THREADS = atoi(argv[2])) <= 0) )
  {
    printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
    printf("\nUsar: %s n\n  Treads\n", argv[0]);
    exit(1);
  }

 //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);

 //Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	setValor(A,i,j,ORDENXFILAS,1);
	setValor(B,i,j,ORDENXFILAS,1);
    setValor(C,i,j,ORDENXFILAS,0);
   }
  }


// Crear los threads y enviarle sus IDs.
  int ids[THREADS];
  STRIPES = N / THREADS;
  pthread_attr_t attr; // Según Adrian podría ser NULL
  pthread_attr_init(&attr);
  pthread_t threads [THREADS];
  timetick = dwalltime();
  printf("Cantidad de filas por proceso %d\n",STRIPES);

  for (i = 0; i<THREADS;i++){
      ids[i] = i;
      pthread_create(&threads[i], NULL, &multiplicar, &ids[i]);
      printf("Se instanció el proceso %d\n",i);
  }

 //Realiza la multiplicacion


  for (i=0; i < THREADS;i++){
      pthread_join(threads[i],NULL);
  }



 printf("Tiempo en segundos %f\n", dwalltime() - timetick);

 //Verifica el resultado
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	check=check&&(getValor(C,i,j,ORDENXFILAS)==N);
   }
  }

  if(check){
   printf("Multiplicacion de matrices resultado correcto\n");
   free(A);
   free(B);
   free(C);
   return(0);
  }else{
    free(A);
    free(B);
    free(C);
   printf("Multiplicacion de matrices resultado erróneo\n");
   return(1);
  }
}

void * multiplicar (void * ptr) {
  int id = *((int *)ptr);
  int i,j,k;
  int pilisTKM;
  for(i=STRIPES*id;i<STRIPES*(id+1);i++){
    for(j=0;j<N;j++){
    pilisTKM=0;
     for(k=0;k<N;k++){
	     pilisTKM = pilisTKM + A[i*N+k] * B[j*N+k];
       //printf("\n%d",id);
     }
     C[i*N+j]=pilisTKM;

     }
   }
   pthread_exit(NULL);
}

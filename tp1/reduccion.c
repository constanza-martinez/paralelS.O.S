#include<stdio.h>
#include<stdlib.h>
#include<math.h>
double Log2( double n )
{
    // log(n)/log(2) is log2.
    return log( n ) / log( 2. );
}
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

int main(int argc,char*argv[]){
 double *A;
 int N,i,j;
 int check=1;
 double timetick;

 //Controla los argumentos al programa
 if ((argc != 2) || ((N = atoi(argv[1])) <= 0) )
  {
    printf("\nUsar: %s n\n  n: Dimension del Array (nxn X nxn)\n", argv[0]);
    exit(1);
  }

  A=(double*)malloc(sizeof(double)*N);
printf("LLEGUÉ A LA INICIALIZACIÖN\n");
// Inicia array
time_t t;
srand((unsigned) time(&t));

  for(i=0;i<N;i++){

      A[i]=rand();

  }
  printf("LOGARITMACIÖN\n");
  int iteraciones = Log2(N);
  timetick = dwalltime();
  printf("ITERACION\n");
  for(i=1;i<=iteraciones;i=i*2){
    for(j=0;j<N;j=j+i*2){
      A[j]=A[j]/A[j+i];
      printf("i: %d\n",i);
    }
  }
  printf("\nResultado %f\n", A[0]);


 printf("Tiempo en segundos %f\n", dwalltime() - timetick);
 free(A);
 return(0);
}

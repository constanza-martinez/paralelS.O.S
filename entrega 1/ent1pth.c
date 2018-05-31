# include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int N=0;
double *A, *B, *C, *D, *L, *AB, *ABC, *LB, *LBD, *M;
double VALOR_MAX =0;
int NUM_THREADS;
double l = 0;
double b = 0;
int contT = 0;


pthread_mutex_t mut;
pthread_barrier_t barrera;

// void imprimirMatriz(double * matriz){
// 	int i,j;
// 	for (i = 0; i < N; ++i){
// 		for (j = 0; j < N; ++j){
// 			printf("%f |",matriz[i*N+j]);
// 		}
// 		printf("\n");
// 	}
// }

double dwalltime(){
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}
void prom(int base, int tope){
	double cantl=0;
	double cantb=0;
	int i,j;

	for(i=base;i<tope;i++){
		for(j=0;j<N;j++){
			cantb += B[i+N*j];
		}
	}
	for(i=base;i<tope;i++){
		for(j=0;j<i;j++){
			cantl += L[i*N+j];
		}
	}
		pthread_mutex_lock(&mut);
		l += cantl;
		b += cantb;

		if(contT == (NUM_THREADS-1)){
			l = l/(N*N);
			b = b/(N*N);
		}else{
			contT++;
		}
		pthread_mutex_unlock(&mut);
}
	//multiplico A y L por los promedios
void mulAL(int base, int tope){
	int i,j;
 	for (i=base;i<tope;i++){
 		for(j=0;j<N;j++){
 			A[i*N+j]*=l;
 			if(j<=i){
 				L[i*N+j]*=b;
 			}
 		}
 	}
}
void mul(double * Res,double *mat1, double *mat2, int base, int tope){
	int i,j,k;
	for (i = base; i < tope; ++i){
		for (j = 0; j < N; ++j){
			for (k = 0; k < N; ++k){
				Res[i*N+j]= Res[i*N+j] + mat1[i*N+k] * mat2[k+N*j] ;
			}
		}
	}
}
void sumM(int base, int tope){
	int i,j;
	for (i=base;i<tope;i++){
		for(j=0;j<N;j++){
			M[i*N+j]= ABC[i*N+j]+LBD[i*N+j];
		}
	}
}

void* hilos(void* ptr){
	int *p ,id;
	p = (int *)ptr;
	id = *p;
	int base,tope;
	base=(N*id)/NUM_THREADS;
	tope=((id+1)*N)/NUM_THREADS;
	prom(base,tope);
	pthread_barrier_wait(&barrera);
	mulAL(base,tope);
	pthread_barrier_wait(&barrera);
	mul(AB,A,B,base,tope);
	mul(LB,L,B,base,tope);
	mul(ABC,AB,C,base,tope);
	mul(LBD,LB,D,base,tope);
	pthread_barrier_wait(&barrera); //fijarse si va esta aca
	sumM(base,tope);
	pthread_exit(NULL);
}

int main(int argc,char*argv[]){

	double timetick;

	if((argc !=3)||((N = atoi(argv[1]))<=0)){
		printf("Datos erroneos %s\n",argv[0] );
		exit(1);
	}
  NUM_THREADS = atoi(argv[2]);

// test oush
A = (double*)malloc(sizeof(double)*N*N);
B = (double*)malloc(sizeof(double)*N*N);
C = (double*)malloc(sizeof(double)*N*N);
L = (double*)malloc(sizeof(double)*N*N);
D = (double*)malloc(sizeof(double)*N*N);
AB = (double*)malloc(sizeof(double)*N*N);
ABC = (double*)malloc(sizeof(double)*N*N);
LB = (double*)malloc(sizeof(double)*N*N);
LBD = (double*)malloc(sizeof(double)*N*N);
M = (double*)malloc(sizeof(double)*N*N);

	int i,j;
	int ids[NUM_THREADS];
	pthread_t threads[NUM_THREADS];
	pthread_mutex_init(&mut, NULL);
	pthread_barrier_init(&barrera,NULL,NUM_THREADS);
	srand(time(NULL));


	for (i=0;i<N;i++){
		for(j=0;j<N;j++){
			A[i*N+j] = 1; //fila
			B[i+N*j] = 1;	//col
			C[i+N*j] = 1;
			D[i+N*j] = 1;
			if(j<=i){
				L[i*N+j]=1;
			}
			AB[i*N+j] = 0;
			ABC[i*N+j] = 0;
			LB[i*N+j] = 0;
			LBD[i*N+j] = 0;
			M[i*N+j] = 0;
		}
	}
	int check = 1;
	timetick = dwalltime();
	for(i=0;i<NUM_THREADS;i++){
		ids[i]=i;
		pthread_create(&threads[i],NULL,hilos,&ids[i]);
	}
	for(i=0;i<NUM_THREADS;i++){
			pthread_join(threads[i],NULL);
	}
	VALOR_MAX=(N*N)*l;
	printf("Tiempo en segundos %f\n", dwalltime() - timetick);
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			check=check&&(M[i*N+j]==(VALOR_MAX+(N*(i+1))));
		}
	}

	if(check){
		printf("Multiplicacion de matrices resultado correcto\n");
		}
	else{
		printf("Multiplicacion de matrices resultado erroneo\n");
		}

	free(A);
	free(B);
	free(C);
	free(D);
	free(L);
	free(AB);
	free(ABC);
	free(LB);
	free(LBD);
	free(M);
	return 0;

}

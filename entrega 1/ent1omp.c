# include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int N=0;
double *A, *B, *C, *D, *L, *AB, *ABC, *LB, *LBD, *M;
double VALOR_MAX =0;
int NUM_THREADS = 0;
double dwalltime(){
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}
void imprimirMatriz(double * matriz){
	int i,j;
	for (i = 0; i < N; ++i){
		for (j = 0; j < N; ++j){
			printf("%f |",matriz[i*N+j]);
		}
		printf("\n");
	}
}
int main(int argc,char*argv[]){

	double timetick;

	if((argc !=3)||((N = atoi(argv[1]))<=0)){
		printf("Datos erroneos %s\n",argv[0] );
		exit(1);
	}
	NUM_THREADS = atoi(argv[2]);
	omp_set_num_threads(NUM_THREADS);
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

	int i,j,k;
	double l = 0;
	double b = 0;
	int check = 1;
	double res1=0;

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

	timetick = dwalltime();
#pragma omp parallel for shared(B) private(i,j) reduction(+:b)
	for (i=0;i<N;i++){
		for(j=0;j<N;j++){
			b += B[i+N*j];
		}
	}
	b = b/(N*N);
	#pragma omp parallel for shared(L) private(i,j) reduction(+:l)
		for (i=0;i<N;i++){
			for(j=0;j<i;j++){
				l += L[i*N+j];
			}
		}
		l = l/(N*N);

	VALOR_MAX = (N*N) * l;
 //multiplico A y L por los promedios
 #pragma omp parallel for shared(l,A) private(i,j)
	for (i=0;i<N;i++){
		for(j=0;j<N;j++){
			A[i*N+j]*=l;
		}
	}

	#pragma omp parallel for shared(b,L) private(i,j)
 	for (i=0;i<N;i++){
 		for(j=0;j<i;j++){
 				L[i*N+j]*=b;
 		}
 	}
	//multiplico las matrices y la guardo en la nueva sumando
	//lo que va quedando

	#pragma omp parallel for shared(AB,B,A,N) private(i,j,k,res1) 
	for (i=0;i<N;i++){
		for(j=0;j<N;j++){
			for(k=0;k<N;k++){
				res1 =  res1 + A[i*N+k]*B[k+j*N];
			}
				AB[i*N+j] = res1;
				res1=0;
		}
	}

	#pragma omp parallel for shared(LB,B,L) private(i,j,k,res1)
	for (i=0;i<N;i++){
		for(j=0;j<N;j++){
				for(k=0;k<N;k++){
					res1 += L[i*N+k]*B[k+j*N];
				}
				LB[i*N+j] = res1;
				res1=0;
		}
	}

	#pragma omp parallel for shared(ABC,AB,C) private(i,j,k,res1)
	for (i=0;i<N;i++){
		for(j=0;j<N;j++){
			for(k=0;k<N;k++){
				res1+= AB[i*N+k]*C[k+N*j];
			}
				ABC[i*N+j] = res1;
				res1=0;
		}
	}
#pragma omp parallel for shared(LBD,LB,D) private(i,j,k,res1)
	for (i=0;i<N;i++){
		for(j=0;j<N;j++){
				for(k=0;k<N;k++){
					res1 +=LB[i*N+k]*D[k+N*j];
				}
				LBD[i*N+j] = res1;
				res1=0;
		}
	}

	#pragma omp parallel for shared(M,ABC,LBD) private(i,j)
	for (i=0;i<N;i++){
		for(j=0;j<N;j++){
			M[i*N+j]= ABC[i*N+j]+LBD[i*N+j];
		}
	}

	printf("Tiempo en segundos %f\n", dwalltime() - timetick);
	//Verifica el resultado
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
		//imprimirMatriz(M);

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

# include <stdio.h>
#include <stdlib.h>

int N;
int *A, *B, *C, *D, *L, AB*, ABC*, LB*, LBD*;

double dwalltime(){
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

int main(int argc,char*argv[]){

	double timetick;

	if((argc !=2)||((N = atoi(argv[1]))<=0)){
		printf("Datos erroneos %s\n",argv[0] );
		exit(1);
	}
// test oush
	A = (int*)malloc(sizeof(int)*N);
	B = (int*)malloc(sizeof(int)*N);
	C = (int*)malloc(sizeof(int)*N);
	D = (int*)malloc(sizeof(int)*N);
	L = (int*)malloc(sizeof(int)*N);
	AB = (int*)malloc(sizeof(int)*N);
	ABC = (int*)malloc(sizeof(int)*N);
	LB = (int*)malloc(sizeof(int)*N);
	LBD = (int*)malloc(sizeof(int)*N);
	M = (int*)malloc(sizeof(int)*N);

	srand(time(NULL));


	for (i=0;i<N*N;i++){
		A[i] = 1;
		B[i] = 1;
		C[i] = 1;
		D[i] = 1;
		AB[i] = 0;
		ABC[i] = 0;
		LB[i] = 0;
		LBD[i] = 0;
		M[i] = 0;
	}

	timetick = dwalltime();


	printf("Tiempo en segundos %f\n", dwalltime() - timetick);
	printf("Promedio del arreglo: %d\n", tot/N);

	free(V);
	return 0;

}

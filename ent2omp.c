#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


double dwalltime(){
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}


int main (int argc,char*argv[]){
  double timetick;
	int i;
	int *V;
	int N=0;
	int pares=0;
	int NUM_THREADS;
//	int cont=0;

	if((argc !=3)||((N = atoi(argv[1]))<=0)){
		printf("Datos erroneos %s\n",argv[0] );
		exit(1);
	}
  NUM_THREADS = atoi(argv[2]);
	V = (int*)malloc(sizeof(int)*N);
	//pongo solo pares
	for (i=0;i<N;i++){
		V[i]=2;
	}
	omp_set_num_threads(NUM_THREADS);

	timetick = dwalltime();

	#pragma omp parallel for shared(N,V) private(i) reduction(+:pares)
		for(i=0;i<N;i++){
			if( V[i] % 2 == 0){
				pares++;
			}
		//	cont++;
		}

	printf("Tiempo en segundos %f\n", dwalltime() - timetick);
	printf("Pares: %d\n", pares);
	//printf("contador de veces que entro al for: %d\n", cont);
	free(V);
	return 0;

}

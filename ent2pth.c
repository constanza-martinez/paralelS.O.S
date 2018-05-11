#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int *V;
int N;
int pares=0;
int NUM_THREADS;
pthread_mutex_t mut;

double dwalltime(){
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}
void* sum(void* ptr){
	int *p ,id;
	p = (int *)ptr;
	id = *p;
	int i;
	int base,tope;
	base=(N*id)/NUM_THREADS;
	tope=((id+1)*N)/NUM_THREADS;
	int cant=0;

	for(i=base;i<tope;i++){
		if((V[i]%2)==0){
			cant++;
		}
	}
	if(cant != 0){
		pthread_mutex_lock(&mut);
		pares += cant;
		pthread_mutex_unlock(&mut);
	}
	pthread_exit(NULL);
}

int main (int argc,char*argv[]){
  double timetick;
	unsigned long long int i;

	if((argc !=3)||((N = atoi(argv[1]))<=0)){
		printf("Datos erroneos %s\n",argv[0] );
		exit(1);
	}
  NUM_THREADS = atoi(argv[2]);
	V = (int*)malloc(sizeof(int)*N);

	int ids[NUM_THREADS];
	pthread_t threads[NUM_THREADS];
	srand(time(NULL));

	//pongo solo pares
	for (i=0;i<N;i++){
		V[i]=2;
	}

	timetick = dwalltime();

	for(i=0;i<NUM_THREADS;i++){
		ids[i]=i;
		pthread_create(&threads[i],NULL,sum,&ids[i]);
	}
	for(i=0;i<NUM_THREADS;i++){
			pthread_join(threads[i],NULL);
	}

	printf("Tiempo en segundos %f\n", dwalltime() - timetick);
	printf("Pares: %d\n", pares);

	free(V);
	return 0;

}

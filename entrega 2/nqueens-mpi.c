#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>

#define MAX_N 16
int SOLUCIONES = 0;
int max_iter = 1;
int num_workers;
int n;
MPI_Status estado;
double start_time, end_time;
void codigoMaster();
void codigoWorker();

double dwalltime(){
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec/1000000.0;
	return sec;
}

int check_acceptable(int queen_rows[MAX_N], int n)
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		for (j = i+1; j < n; j++)
		{
			// two queens in the same row => not a solution!
			if (queen_rows[i] == queen_rows[j]) return 0;

			// two queens in the same diagonal => not a solution!
			if (queen_rows[i] - queen_rows[j] == i - j ||
			    queen_rows[i] - queen_rows[j] == j - i)
			    return 0;
		}
	}

	return 1;
}

int main(int argc, char* argv[]){
    int id;
		int i;
		n = (argc > 1) ? atoi(argv[1]) : 8;
		start_time = dwalltime();
  	MPI_Init(&argc, &argv);
  	MPI_Comm_size(MPI_COMM_WORLD,&num_workers);
  	MPI_Comm_rank(MPI_COMM_WORLD,&id);
		if(id==0){
			codigoMaster();
		} else {
			codigoWorker();
		}
    MPI_Finalize();
		return 0;
}

void codigoMaster(){
	for (int 	i = 0; i < n; i++){
			max_iter *= n;
	}
	int pilisRecepcion;
	int workersDisponibles = 1;
for (int iter =  0; iter < max_iter; iter+=(n*n)){
	int flag=0;
	MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &estado); // Hay un Pedido entrante
	if(flag){
		MPI_Recv(&pilisRecepcion,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&estado);
		workersDisponibles++;
		if(pilisRecepcion){
			// PEDIDO CON SOLUCION
			SOLUCIONES+=pilisRecepcion;
		}
		MPI_Send(&iter,1,MPI_INT,estado.MPI_SOURCE,0,MPI_COMM_WORLD);
		workersDisponibles--;
	} else {
		int iterar=iter;
		for (int j = 0; j < n*n; j++) {
			int code = iterar; // WORKERITO
			int i;
			int queen_rows[MAX_N];
			for (i = 0; i < n; i++){
				queen_rows[i] = code % n;
				code /= n;
			}
			if (check_acceptable(queen_rows, n)){
					SOLUCIONES++;
			}
			iterar++;
		}
	}
}
int fin=-1;
	while(workersDisponibles<num_workers){
		MPI_Recv(&pilisRecepcion,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&estado);
		workersDisponibles++;
		if(pilisRecepcion){
			// PEDIDO CON SOLUCION
			SOLUCIONES++;
		}

		MPI_Send(&fin,1,MPI_INT,estado.MPI_SOURCE,0,MPI_COMM_WORLD);
	}


	// get end time
	end_time = dwalltime();
	// print results
	printf("The execution time is %f sec\n", end_time - start_time);
	printf("Number of found solutions is %d\n", SOLUCIONES);
}

void codigoWorker(){
	// pedirTarea
	int tarea=0;
	MPI_Send(&tarea,1,MPI_INT,0,0,MPI_COMM_WORLD);
	MPI_Recv(&tarea,1,MPI_INT,0,0,MPI_COMM_WORLD,&estado);
	while(tarea>-1){
		int send=0;
		 // WORKERITO
		for (int j = 0; j < n*n; j++) {
			int code = tarea;
			int queen_rows[MAX_N];
			for (int i = 0; i < n; i++){
				queen_rows[i] = code % n;
				code /= n;
			}

			if(check_acceptable(queen_rows, n)){
				send++;
			}
			tarea++;
		}
		// pedirTarea
		MPI_Send(&send,1,MPI_INT,0,0,MPI_COMM_WORLD);
		MPI_Recv(&tarea,1,MPI_INT,0,0,MPI_COMM_WORLD,&estado);
	}

}

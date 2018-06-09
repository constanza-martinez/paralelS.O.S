#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <mpi.h>

#define MAX_N 16
int SOLUCIONES = 0;

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

int main(int argc, char* argv[]) // READY
{
    int n;
    int max_iter = 1;
    int num_workers;
    int id;
		double start_time, end_time;
		int i;
		n = (argc > 1) ? atoi(argv[1]) : 8;
		for (i = 0; i < n; i++){
				max_iter *= n;
		}
		start_time = dwalltime();
		int iter;
  	MPI_Init(&argc, &argv);
  	MPI_Comm_size(MPI_COMM_WORLD,&num_workers);
  	MPI_Status estado;
  	MPI_Comm_rank(MPI_COMM_WORLD,&id);
		if(id==0){
			codigoMaster();
		} else {
			codigoWorker();
		}
    MPI_Finalize();
		return 0;
}

codigoMaster(){
	int pilisRecepcion;
	int workersDisponibles = 1;
for (iter =  0; iter < max_iter; iter++){
	int flag=0;
	MPI_Iprobe(ANY_SOURCE, ANY_TAG, MPI_COMM_WORLD, &flag, &estado); // Hay un Pedido entrante
	if(flag){
		MPI_Recv(&pilisRecepcion,1,MPI_INT,ANY_SOURCE,ANY_TAG,MPI_COMM_WORLD,&estado);
		workersDisponibles++;
		if(estado.MPI_TAG==1){
			// PEDIDO CON SOLUCION
			SOLUCIONES++;
		}
		MPI_Send(&iter,1,MPI_INT,0,1,MPI_COMM_WORLD);
		workersDisponibles--;
	} else {
		int code = iter; // WORKERITO
		int i;
		int queen_rows[MAX_N];
		for (i = 0; i < n; i++){
			queen_rows[i] = code % n;
			code /= n;
		}
		if (check_acceptable(queen_rows, n))
		{
				SOLUCIONES++;
		}
	}
}
	while(workersDisponibles!=num_workers){
		MPI_Recv(&pilisRecepcion,1,MPI_INT,ANY_SOURCE,ANY_TAG,MPI_COMM_WORLD,&estado);
		workersDisponibles++;
		if(estado.MPI_TAG==1){
			// PEDIDO CON SOLUCION
			SOLUCIONES++;
		}
		MPI_Send(-1,1,MPI_INT,0,1,MPI_COMM_WORLD);
	}


	// get end time
	end_time = dwalltime();
	// print results
	printf("The execution time is %f sec\n", end_time - start_time);
	printf("Number of found solutions is %d\n", number_solutions);
}

codigoWorker(){
	// pedirTarea
	int tarea=0;
	MPI_Send(&tarea,1,MPI_INT,0,0,MPI_COMM_WORLD);
	MPI_Recv(&tarea,1,MPI_INT,0,0,MPI_COMM_WORLD,&estado);
	while(tarea!=-1){
		int code = tarea; // WORKERITO
		int i;
		int queen_rows[MAX_N];
		for (i = 0; i < n; i++){
			queen_rows[i] = code % n;
			code /= n;
		}
		int send;
		send = (check_acceptable(queen_rows, n) ? 1 : 0;
		// pedirTarea
		MPI_Send(&tarea,1,MPI_INT,0,send,MPI_COMM_WORLD);
		MPI_Recv(&tarea,1,MPI_INT,0,0,MPI_COMM_WORLD,&estado);
	}

}

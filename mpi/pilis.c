#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc,char *argv[]){
  int cant,id;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD,&cant);
  MPI_Status pilisEstado;

  MPI_Comm_rank(MPI_COMM_WORLD,&id);
if(id==1){
  int pilis=12467;
  MPI_Send(&pilis,1,MPI_INT,0,1,MPI_COMM_WORLD);
} else {
  
int pilisRecepcion;

  MPI_Recv(&pilisRecepcion,1,MPI_INT,1,1,MPI_COMM_WORLD,&pilisEstado);
printf("RECIBI LEGAJO %d porque soy el Proceso %d \n",pilisRecepcion,id);
}
  printf("Soy %d de %d \n",id,cant);
  MPI_Finalize();
  return 0;
}

/* C Example */
#include <mpi.h>
#include <stdio.h>
 
int main (int argc, char* argv[])
{
  int rank, size, name_len;
  char name[MPI_MAX_PROCESSOR_NAME];
 
  MPI_Init (&argc, &argv);      /* starts MPI */
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
  MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
  MPI_Get_processor_name(name, &name_len); /* get hostname */
  printf( "Hello world from processor %s, rank %d of %d\n", name, rank, size );
  MPI_Finalize();
  return 0;
}

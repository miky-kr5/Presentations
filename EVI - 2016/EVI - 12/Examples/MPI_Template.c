#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char **argv) {
  int rank;

  // Initalize MPI and get the process' rank.
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(rank == 0) {
    int size, i;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("MASTER: %d processes available.\n", size);

    for(i = 1; i < size; i++) {
      // Send data to slaves.
    }

    for(i = 1; i < size; i++) {
      // Receive results from slaves.
    }

    // Do something with the results.
    
  } else {
    
    // Slave's code.
    
  }

  MPI_Finalize();
  
  return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define BUFSIZE 128

int main(int argc, char **argv) {
  char buffer[BUFSIZE];
  int rank;

  /* Initalize MPI and get the process' rank. */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  
  if(rank == 0) {
    memset(buffer, 0, BUFSIZE);
    sprintf(buffer, "Hello, Broadcast!");
    printf("MASTER: Preparing for broadcast!\n");
  }

  MPI_Bcast(buffer, BUFSIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

  if(rank != 0) {
    printf("SLAVE %d: Master broadcasted \"%s\"\n", rank, buffer);
  }
  
  MPI_Finalize();
  
  return EXIT_SUCCESS;
}

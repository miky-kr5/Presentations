#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
 
#define BUFSIZE 1000 /* Amount of numbers to sum. */
#define TAG 0        /* Message tag. */

int main(int argc, char **argv) {
  int size, rank, elems, i, temp = 0, sum = 0;
  
  /* Start with MPI_Init and get the rank. */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /***********************************************************************
   * MASTER PROCESS                                                      *
   ***********************************************************************/
  if(rank == 0) {
    int array[BUFSIZE];
    
    /* Get the number of processes. */
    printf("MASTER: %d processes available.\n", size);

    /* Create the data to send. */
    for(i = 0; i < BUFSIZE; i++) {
      array[i] = 9;
    }

    /* Calculate how much data to send to each slave. */
    elems = BUFSIZE / (size - 1);
    
    /* Send data to the slaves. */
    for(i = 1; i < size; i++) {
      MPI_Send(&array[(i - 1) * elems], elems, MPI_INT, i, TAG, MPI_COMM_WORLD);
    }
    
    /***********************************************************************
     * SLAVE PROCESS                                                       *
     ***********************************************************************/
  } else {
    int * buffer;
    
    /* Calculate how much data to receive from the master and allocate memory. */
    elems = BUFSIZE / (size - 1);
    buffer = (int *)malloc(sizeof(int) * elems);
    
    /* Receive orders from the master. */
    MPI_Recv(buffer, elems, MPI_INT, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    /* Calculate. */
    for(i = 0; i < elems; i++)
      temp += buffer[i];

    /* Clean up. */
    free(buffer);
  }

  MPI_Reduce(&temp, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if(rank == 0)
    printf("MASTER: The sum of the array is: %d.\n", sum);
  
  /* End with MPI_Finalize. */
  MPI_Finalize();
  
  return EXIT_SUCCESS;
}

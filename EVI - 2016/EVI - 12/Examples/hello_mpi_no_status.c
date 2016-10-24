#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
 
#define BUFSIZE 128 /* Enough space for our needs. */
#define TAG 0       /* Message tag. */
 
int main(int argc, char *argv[]) {
  char buff[BUFSIZE];
  int rank;

  /* Start with MPI_Init and get the rank. */
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /***********************************************************************
   * MASTER PROCESS                                                      *
   ***********************************************************************/
  if(rank == 0) {
    int size, i;
    
    /* Get the number of processes. */
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("MASTER: %d processes available.\n", size);

    /* Send data to the slaves. */
    for(i = 1; i < size; i++) {
      memset(buff, 0, BUFSIZE);
      sprintf(buff, "Hello, Process %d! ", i);
      MPI_Send(buff, BUFSIZE, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
    }

    /* Receive answers from the slaves. */
    for(i = 1; i < size; i++) {
      memset(buff, 0, BUFSIZE);
      MPI_Recv(buff, BUFSIZE, MPI_CHAR, i, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("MASTER: Received \"%s\"\n", buff);
    }

    /***********************************************************************
     * SLAVE PROCESS                                                       *
     ***********************************************************************/
  } else {
    /* Receive orders from the master. */
    memset(buff, 0, BUFSIZE);
    MPI_Recv(buff, BUFSIZE, MPI_CHAR, 0 /*Master's rank*/, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("SLAVE %d: Master says: \"%s\"\n", rank, buff);

    /* Calculate something. */
    memset(buff, 0, BUFSIZE);
    sprintf(buff, "Processor %d reporting for duty!", rank);

    /* Send answer to the master. */
    MPI_Send(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
  }
 
  /* End with MPI_Finalize. */
  MPI_Finalize();
  
  return EXIT_SUCCESS;
}

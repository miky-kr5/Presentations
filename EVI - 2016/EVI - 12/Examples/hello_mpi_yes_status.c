#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

#define TAG 0 /* Message tag. */
 
int main(int argc, char *argv[]) {
  char * buff = NULL;
  int rank;

  /* Start with MPI_Init and get the rank. */
  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /***********************************************************************
   * MASTER PROCESS                                                      *
   ***********************************************************************/
  if(rank == 0) {
    int size, i;
    
    buff = (char *)calloc(128, sizeof(char));
    
    /* Get the number of processes. */
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("MASTER: %d processes available.\n", size);

    /* Send data to the slaves. */
    for(i = 1; i < size; i++) {
      memset(buff, 0, 128);
      sprintf(buff, "Hello, Process %d!", i);
      MPI_Send(buff, strlen(buff) + 1, MPI_CHAR, i, TAG, MPI_COMM_WORLD);
    }

    /***********************************************************************
     * SLAVE PROCESS                                                       *
     ***********************************************************************/
  } else {
    MPI_Status status;
    int data_size;
    
    /* Probe orders from the master. */
    MPI_Probe(0 /*Master's rank*/, TAG, MPI_COMM_WORLD, &status);
    
    /* Find the amount of data sent by the master and allocate memory for it. */
    MPI_Get_count(&status, MPI_CHAR, &data_size);
    buff = (char *)calloc(data_size, sizeof(char));

    /* Actually receive the data. */
    MPI_Recv(buff, data_size, MPI_CHAR, 0 /*Master's rank*/, TAG, MPI_COMM_WORLD, &status);
    printf("SLAVE %d: Master says: \"%s\"\n", rank, buff);
  }
  
  /* Clean up. */
  if(buff != NULL)
    free(buff);
  
  /* End with MPI_Finalize. */
  MPI_Finalize();
  
  return EXIT_SUCCESS;
}

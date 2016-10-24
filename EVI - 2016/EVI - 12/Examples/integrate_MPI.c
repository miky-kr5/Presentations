#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <mpi.h>

/****************************************************************************************************************************
 * CONSTANTS                                                                                                                *
 ****************************************************************************************************************************/

const double       REAL      = 1.462651745907181608804; /* A precalculated value of the integral for error calculation. */
const long int     N_INTERV  = 1073741824;              /* Number of quadrature intervals. */
const double       a         = 0.0;                     /* First point of the integration interval. */
const double       b         = 1.0;                     /* Last point of the integration interval. */

/****************************************************************************************************************************
 * FUNCTION PROTOTYPES                                                                                                      *
 ****************************************************************************************************************************/

double f( double );              /* The function to integrate. */
double dabs( double );           /* An absolute value function for doubles (C only has integer absolute value). */

/****************************************************************************************************************************
 * MAIN FUNCTION                                                                                                            *
 ****************************************************************************************************************************/

/*
 * Calculate the integral of f using the trapezoid rule.
 * The trapezoid rule is defined as follows:
 *
 *      - b                --           n - 1             --
 *     |               h   |             ---               |
 * I = |    f(x) dx ~= - * | f(a) +  2 *  >  f(x_j) + f(b) |
 *     |               2   |             ---               |
 *    -   a                --           j = 1             --
 *
 * Where:
 *   *) n is an arbitrary number of intervals between a and b.
 *   *) h = ( b - a ) / n
 */
int main( int argc, char ** argv ){
  clock_t    t1;       /* Time at the start of the computation. */
  clock_t    t2;       /* Time at the end of the computation. */
  double     secs;     /* Total time of execution in seconds. */
  double     I;        /* Result of the integration. */
  double     accum;    /* Accumulator of intermediate results. */
  double     h;        /* */
  int        n_procs;  /* Number of active MPI processes. */
  int        m_id;     /* MPI Process ID. */
  long       p_id;     /* Process id for calculations. */
  long       msg[ 1 ]; /* Message to send between processes. */
  double     res[ 1 ]; /* Result message from each thread. */
  MPI_Status stat;     /* Status of MPI communications. */
  long       i;        /* Iterator variable. */

  I     = 0;
  accum = 0;
  h     = ( b - a ) / N_INTERV;

  /* MPI setup. */
  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &n_procs );
  MPI_Comm_rank( MPI_COMM_WORLD, &m_id );

  if( m_id == 0 ) { /* Master process. */
    t1 = clock(  );

    /* Distribute the intervals among the processes. */
    for( i = 1; i < n_procs; ++i ) {
      p_id = i - 1;
      msg[ 0 ] = p_id;
      MPI_Send( msg, 1, MPI_LONG, i, 0, MPI_COMM_WORLD );
    }

    /* Wait for the processes to end and send their results. */
    for( i = 1; i < n_procs; ++i ) {
      MPI_Recv( res, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &stat );
      accum += res[ 0 ];
    }

    /* Apply the quadrature formula. */
    I = ( h / 2 ) * ( f( a ) + ( 2 * accum ) + f( b ) );

    /* Calculate the time taken for the integration. */
    t2 = clock(  );
    secs = ( ( double ) ( t2 - t1 ) ) / CLOCKS_PER_SEC;

    /* Print results. */
    printf( "Result: integral of exp( x * x ) from %.2f to %.2f: %.20e\n", a, b, I );
    printf( "Actual: integral of exp( x * x ) from %.2f to %.2f: %.20e\n", a, b, REAL );
    printf( "Relative approximation error: %.20e\n", dabs( REAL - I ) / dabs( REAL ) );
    printf( "Used %ld intervals and %d processes.\n", N_INTERV, n_procs ); 
    printf( "Time of execution in clock ticks %ld\n", t2 - t1 );
    printf( "Time of execution in seconds %f\n", secs );
    printf( "Clock resolution is %ld ticks per seconds.\n", CLOCKS_PER_SEC );

  } else { /* Other processess. */
    long   start;     /* Start of the subinterval that will be processed by this thread. */
    long   end;       /* End of the subinterval that will be processed by this thread. */
    double sum;       /* Result accumulator. */
    double xj;        /* Point inside the subinterval, interpolated linearly between start and end. */
    double t;         /* Linear interpolation point. */
    long   n_threads; /* Number of threads in the system. */
    double step;      /* The size of an interpolation step. */

    /* Receive the data sent by the master process. */
    MPI_Recv( msg, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD, &stat );
    p_id = msg[ 0 ];

    step = 1.0 / ( ( double ) N_INTERV );
    sum  = 0.0;

    /* The start of the subinterval is easy to calculate but difficult to describe. */
    start  = ( N_INTERV / ( n_procs - 1 ) ) * p_id;
    start += ( p_id == 0 ) ? 1 : 0;

    /* The end of the subinterval is the start of the subinterval corresponding to the next thread. */
    end  = ( N_INTERV / ( n_procs - 1 ) ) * ( p_id + 1 );
    end -= ( p_id == n_procs - 1 ) ? 1 : 0;

    /* The starting point for the interpolation is the first point of the starting subinterval. */
    t = start * step;

    for( start; start < end; ++start ) {
      xj = a * ( 1 - t ) + b * t;
      sum += f( xj );
      t += step;
    }

    res[ 0 ] = sum;
    MPI_Send( res, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD );
  }

  MPI_Finalize();

  return EXIT_SUCCESS;
}

/****************************************************************************************************************************
 * HELPER FUNCTIONS                                                                                                         *
 ****************************************************************************************************************************/

/* The function to integrate. */
inline double f( double x ) {
  return exp(x*x);
}

/* An absolute value function for doubles. */
inline double dabs( double x ) {
  return ( x < 0.0 ) ? -x : x;
}

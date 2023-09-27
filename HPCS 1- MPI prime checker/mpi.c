#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

int main(int argc,char **argv) {

  Args ins__args;
  parseArgs(&ins__args, &argc, argv);

  //program input argument
  long prime = ins__args.arg; 

  struct timeval ins__tstart, ins__tstop;

  int myrank,nproc;
  
  MPI_Init(&argc,&argv);

  // obtain my rank
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  // and the number of processes
  MPI_Comm_size(MPI_COMM_WORLD,&nproc);

  if(!myrank)
      gettimeofday(&ins__tstart, NULL);


  // run your computations here (including MPI communication)
 int is_prime = 1;
 
    for (int i=myrank + 2; i* i <= prime; i+=nproc)
        {
        int r = prime % i;
        if (r == 0)
        {
            is_prime = 0;
            break;
        }
    }
    
int is_prime_final=0;

    MPI_Reduce(&is_prime, &is_prime_final, 1,MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (myrank==0) {
    if (is_prime_final == nproc) {
        printf( "%ln is a prime number\n", &prime); 
    } 
    
    else {
        printf( "%ln is not a prime number\n", &prime); 
    }
    gettimeofday(&ins__tstop, NULL);
   ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
  }

MPI_Finalize();


}  
  
 

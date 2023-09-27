#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

int main(int argc, char **argv)
{

  Args ins__args;
  parseArgs(&ins__args, &argc, argv);

  // set number of threads
  omp_set_num_threads(ins__args.n_thr);

  // program input argument
  int input = ins__args.start;

  struct timeval ins__tstart, ins__tstop;
  gettimeofday(&ins__tstart, NULL);

  // run your computations here (including OpenMP stuff)

  double precision = (double)ins__args.start;
  int threadNumber = (int)ins__args.n_thr;
  double pi;
  int mine, sign;

  pi = 0;

#pragma omp parallel reduction(+ : pi)
  {
    mine = omp_get_thread_num() * 2 + 1;
    sign = (((mine - 1) / 2) % 2) ? -1 : 1;

    for (; mine < precision;)
    {
      pi += sign / (double)mine;
      mine += 2 * threadNumber;
      sign = (((mine - 1) / 2) % 2) ? -1 : 1;
    }
  }

  pi *= 4;
  printf("pi=%f", pi);

  gettimeofday(&ins__tstop, NULL);
  ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);
}

#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <sys/time.h>

	__global__
	void checkPrime(int *isPrime, long inputArgument) {

	int my_index=blockIdx.x*blockDim.x+threadIdx.x;


		if (my_index<inputArgument) {

			if(inputArgument % my_index == 0) {
				isPrime[my_index]=1;
				}
			else{
			isPrime[my_index] = 0;
			}
		}
	}


int main(int argc, char **argv) {

  Args ins__args;
  parseArgs(&ins__args, &argc, argv);
  
  //program input argument
  long inputArgument = ins__args.arg; 

  struct timeval ins__tstart, ins__tstop;
  gettimeofday(&ins__tstart, NULL);
  
  // run your CUDA kernel(s) here
	
	long isPrime = 0 ;
	
	int threadsinblock=1024;
	int blocksingrid=10000;
	
	int size = threadsinblock*blocksingrid;
	
	
	int *hisPrime=(int*)malloc(size*sizeof(int));
	
	int *disPrime=NULL;
	
	cudaMalloc((void**)&disPrime, size*sizeof(int));
	
	
	checkPrime <<<blocksingrid,threadsinblock>>> (disPrime, inputArgument);
	
	
	cudaMemcpy(hisPrime, disPrime, size*sizeof(int),cudaMemcpyDeviceToHost);
	 
	
	for(int i=0 ;i<size; i++){
	isPrime = isPrime + hisPrime[i]; 
	}
	
	free(hisPrime);
	cudaFree(disPrime);
	
    // synchronize/finalize your CUDA computations

  gettimeofday(&ins__tstop, NULL);
  ins__printtime(&ins__tstart, &ins__tstop, ins__args.marker);

	if (isPrime<=2){
		printf("%ld is a prime number. \n", inputArgument);
		}
		
  else {
		printf("%ld is not a prime number. /n",inputArgument);
		}
		
}

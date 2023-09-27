#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <time.h>

#define DATA 0
#define RESULT 1
#define FINISH 2

int main(int argc, char **argv)
{
  Args ins__args;
  parseArgs(&ins__args, &argc, argv);

  // program input argument

  long inputArgument = ins__args.arg;
  struct timeval ins__tstart, ins__tstop;

  int myrank, nproc;
  int a = 1;             // dolny zakres liczb losowanych
  int b = 1000;          // górny zakres liczb losowanych
  int E = ins__args.arg; // liczba losowanych elementów
  int range[2];          // zmienna pozwalająca na tworzenie tablicy wysyłanych liczb
  int range_size = 100;  // wielkość listy wysyłanej do każdego z procesów

  long *subcount = (long *)calloc(b + 1, sizeof(long));
  long *count = (long *)calloc(b + 1, sizeof(long));
  long *random_list = (long *)calloc(E + 1, sizeof(long));
  long *sub_list = (long *)calloc(range_size + 1, sizeof(long));

  MPI_Status status;

  MPI_Init(&argc, &argv);

  // obtain my rank
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  // and the number of processes
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  if (myrank == 0)
  {

    srand(time(0));

    for (int i = 0; i < E; i++)
    {
      random_list[i] = (rand() % (b - a + 1)) + a;
    }

    range[0] = a;

    for (int i = 1; i < nproc; i++)
    {

      range[1] = range[0] + range_size;

      // if (range[1] > E)
      //  range[1] = E;

      for (int j = 0; j < range_size; j++)
      {

        sub_list[j] = random_list[range[0] - 1 + j];
      }

      MPI_Send(sub_list, range_size, MPI_LONG, i, DATA, MPI_COMM_WORLD);

      range[0] = range[1];
    }

    do
    {
      MPI_Recv(subcount, b + 1, MPI_LONG, MPI_ANY_SOURCE, RESULT, MPI_COMM_WORLD, &status); // usunięty & z subcount

      for (int i = 0; i <= b; i++)
      {
        count[i] += subcount[i];
      }

      range[1] = range[0] + range_size;

      if (range[1] > E)
        range[1] = E;

      for (int j = 0; j < range_size; j++)
      {
        sub_list[j] = random_list[range[0] - 1 + j];
      }

      MPI_Send(sub_list, range_size, MPI_LONG, status.MPI_SOURCE, DATA, MPI_COMM_WORLD);

      range[0] = range[1];

    } while (range[1] < E);

    for (int i = 0; i < (nproc - 1); i++)
    {
      MPI_Recv(subcount, b + 1, MPI_LONG, MPI_ANY_SOURCE, RESULT, MPI_COMM_WORLD, &status);
      for (int x = 0; x < b; x++)
      {
        count[x] += subcount[x];
      }
    }

    for (int i = 1; i < nproc; i++)
    {
      MPI_Send(NULL, 0, MPI_LONG, i, FINISH, MPI_COMM_WORLD);
    }

    printf("\nQuantity diagram of the numbers in the array:\n");

    // int sum = 0;

    for (int i = 1; i <= b; i++)
    {

      if (count[i] > 0)
      {
        printf("\n liczba %d wystepuje %ld razy.", i, count[i]);
      }
    }

    // for (int i = 0; i <= b; i++)
    // {
    //  sum += count[i];
    // }

    // printf("\n sum: %d", sum);
  }

  else
  {
    do
    {
      MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      if (status.MPI_TAG == DATA)
      {
        MPI_Recv(sub_list, range_size, MPI_LONG, 0, DATA, MPI_COMM_WORLD, &status);

        for (int i = 0; i < b + 1; i++)
        {
          subcount[i] = 0;
        }

        for (int i = 0; i < range_size; i++)
        {
          subcount[sub_list[i]]++;
        }

        MPI_Send(subcount, b + 1, MPI_LONG, 0, RESULT, MPI_COMM_WORLD);
      }

    }

    while (status.MPI_TAG != FINISH);
  }

  free(subcount);
  // free(count);
  free(random_list);
  free(sub_list);

  MPI_Finalize();

  return 0;
}

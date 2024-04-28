#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
int main(int argc, char* argv[])
{
    static long num_steps = 1000000, part_steps, start = 0, end = 0;
    int rank, np, i, j;
    double step;
    double x, pi, part_sum, total_sum;
    step = 1.0 / (double)num_steps;

    /* Start up MPI */
    MPI_Init(&argc, &argv);

    /* Find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Find out number of process */
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if(rank == 0){
        part_steps = num_steps/np;
        for (i = 0; i < part_steps; i++) {
        x = ((double)(i + 0.5)) * step;
        part_sum += 4.0 / (1.0 + x * x);
      }
    }

    MPI_Bcast(&part_steps, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&start, 1, MPI_INT, 0, MPI_COMM_WORLD);
    start = part_steps * rank;

    MPI_Bcast(&end, 1, MPI_INT, 0, MPI_COMM_WORLD);
    end =  part_steps * rank + part_steps;

    if (rank == (np - 1))
    {
            end = num_steps - 1;
    }

    if(rank != 0){
      for (i = start; i < end; i++) {
        x = ((double)(i + 0.5)) * step;
        part_sum += 4.0 / (1.0 + x * x);
      }
    }

    MPI_Reduce(&part_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if(rank == 0){
        pi = step * total_sum;
        printf("\npi = ");
        printf("%.20f", pi);
        printf("\n\n");
    }

    MPI_Finalize();
    return 0;
}

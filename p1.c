#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int i, j, done = 0, n, count, localCount;
    double PI25DT = 3.141592653589793238462643;
    double pi, x, y, z;

    int numprocs, rank;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while (!done) {
        if (rank == 0) {
            printf("Enter the number of intervals: (0 quits) \n");
            scanf("%d",&n);
            for (i = 1; i < numprocs; i++)
                MPI_Send(&n, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        } else {
            MPI_Recv(&n, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }
        // if number of intervals is 0 quit
        if (n == 0) break;

        count = 0;  

        for (j = 1; j <= n; j++) {
            // Get the random numbers between 0 and 1
            x = ((double) rand()) / ((double) RAND_MAX);
            y = ((double) rand()) / ((double) RAND_MAX);

            // Calculate the square root of the squares
            z = sqrt((x*x)+(y*y));

            // Check whether z is within the circle
            if(z <= 1.0)
                count++;
        }

        if (rank > 0) {
            MPI_Send(&count, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        } else {
            for (i = 1; i < numprocs; i++) {
                MPI_Recv(&localCount, 1, MPI_DOUBLE, MPI_ANY_SOURCE,
                         MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                count += localCount;
            }

            pi = ((double) count/(double) n)*4.0;

            printf("pi is approx. %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));
        }
    }
    MPI_Finalize();
}

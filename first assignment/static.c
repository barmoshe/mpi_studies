#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "mpi.h"

#define HEAVY 1000
#define SIZE 40
#define RADIUS 10

// This function performs heavy computations,
// its run time depends on x and y values
// DO NOT change the function
double heavy(int x, int y)
{
    int i, loop;
    double sum = 0;

    if (sqrt((x - 0.75 * SIZE) * (x - 0.75 * SIZE) + (y - 0.25 * SIZE) * (y - 0.25 * SIZE)) < RADIUS)
        loop = 5 * x * y;
    else
        loop = y + x;

    for (i = 0; i < loop * HEAVY; i++)
        sum += sin(exp(cos((double)i / HEAVY)));

    return sum;
}

// STATIC
int main(int argc, char **argv)
{
    time_t t1; // not a primitive datatype
    time_t t2; // not a primitive datatype
    time(&t1);
    int x, y, rank, procAmount, source, results;
    int size = SIZE;
    double answer = 0;
    double data = 0;
    int tag = 0;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procAmount);

    results = size / procAmount;

    if (rank != 0)
    { // not master
        for (x = results * rank; x < results * (rank + 1); x++)
            for (y = 0; y < size; y++)
                answer += heavy(x, y);

        MPI_Send(&answer, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
    }
    else
    { // master
        for (source = 1; source < procAmount; source++)
        {
            MPI_Recv(&data, 1, MPI_DOUBLE, source, tag, MPI_COMM_WORLD, &status);
            answer += data;
        }
        time(&t2);
        printf("answer = %e\n", answer);
        printf("Delay = %f\n", difftime(t2, t1));
    }
    MPI_Finalize();

    return 0;
}

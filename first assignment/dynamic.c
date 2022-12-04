#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <time.h>
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

// DYNAMIC
int main(int argc, char *argv[])
{
    time_t t1; // not a primitive datatype
    time_t t2; // not a primitive datatype
    int size = SIZE;
    int x, y;
    double answer = 0;
    int rank;       // rank of process
    int procAmount; // number of processes
    int tag = 0;    // tag for messages
    int dataXY[2];
    double sum;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &procAmount);
    if (rank == 0)
    {
        time(&t1);
        for (x = 0; x < size; x++)
        {
            for (y = 0; y < size; y++)
            {
                dataXY[0] = x;
                dataXY[1] = y;
                if (x * size + y < procAmount - 1) // if less jobs then slaves
                {
                    MPI_Send(dataXY, 2, MPI_INT, x * size + y + 1, tag, MPI_COMM_WORLD); // if free send job
                }
                else // if more jobs then slave
                {
                    MPI_Recv(&sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
                    answer = answer + sum;
                    MPI_Send(dataXY, 2, MPI_INT, status.MPI_SOURCE, tag, MPI_COMM_WORLD); // send next  job to the finnished slave
                }
            }
        }
        for (int i = 1; i < procAmount; i++)
        {
            MPI_Recv(&sum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
            answer = answer + sum;
            MPI_Send(dataXY, 2, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
        }
        printf("answer = %e\n", answer);
        time(&t2);
        printf("Delay = %f\n", difftime(t2, t1));
    }
    else //  not master
    {
        do
        {
            MPI_Recv(dataXY, 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // recieve x,y from master
            x = dataXY[0];
            y = dataXY[1];
            sum = heavy(x, y); // calc sum from heavy
            if (status.MPI_TAG == 0)
                MPI_Send(&sum, 1, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
        } while (status.MPI_TAG == 0);
    }
    MPI_Finalize();
    return 0;
}

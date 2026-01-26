#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define Max(a,b) ((a)>(b)?(a):(b))
#define N (2*2*2*2*2*2+2)  
#define NH 5 

double maxeps = 0.1e-7;
int itmax = 100;

int nProcs, rank;
double (*local_A)[N][N];
int start_i, local_size, local_size_with_ghosts;

void init();
void relax(double *eps);
void verify();
void exchange_boundaries();

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int base_chunk = N / nProcs;
    int remainder = N % nProcs;
    if(base_chunk < NH) {
        if(rank==0) fprintf(stderr,"Too many processes for this problem size.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if(rank < remainder) {
        local_size = base_chunk + 1;
        start_i = rank * local_size;
    } else {
        local_size = base_chunk;
        start_i = remainder*(base_chunk+1) + (rank-remainder)*base_chunk;
    }

    local_size_with_ghosts = local_size + 2*NH;
    local_A = malloc(local_size_with_ghosts * N * N * sizeof(double));
    if(!local_A) {
        fprintf(stderr,"Process %d: Memory allocation failed!\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    init();

    double timer = MPI_Wtime();
    for(int it=1; it<=itmax; it++)
    {
        double eps=0.0;
        relax(&eps);

        double global_eps;
        MPI_Allreduce(&eps, &global_eps, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

        if(rank==0) printf("it=%4i   eps=%f\n", it, global_eps);
        if(global_eps < maxeps) break;
    }
    timer = MPI_Wtime() - timer;

    if(rank==0) printf("Execution time: %.3f sec with %d processes\n", timer, nProcs);

    verify();

    free(local_A);
    MPI_Finalize();
    return 0;
}

void init()
{
    for(int li=0; li<local_size_with_ghosts; li++)
        for(int j=0; j<N; j++)
            for(int k=0; k<N; k++)
                local_A[li][j][k] = 0.0;

    for(int li=NH; li<local_size+NH; li++)
    {
        int gi = start_i + li - NH;
        if(gi>0 && gi<N-1)
            for(int j=1; j<N-1; j++)
                for(int k=1; k<N-1; k++)
                    local_A[li][j][k] = 1. + gi + j + k;
    }
}

void exchange_boundaries()
{
    if(nProcs==1) return;
    int count = NH*N*N;
    MPI_Status status;

    if(rank>0)
        MPI_Sendrecv(&local_A[NH][0][0], count, MPI_DOUBLE, rank-1, 0,
                     &local_A[0][0][0], count, MPI_DOUBLE, rank-1, 1,
                     MPI_COMM_WORLD, &status);

    if(rank<nProcs-1)
        MPI_Sendrecv(&local_A[local_size][0][0], count, MPI_DOUBLE, rank+1, 1,
                     &local_A[local_size+NH][0][0], count, MPI_DOUBLE, rank+1, 0,
                     MPI_COMM_WORLD, &status);
}

void relax(double *eps)
{
    double local_eps = 0.0;

    for(int li=NH; li<local_size+NH; li++)
    {
        int gi = start_i + li - NH;
        if(gi>0 && gi<N-1)
        {
            for(int j=1; j<N-1; j++)
            {
                for(int k=NH; k<N-NH; k++)
                {
                    local_A[li][j][k] = (
                        local_A[li][j][k-1]+local_A[li][j][k+1]+
                        local_A[li][j][k-2]+local_A[li][j][k+2]+
                        local_A[li][j][k-3]+local_A[li][j][k+3]+
                        local_A[li][j][k-4]+local_A[li][j][k+4]+
                        local_A[li][j][k-5]+local_A[li][j][k+5]
                    )/10.;
                }
            }
        }
    }

    exchange_boundaries();

    for(int li=NH; li<local_size+NH; li++)
    {
        int gi = start_i + li - NH;
        if(gi>=NH && gi<N-NH)
        {
            for(int j=1; j<N-1; j++)
            {
                for(int k=1; k<N-1; k++)
                {
                    local_A[li][j][k] = (
                        local_A[li-1][j][k]+local_A[li+1][j][k]+
                        local_A[li-2][j][k]+local_A[li+2][j][k]+
                        local_A[li-3][j][k]+local_A[li+3][j][k]+
                        local_A[li-4][j][k]+local_A[li+4][j][k]+
                        local_A[li-5][j][k]+local_A[li+5][j][k]
                    )/10.;
                }
            }
        }
    }

    exchange_boundaries();

    for(int li=NH; li<local_size+NH; li++)
    {
        int gi = start_i + li - NH;
        if(gi>0 && gi<N-1)
        {
            for(int k=1; k<N-1; k++)
            {
                for(int j=NH; j<N-NH; j++)
                {
                    double e = local_A[li][j][k];
                    local_A[li][j][k] = (
                        local_A[li][j-1][k]+local_A[li][j+1][k]+
                        local_A[li][j-2][k]+local_A[li][j+2][k]+
                        local_A[li][j-3][k]+local_A[li][j+3][k]+
                        local_A[li][j-4][k]+local_A[li][j+4][k]+
                        local_A[li][j-5][k]+local_A[li][j+5][k]
                    )/10.;
                    local_eps = Max(local_eps, fabs(e - local_A[li][j][k]));
                }
            }
        }
    }

    *eps = local_eps;
}

void verify()
{
    double local_sum = 0.0, global_sum = 0.0;

    for(int li=NH; li<local_size+NH; li++)
    {
        int gi = start_i + li - NH;
        if(gi>=0 && gi<N)
            for(int j=0; j<N; j++)
                for(int k=0; k<N; k++)
                    local_sum += local_A[li][j][k]*(gi+1)*(j+1)*(k+1)/(N*N*N);
    }

    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if(rank==0) printf("\n  S = %f\n", global_sum);
}

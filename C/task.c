#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#define N (2*2*2*2*2*2+2)
double maxeps = 0.1e-7;
int itmax = 100;
double eps;
double A[N][N][N];
void relax();
void init();
void verify();
int main(int an, char **as)
{
    double time = omp_get_wtime();
    int it;
    init();
    for(it = 1; it <= itmax; it++)
    {
        eps = 0.;
        relax();
        if (eps < maxeps) break;
    }
    verify();
    
    time = omp_get_wtime() - time;
    int thread_count = omp_get_max_threads();
    printf("Execution time: %.3f seconds with %d threads\n", time, thread_count);
    return 0;
}
void init()
{
    #pragma omp parallel for collapse(3)
    for(int i = 0; i < N; i++)
    for(int j = 0; j < N; j++)
    for(int k = 0; k < N; k++)
    {
        if(i == 0 || i == N-1 || j == 0 || j == N-1 || k == 0 || k == N-1)
            A[i][j][k] = 0.;
        else
            A[i][j][k] = (1. + i + j + k);
    }
}
void relax()
{
    int max_threads = omp_get_max_threads();
    
    #pragma omp parallel
    #pragma omp single
    {
        int iter_per_task = (N-2) / max_threads;
        if (iter_per_task == 0) iter_per_task = 1;
        
        for (int i_start = 1; i_start < N-1; i_start += iter_per_task) {
            #pragma omp task firstprivate(i_start, iter_per_task)
            {
                int i_end = i_start + iter_per_task;
                if (i_end > N-1) i_end = N-1;
                
                for (int i = i_start; i < i_end; i++) {
                    for (int j = 1; j < N-1; j++) {
                        for (int k = 5; k < N-5; k++) {
                            A[i][j][k] = (A[i][j][k-1] + A[i][j][k+1] +
                                          A[i][j][k-2] + A[i][j][k+2] +
                                          A[i][j][k-3] + A[i][j][k+3] +
                                          A[i][j][k-4] + A[i][j][k+4] +
                                          A[i][j][k-5] + A[i][j][k+5]) * 0.1;
                        }
                    }
                }
            }
        }
    }
     #pragma omp parallel
    #pragma omp single
    {
        int iter_per_task = (N-2) / max_threads;
        if (iter_per_task == 0) iter_per_task = 1;
        
        for (int j_start = 1; j_start < N-1; j_start += iter_per_task) {
            #pragma omp task firstprivate(j_start, iter_per_task)
            {
                int j_end = j_start + iter_per_task;
                if (j_end > N-1) j_end = N-1;
                
                for (int j = j_start; j < j_end; j++) {
                    for (int k = 1; k < N-1; k++) {
                        for (int i = 5; i < N-5; i++) {
                            A[i][j][k] = (A[i-1][j][k] + A[i+1][j][k] +
                                          A[i-2][j][k] + A[i+2][j][k] +
                                          A[i-3][j][k] + A[i+3][j][k] +
                                          A[i-4][j][k] + A[i+4][j][k] +
                                          A[i-5][j][k] + A[i+5][j][k]) * 0.1;
                        }
                    }
                }
            }
        }
    }
    #pragma omp parallel
    #pragma omp single
    {
        int iter_per_task = (N-2) / max_threads;
        if (iter_per_task == 0) iter_per_task = 1;
        
        for (int i_start = 1; i_start < N-1; i_start += iter_per_task) {
            #pragma omp task firstprivate(i_start, iter_per_task)
            {
                int i_end = i_start + iter_per_task;
                if (i_end > N-1) i_end = N-1;
                
                double local_eps = 0.0;
                for (int i = i_start; i < i_end; i++) {
                    for (int k = 1; k < N-1; k++) {
                        for (int j = 5; j < N-5; j++) {
                            double e = A[i][j][k];
                            A[i][j][k] = (A[i][j-1][k] + A[i][j+1][k] +
                                          A[i][j-2][k] + A[i][j+2][k] +
                                          A[i][j-3][k] + A[i][j+3][k] +
                                          A[i][j-4][k] + A[i][j+4][k] +
                                          A[i][j-5][k] + A[i][j+5][k]) * 0.1;
                            
                            double diff = fabs(e - A[i][j][k]);
                            if (diff > local_eps) local_eps = diff;
                        }
                    }
                }
                
                             #pragma omp critical
                {
                    if (local_eps > eps) eps = local_eps;
                }
            }
        }
    }
}
void verify()
{
    double s = 0.0;
    double inv_n3 = 1.0 / (N * N * N);
    
    #pragma omp parallel for collapse(3) reduction(+:s)
    for(int i = 0; i < N; i++)
    for(int j = 0; j < N; j++)
    for(int k = 0; k < N; k++)
    {
        s += A[i][j][k] * (i+1) * (j+1) * (k+1) * inv_n3;
    }
    printf("S = %f\n", s);
}

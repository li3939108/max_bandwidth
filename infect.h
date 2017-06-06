//
// Created by chaofal on 5/29/2017.
//

#ifndef INFECT_INFECT_H


#define INFECT_INFECT_H


#include "graph.h"
#include "parameters.h"

//#define WITH_INFECTED_COUNT

#ifndef INFECT_GLOBAL_VAR
#define INFECT_GLOBAL_VAR


extern Graph *G;
extern int *seed_vertices;
extern int n_seed;
extern int Ninfected_ptr[NUM_THREADS];
extern FILE *out2;

#endif

enum objective {
    MEAN = 0,
    U_MEAN = 1,
    T_MEAN,
    UM, //U_MEAN and MEAN
    LN, //Log
    TM, //T_MEAN and MEAN
    EX, // Exponential
    VAR,
};

void stable_infect(unsigned int K, unsigned int U, enum objective obj_type);

float multithread_infect(char *out2str, unsigned U, enum objective obj_type);

int infect(Graph *G, int Nseed, int *seed, char *infected,
           reent *seedp);

void infect_dfs(Graph *G, Vertex *v, char *infected,
                reent *seedp, int *Ninfected_ptr);

void *perform_work(void *argument);

/******************************************************************************
 * @date  June 4th 2017
 * @brief print the probability density function (PDF)
 * @param fp
 * The output file
 * @param Ninfected
 * Array of total number of infected nodes for each thread run
 * @param num_threads
 * Total number of threads
 * @param V
 * Total number of nodes
 * @param w
 * length of each segment
 ******************************************************************************/
void print_pdf(FILE *fp, int Ninfected[],
               int num_threads, int V, int w);

void print_seeds(FILE *fp, int seed_vertices[], int n_seed, int column);

/******************************************************************************
 * @date June 4th 2017
 * @brief print the cumulative density function (CDF)
 * @param fp
 * The output file
 * @param Ninfected
 * Array of total number of infected nodes for each thread run
 * @param num_threads
 * Total number of threads
 * @param V
 * Total number of nodes
 * @param w
 * length of each segment
 ******************************************************************************/
void print_cdf(FILE *fp, int *Ninfected, int num_threads, int V, int w);

#endif //INFECT_INFECT_H

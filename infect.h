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

void stable_infect(unsigned int K, unsigned int U, enum objective obj_type);

float multithread_infect(char *out2str, unsigned U, enum objective obj_type);

int infect(Graph *G, int Nseed, int *seed, char *infected,
           reent *seedp);

void infect_dfs(Graph *G, Vertex *v, char *infected,
                reent *seedp, int *Ninfected_ptr);

void *perform_work(void *argument);

#endif //INFECT_INFECT_H
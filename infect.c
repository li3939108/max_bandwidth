//
// Created by chaofal on 5/29/2017.
//
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "graph.h"

#include "parameters.h"

#ifndef INFECT_GLOBAL_VAR
#define INFECT_GLOBAL_VAR

Graph *G;
int *seed_vertices = NULL;
int n_seed = DEFAULT_NUM_SEEDS;
int Ninfected_ptr[NUM_THREADS];
FILE *out2;

#endif

#include "infect.h"

void infect_dfs(Graph *G, Vertex *v, char *infected, reent *seedp,
                int *Ninfected_ptr)
{
    int j;

    if (infected[v->label] == 1)
    {
        return;
    } else
    {
        *Ninfected_ptr = *Ninfected_ptr + 1;
        infected[v->label] = 1;
    }

    for (j = 0; j < v->degree; ++j)
    {
        if (v->list[j][2] == 0
            && v->list[j][1] > getRandTo_r(MAX_EDGE_WEIGHT, seedp))
        {
            infect_dfs(G, G->adj_list[v->list[j][0]],
                       infected, seedp, Ninfected_ptr);
        }
    }
}

int infect(Graph *G, int Nseed, int *seed, char *infected, reent *seedp)
{
    int i, Ninfected = 0;
    for (i = 0; i < Nseed; ++i)
    {
        infect_dfs(G, G->adj_list[seed[i]], infected, seedp, &Ninfected);
    }
    return Ninfected;
}


void stable_infect(unsigned int K, unsigned int U, enum objective obj_type)
{

    FILE *info = stdout;
    unsigned int V = (unsigned int) G->V, initial_number_of_seed = 0, i = 0x323;
    float Ninfected_mean[V + 1];
    float obj_value = (float) (0 - 99999999999999999.9999);
    char seeds[V + 1];

    K = K < (V + 1) ? K : (V + 1);
    memset(Ninfected_mean, 0, V * sizeof *Ninfected_mean);
    memset(seeds, 0, (V + 1) * sizeof *seeds);
    seed_vertices = (int *) calloc((size_t) (V + 5), sizeof *seed_vertices);
    /*
     * Select next seed with maximum mean of infection
     */
    for (i = initial_number_of_seed; i < K; ++i)
    {
        int new_seed_label;
        float max_ = (float) -99999999999999.999;
        int max_label = 0x12121;

        n_seed = i + 1;
        fprintf(info, "  #seed: %d\n", n_seed);
        memset(Ninfected_mean, 0, (V + 1) * sizeof *Ninfected_mean);
        fprintf(info, "new seed : ");
        for (new_seed_label = 1; new_seed_label <= V; ++new_seed_label)
        {
            if (!seeds[new_seed_label])
            {
                seed_vertices[i] = new_seed_label;
                float new_ = multithread_infect("/dev/null", U, obj_type);
                Ninfected_mean[new_seed_label] = new_;
                fprintf(info, "%d:%f ", new_seed_label, new_);
                fflush(info);
                if (max_ < new_)
                {
                    max_ = new_;
                    max_label = new_seed_label;
                }
            }

        }
        fputc('\n', info);
        float th = 0.0;
        if (obj_type == MEAN)
        {
            th = (G->V / 500 > 1 ? G->V / 500 : 1);
        } else if (obj_type == U_MEAN)
        {
            th = 0.00001;
        }
        int w = G->V / (NUM_THREADS / 10);
        if (max_ - obj_value > th)
        {
            seed_vertices[i] = max_label;
            seeds[max_label] = 1;
            obj_value = max_;
            fprintf(info, "Selected new seed: %d, obj_value : %f \n",
                    max_label, max_);

            print_seeds(info, seed_vertices, n_seed, 20);
            /*
             * The mean value of infected nodes
             */
            fprintf(info, "\nThe mean of infected nodes : %f\n",
                    multithread_infect("/dev/null", 0, MEAN));
            print_distribution(info, Ninfected_ptr, NUM_THREADS,
                               G->V, w > 1 ? w : 1);
        } else
        {
            fprintf(info, "No improvement !!!! \n "
                    "------------------- \n Done !!!!");
            break;
        }
    }
}


void *perform_work(void *argument)
{

    int passed_in_value;
    passed_in_value = *((int *) argument);

    /*
     * Record whether a vertex is infected
     */
    char infected[G->V + 5];
    memset(infected, 0, (G->V + 5) * sizeof *infected);
    unsigned long seed_int = (unsigned long) (1 + passed_in_value);
#ifdef __CYGWIN__
    reent seed = (reent) (seed_int * 100 + time(NULL) % 99888);
#else
    struct drand48_data seed;
    srand48_r(seed_int, &seed);
#endif
    Ninfected_ptr[passed_in_value] =
            infect(G, n_seed, seed_vertices, infected, &seed);

    fprintf(out2, "infected[%d] : %d\n", passed_in_value,
            Ninfected_ptr[passed_in_value]);

    return NULL;
}


float multithread_infect(char *out2str, unsigned U, enum objective obj_type)
{
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int result_code;
    unsigned index, i;

    out2 = fopen(out2str, "w");
    /*
     * create all threads one by one
     */
    for (index = 0; index < NUM_THREADS; ++index)
    {
        thread_args[index] = index;
        result_code = pthread_create(&threads[index], NULL,
                                     perform_work, &thread_args[index]);
        assert(!result_code);
    }

    /*
     * wait for each thread to complete
     */
    for (index = 0; index < NUM_THREADS; ++index)
    {
        // block until thread 'index' completes
        result_code = pthread_join(threads[index], NULL);
        assert(!result_code);
    }


    /*
     * To calculate the MEAN
     */
    float sum = 0.0;
    for (i = 0; i < NUM_THREADS; ++i)
    {
        if (obj_type == U_MEAN)
        {
            sum += 1.0 - (U + 0.0) / (0.0 + Ninfected_ptr[i]);
        } else if (obj_type == MEAN)
        {
            sum += Ninfected_ptr[i];
        }
    }
    float mean = sum / ((float) NUM_THREADS);
    fprintf(out2, "sum: %f\nmean : %f \n ", sum, mean);
//    printf("\nsum: %f\nmean : %f \n ", sum, mean);
    //print_distribution(stdout, Ninfected_ptr, NUM_THREADS, G->V, 25);
    fclose(out2);
    return mean;
}


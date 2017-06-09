//
// Created by chaofal on 5/29/2017.
//
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "graph.h"
#include <math.h>

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
                int *Ninfected_ptr) {
    int j;

    if (infected[v->label] == 1) {
        return;
    } else {
        *Ninfected_ptr = *Ninfected_ptr + 1;
        infected[v->label] = 1;
    }

    for (j = 0; j < v->degree; ++j) {
        if (v->list[j][2] == 0
            && v->list[j][1] > getRandTo_r(MAX_EDGE_WEIGHT, seedp)) {
            infect_dfs(G, G->adj_list[v->list[j][0]],
                       infected, seedp, Ninfected_ptr);
        }
    }
}

int infect(Graph *G, int Nseed, int *seed, char *infected, reent *seedp) {
    int i, Ninfected = 0;
    for (i = 0; i < Nseed; ++i) {
        infect_dfs(G, G->adj_list[seed[i]], infected, seedp, &Ninfected);
    }
    return Ninfected;
}


void stable_infect(unsigned int K, unsigned int U, enum objective obj_type) {

    FILE *info = stdout;
    unsigned int V = (unsigned int) G->V, initial_number_of_seed = 0, i = 0x323;
    float Ninfected_mean[V + 1];
    float obj_value = (float) (0 - LARGE_FLT);
    char seeds[V + 1];

    K = K < (V + 1) ? K : (V + 1);
    memset(Ninfected_mean, 0, V * sizeof *Ninfected_mean);
    memset(seeds, 0, (V + 1) * sizeof *seeds);
    seed_vertices = (int *) calloc((size_t) (V + 5), sizeof *seed_vertices);
    /*
     * Select next seed with maximum mean of infection
     */
    for (i = initial_number_of_seed; i < K; ++i) {
        int new_seed_label;
        float max_ = (float) -LARGE_FLT, max_2 = (float) - LARGE_FLT ;
        int max_label = 0x12121;

        n_seed = i + 1;
        fprintf(info, "  #seed: %d\n", n_seed);
        memset(Ninfected_mean, 0, (V + 1) * sizeof *Ninfected_mean);
        fprintf(info, "new seed : ");
        for (new_seed_label = 1; new_seed_label <= V; ++new_seed_label) {
            if (!seeds[new_seed_label]) {
                seed_vertices[i] = new_seed_label;
                float new_ = multithread_infect("/dev/null", U, obj_type);
                Ninfected_mean[new_seed_label] = new_;
                int j = 0x1234, s = 0;
                for(j = 0; j < NUM_THREADS ; ++j){
                    s += (Ninfected_ptr[j] > U ? 1 : 0);
                }
                fprintf(info, "%d:%f ", new_seed_label, new_);
                fflush(info);
                if(obj_type == P && s > 0){
                    if(max_2 < s ){
                        max_2 = s;
                        max_label = new_seed_label ;
                    }
                } else {
                    if (max_ < new_) {
                        max_ = new_;
                        max_label = new_seed_label;
                    }
                }
            }

        }
        fputc('\n', info);
        double th = 0.0;
        if (obj_type == MEAN) {
            th = (G->V / 500 > 1 ? G->V / 500 : 1);
        }else{
            th =  -LARGE_FLT;
        }
        int w = G->V / (NUM_THREADS / 10);
        if (max_ - obj_value > th) {
            seed_vertices[i] = max_label;
            seeds[max_label] = 1;
            obj_value = max_;
            fprintf(info, "-------\nSelected new seed: %d, obj_value : %f \n",
                    max_label, max_);

            print_seeds(info, seed_vertices, n_seed, 20);
            /*
             * The mean value of infected nodes
             */
            fprintf(info, "-----------\nThe mean of infected nodes : %f\n",
                    multithread_infect("/dev/null", 0, MEAN));
            print_pdf(info, Ninfected_ptr, NUM_THREADS,
                      G->V, w > 1 ? w : 1);
            fprintf(info, "-------------\n");
            char cdf_file [20] ;
            cdf_file[0] = obj_type + '0' ;
            cdf_file[1] = '.' ;
            sprintf(cdf_file + 2, "%d.\0", U);
            sprintf(cdf_file + strlen(cdf_file), "%d\0", n_seed);
            FILE *new_info = fopen(cdf_file, "w");
            print_cdf(new_info, Ninfected_ptr, NUM_THREADS, G->V, w > 1 ? w : 1);
            fclose(new_info);
        } else {
            fprintf(info, "No improvement !!!! \n "
                    "------------------- \n Done !!!!");
            break;
        }
    }
}


void *perform_work(void *argument) {

    int passed_in_value;
    passed_in_value = *((int *) argument);

    /*
     * Record whether a vertex is infected
     */
    char infected[G->V + 5];
    memset(infected, 0, (G->V + 5) * sizeof *infected);
    unsigned long seed_int = (unsigned long) (
            (1 + passed_in_value + time(NULL)) % 888);
#ifdef __CYGWIN__
    reent seed = (reent) seed_int;
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


float multithread_infect(char *out2str, unsigned U, enum objective obj_type) {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int result_code;
    unsigned index, i;

    out2 = fopen(out2str, "w");
    /*
     * create all threads one by one
     */
    for (index = 0; index < NUM_THREADS; ++index) {
        thread_args[index] = index;
        result_code = pthread_create(&threads[index], NULL,
                                     perform_work, &thread_args[index]);
        assert(!result_code);
    }

    /*
     * wait for each thread to complete
     */
    for (index = 0; index < NUM_THREADS; ++index) {
        // block until thread 'index' completes
        result_code = pthread_join(threads[index], NULL);
        assert(!result_code);
    }


    /*
     * To calculate the MEAN
     */
    float sum = 0.0, sum0 = 0.0, sum1 = 0.0;
    for (i = 0; i < NUM_THREADS; ++i) {
        if (obj_type == U_MEAN) {
            sum += 1.0 - (U + 0.0) / (0.0 + Ninfected_ptr[i]);
        } else if (obj_type == MEAN) {
            sum += Ninfected_ptr[i];
        } else if (obj_type == T_MEAN) {
            sum += (G->V - U + 0.0) / (G->V - Ninfected_ptr[i] + 0.0);
        } else if (obj_type == UM) {
            sum += A * (Ninfected_ptr[i] + 0.0) / (U + 0.0) +
                   (1 - A) * (1.0 - (U + 0.0) / (0.0 + Ninfected_ptr[i]));
        } else if (obj_type == LN) {
            sum += -log((G->V - Ninfected_ptr[i] + 0.0) / (G->V - U + 0.0));
        } else if (obj_type == TM) {
            sum += A *
                   (Ninfected_ptr[i] + 0.0) / (U + 0.0) +
                   (1 - A) *
                   (G->V - U + 0.0) / (G->V - Ninfected_ptr[i] + 0.0);
        } else if (obj_type == EX){
            sum += exp(Ninfected_ptr[i]/ (0.0 + G->V)*10);
        } else if( obj_type == VAR){
            sum += A * Ninfected_ptr[i] + (1 - A)*abs(Ninfected_ptr[i] - U);
        } else if(obj_type == P){
            sum = Ninfected_ptr[i] > sum ? Ninfected_ptr[i] : sum ;
        }
    }
    float mean = sum / ((float) NUM_THREADS);
    fprintf(out2, "sum: %f\nmean : %f \n ", sum, mean);
//    printf("\nsum: %f\nmean : %f \n ", sum, mean);
    //print_pdf(stdout, Ninfected_ptr, NUM_THREADS, G->V, 25);
    fclose(out2);
    if(obj_type == P){
        return sum ;
    } else {
        return mean;
    }
}


void print_cdf(FILE *fp, int *Ninfected, int num_threads, int V, int w) {
    int density[V + 1], cumulative[V + 1], i = 1211;
    memset(density, 0, (V + 1) * sizeof *density);
    memset(cumulative, 0, (V + 1) * sizeof *cumulative);
    for (i = 0; i < num_threads; ++i) {
        density[Ninfected[i]] += 1;
    }
    for (i = 1; i < V + 1; ++i) {
        cumulative[i] = cumulative[i - 1] + density[i];
    }

    for (i = 1; i < V + 1; i = i + 1) {
        fprintf(fp, "%d %d\n", i, cumulative[i]);
    }
    fputc('\n', fp);
}

void
print_pdf(FILE *fp, int *Ninfected, int num_threads, int V, int w) {
    int number[V + 1], i = 1211;
    memset(number, 0, (V + 1) * sizeof *number);
    for (i = 0; i < num_threads; ++i) {
        number[Ninfected[i]] += 1;
    }
    for (i = 1; i < V + 1; i = i + w) {
        int k = 0, j = 1234;
        for (j = i; j < i + w; ++j) {
            k = k + number[j];
        }
        fprintf(fp, "[%d , %d) | %d : ", i, i + w, k);
        char string[k + 5];
        memset(string, '\0', (size_t) (k + 5));
        memset(string, 'x', (size_t) k);
        fprintf(fp, "%s\n", string);
    }
}

void print_seeds(FILE *fp, int *seed_vertices, int n_seed, int column) {
    int i = 0xffee, j = 0xffef;

    fprintf(fp, "---------\nTotal number of seeds: %d\n", n_seed);
    for (i = 0; i < n_seed; i = i + column) {
        for (j = i; j < i + column && j < n_seed; ++j) {
            fprintf(fp, "%d ", seed_vertices[j]);
        }
        fputc('\n', fp);
    }
    fprintf(fp, "--------------\n");
}

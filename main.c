//
// Created by li3939108 on 5/15/17.
//
#include <pthread.h>

#include "graph.h"
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <memory.h>
#include <limits.h>
#include <elf.h>
#include <sys/config.h>

//#define WITH_INFECTED_COUNT
#define NUM_THREADS     5000
#define SEED            8
#define N_SEED          1

Graph *G;

int *seed_vertices = NULL;
int n_seed = N_SEED;

int Ninfected_ptr[NUM_THREADS];
FILE *out2;

int multithread_infect(char *out2str, unsigned U, enum objective obj_type);

void infect_dfs(Graph *G, Vertex *v, char *infected,
                reent *seedp, int *Ninfected_ptr)
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

int infect(Graph *G, int Nseed, int *seed, char *infected,
           reent *seedp)
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
    int Ninfected_mean[V + 1];
    int mean = 0;
    char seeds[V + 1];

    K = K < (V + 1) ? K : (V + 1);
    memset(Ninfected_mean, 0, V * sizeof *Ninfected_mean);
    memset(seeds, 0, (V + 1) * sizeof *seeds);
    seed_vertices = calloc((size_t) (V + 5), sizeof *seed_vertices);
    /*
     * Select next seed with maximum mean of infection
     */
    for (i = initial_number_of_seed; i < K; ++i)
    {
        int new_seed_label;
        int max_ = 0;
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
                fprintf(info, "%d ", new_seed_label);
                fflush(info);
                int new_ = multithread_infect("/dev/null", 0, MEAN);
                Ninfected_mean[new_seed_label] = new_;
                if (max_ < new_)
                {
                    max_ = new_;
                    max_label = new_seed_label;
                }
            }

        }
        fputc('\n', info);
        if (max_ - mean > (G->V / 500 > 1 ? G->V / 500 : 1))
        {
            seed_vertices[i] = max_label;
            seeds[max_label] = 1;
            mean = max_;
            fprintf(info, "Selected new seed: %d, mean : %d \n",
                    max_label, max_);
        } else
        {
            fprintf(info, "No improvement !!!! \n ------------------- \n");
            /*
             * Print out the infecting result with selected initial seeds
             */
            print_seeds(info, seed_vertices, n_seed, 20);
            multithread_infect("/dev/null", U, MEAN);
            print_distribution(info, Ninfected_ptr, NUM_THREADS,
                               G->V, (G->V / 500 > 1 ? G->V / 500 : 1));
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
    struct drand48_data seed ;
    srand48_r(seed_int, &seed);
#endif
    Ninfected_ptr[passed_in_value] =
            infect(G, n_seed, seed_vertices, infected, &seed);

    fprintf(out2, "infected[%d] : %d\n", passed_in_value,
            Ninfected_ptr[passed_in_value]);

    return NULL;
}

int main(int argc, char *argv[])
{
    int j, *result, s_label, t_label, D;
    FILE *out_graph;
    struct timeval tv;
    double st, et;
    char out2str[200];

    srand((unsigned int) time(NULL));


    gettimeofday(&tv, NULL);
    st = (double) tv.tv_sec + (0.000001f * tv.tv_usec);
    gettimeofday(&tv, NULL);
    et = (double) tv.tv_sec + (0.000001f * tv.tv_usec);
    if (argc < 3)
    {
        if (argc == 2)
        {
            FILE *fp = fopen(argv[1], "r");
            G = read_graph(fp);

#ifdef WITH_INFECTED_COUNT
            strcpy(out2str, argv[1]);
            sprintf(out2str, "%s.%d", argv[1], SEED);
#else
            strcpy(out2str, "/dev/null");
#endif
        } else
        {
            printf("less arguments\n$ maxbw n r\n");
            exit(EXIT_FAILURE);
        }
    } else if (argc == 3)
    {

        /*
         * Degree of each vertex
         */
        D = atoi(argv[2]);
        /*
         * Number of Vertices
         */
        int V = atoi(argv[1]);
        G = gen(D, V);
        strcpy(out2str, "infect.raw");
    }
    if ((out_graph = fopen("./graph.raw", "w")) == NULL)
    {
        fprintf(stderr,
                "cannot open file to output the graph, use stdout instead\n");
        out_graph = stdout;
    }


    pg(G, out_graph);
    printf("\n%d-regular graph with %d vertices generated in %fs\nweights are "
                   "randomly selected between 1 to %d\n"
                   "Graph data are stored in graph.raw\n"
                   "------------------------------------------\n",
           D, G->V, et - st, MAX_EDGE_WEIGHT);


    seed_vertices = calloc((size_t) G->V, sizeof *seed_vertices);
    seed_vertices[0] = SEED;
    seed_vertices[1] = SEED + 1002;
    //multithread_infect(out2str);
    stable_infect(INT_MAX, (unsigned int) (G->V * 3 / 5), MEAN);

    fclose(out_graph);

    exit(EXIT_SUCCESS);

}

int multithread_infect(char *out2str, unsigned U, enum objective obj_type)
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
    int32_t sum = 0;
    for (i = 0; i < NUM_THREADS; ++i)
    {
        if(obj_type == U_MEAN){
            sum +=  (int) ( ( (float) U ) / ( (float)  Ninfected_ptr[i]) * 1000.0 );
        } else if (obj_type == MEAN){
            sum +=   Ninfected_ptr[i] ;
        }
    }
    float mean = ((float) sum) / ((float) NUM_THREADS);
    fprintf(out2, "sum: %f\nmean : %d \n ", sum, mean);
//    printf("sum: %ld\nmean : %d \n ", sum, mean);
    //print_distribution(stdout, Ninfected_ptr, NUM_THREADS, G->V, 25);
    fclose(out2);
    return mean;
}

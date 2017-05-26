//
// Created by li3939108 on 5/15/17.
//
#include <pthread.h>


#include "graph.h"
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <float.h>
#include <memory.h>

//#define WITH_INFECTED_COUNT
#define NUM_THREADS     5000
#define SEED            8
#define N_SEED          1

Graph *G;

int *seed_vertices = NULL;
int n_seed = N_SEED;

int Ninfected[NUM_THREADS];
FILE *out2;

int multithread_infect(char *out2str);

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

void stable_infect()
{
    FILE *info = stdout ;
    int V = G->V, initial_number_of_seed = 0, i = 0x323;
    int Ninfected_mean[V + 1];
    int mean = 0 ;
    char seeds[V + 1];

    memset(Ninfected_mean, 0, V * sizeof *Ninfected_mean);
    memset(seeds, 0, (V + 1) * sizeof *seeds);
    seed_vertices = calloc((size_t) (V + 5), sizeof *seed_vertices);
    for (i = initial_number_of_seed; i < V - 1; ++i)
    {
        int new_seed_label;
        int max_mean = 0;
        int max_mean_label = 0x12121;

        n_seed = i + 1;
        fprintf(info, "#seed: %d\n", n_seed) ;
        memset(Ninfected_mean, 0, (V + 1) * sizeof *Ninfected_mean);
        for (new_seed_label = 1; new_seed_label <= V; ++new_seed_label)
        {
            if (!seeds[new_seed_label])
            {
                seed_vertices[i] = new_seed_label;
                fprintf(info, "new_seed: %d\n", new_seed_label);

                int new_mean = multithread_infect("/dev/null");
                Ninfected_mean[new_seed_label] = new_mean;
                if (max_mean < new_mean)
                {
                    max_mean = new_mean;
                    max_mean_label = new_seed_label;
                }
            }

        }
        if (max_mean - mean > G->V / 500) {
            seed_vertices[i] = max_mean_label ;
            seeds[ max_mean_label ] = 1 ;
            mean = max_mean ;
            fprintf(info, "new seed: %d, mean : %d \n ", max_mean_label, max_mean);
        } else {
            fprintf(info, "No improvement !!!! \n");
            break ;
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
    char infected[G->V];
    memset(infected, 0, G->V * sizeof *infected);
    unsigned long seed_int = (unsigned long) (1 + passed_in_value);
#ifdef __CYGWIN__
    reent seed = (reent) (seed_int * 100 + time(NULL) % 99888);
#else
    struct drand48_data seed ;
    srand48_r(seed_int, &seed);
#endif
    Ninfected[passed_in_value] =
            infect(G, n_seed, seed_vertices, infected, &seed);

    fprintf(out2, "infected[%d] : %d\n", passed_in_value,
            Ninfected[passed_in_value]);

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
    stable_infect() ;

    fclose(out_graph);

    exit(EXIT_SUCCESS);

}

int multithread_infect(char *out2str)
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
        sum += Ninfected[i];
    }
    float mean = ( (float) sum) /  ( (float) NUM_THREADS );
    fprintf(out2, "sum: %f\nmean : %d \n ", sum, mean);
//    printf("sum: %ld\nmean : %d \n ", sum, mean);
    //print_distribution(stdout, Ninfected, NUM_THREADS, G->V, 25);
    fclose(out2);
    return mean;
}

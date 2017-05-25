//
// Created by li3939108 on 5/15/17.
//
#include <pthread.h>


#include "graph.h"
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <memory.h>

#define NUM_THREADS     1000

#define SEED 101

Graph *G;
int seed_vertices[1] = {SEED};
int Ninfected[NUM_THREADS];
FILE *out2;


void infect_dfs(Graph *G, Vertex *v, char infected[],
                unsigned int *seedp, int *Ninfected_ptr)
{
    int j;

    *Ninfected_ptr = *Ninfected_ptr + 1;
    infected[v->label] = 1;

    for (j = 0; j < v->degree; ++j)
    {
        if (v->list[j][2] == 0
            && v->list[j][1] >  getRandTo_r(MAX_EDGE_WEIGHT, seedp)
            && (!infected[v->list[j][0]]) )
        {
            infect_dfs(G, G->adj_list[v->list[j][0]],
                       infected, seedp, Ninfected_ptr);
        }
    }
}

int infect(Graph *G, int Nseed, int seed[], char infected[],
           unsigned int *seedp)
{
    int i, Ninfected = 0;
    for (i = 0; i < Nseed; ++i)
    {
        infect_dfs(G, G->adj_list[seed[i]], infected, seedp, &Ninfected);
    }
    return Ninfected;
}

void *perform_work(void *argument)
{

    int passed_in_value;
    passed_in_value = *((int *) argument);

    /*
     * Record whether a vertex is infected
     */
    char infected[G->V];
    unsigned int seed = ( 1 + passed_in_value);
    Ninfected[passed_in_value] =
            infect(G, 1, seed_vertices, infected, &seed);

    fprintf(out2, "infected[%d] : %d\n", passed_in_value,
            Ninfected[passed_in_value]);

    return NULL;
}

int main(int argc, char *argv[])
{
    int i, j, *result, s_label, t_label, D;
    FILE *out_graph;
    struct timeval tv;
    double st, et;

    srand(time(NULL));


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
            char out2str[200];
            strcpy(out2str, argv[1]);
            sprintf(out2str, "%s.%d", argv[1], SEED);
            out2 = fopen(out2str, "w");
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
        if ((out2 = fopen("./infect.raw", "w")) == NULL)
        {
            fprintf(stderr, "Cannot open file about infect");
            out2 = stderr;
        }
    }
    if ((out_graph = fopen("./graph.raw", "w")) == NULL)
    {
        fprintf(stderr,
                "cannot open file to output the graph, use stdout instead\n");
        out_graph = stdout;
    }


    pg(G, out_graph);
    printf("\n%d-regular graph with %d vertices generated in %fs\nweights are \
randomly selected between 1 to %d\nGraph data are stored in graph.raw\n \
------------------------------------------\n" ,
           D, G->V, et - st, MAX_EDGE_WEIGHT);


    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];
    int result_code;
    unsigned index;

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

    printf("In main: All threads completed successfully\n");

    /*
     * To calculate the MEAN
     */
    long sum = 0;
    for (i = 0; i < NUM_THREADS; ++i)
    {
        sum += Ninfected[i];
    }
    fprintf(out2, "sum: %d\nmean : %f \n ", sum, sum / (0.0 + NUM_THREADS));
    printf( "sum: %d\nmean : %f \n ", sum, sum / (0.0 + NUM_THREADS));
    fclose(out2);
    fclose(out_graph);

    exit(EXIT_SUCCESS);


}
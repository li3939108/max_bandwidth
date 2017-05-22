//
// Created by li3939108 on 5/15/17.
//
#include <pthread.h>

#include "graph.h"
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>

#define NUM_THREADS     500


Graph *G;
char seed_vertices[1] = {1};
int V;
FILE *out;

void *perform_work(void *argument)
{

    int passed_in_value;
    passed_in_value = *((int *) argument);

    printf("Hello World! It's me, thread with argument %d!\n",
           passed_in_value);

    /*
     * Record whether a vertex is infected
     */
    char infected[V];
    unsigned int threshold =  (1+passed_in_value );


    fprintf(out, "infected[%d] : %d\n", passed_in_value,
            infect(G, 1, seed_vertices, infected, &threshold));

    /* optionally: insert more useful stuff here */

    return NULL;
}

void infect_dfs(Graph *G, Vertex *v, char infected[],
                unsigned int *threshold, int *Ninfected_ptr)
{
    int j;

    *Ninfected_ptr = *Ninfected_ptr + 1;
    infected[v->label] = 1;

    for (j = 0; j < v->degree; ++j)
    {
        if (v->list[j][2] == 0
            && v->list[j][1] > getRandTo_r(MAX_EDGE_WEIGHT, threshold)
            && (!infected[v->list[j][0]]))
        {
            infect_dfs(G, G->adj_list[v->list[j][0]],
                       infected, threshold, Ninfected_ptr);
        }
    }
}

int infect(Graph *G, int Nseed, char seed[], char infected[],
           unsigned int *threshold)
{
    int i, Ninfected = 0;
    for (i = 0; i < Nseed; ++i)
    {
        infect_dfs(G, G->adj_list[seed[i]], infected, threshold, &Ninfected);
    }
    return Ninfected;
}

int main(int argc, char *argv[])
{
    int i, j, *result, D = 1000, s_label, t_label;

    struct timeval tv;
    double st, et;
    if (argc < 3)
    {
        if(argc == 1){
            FILE *fp = fopen("./graph.raw", "r");
            read(fp);
        } else{
            printf("less arguments\n$ maxbw n r\n");

        }
        exit(EXIT_FAILURE);
    }
    /*
     * Degree of each vertex
     */
    D = atoi(argv[2]);
    /*
     * Number of Vertices
     */
    V = atoi(argv[1]);


    srand(time(NULL));


    gettimeofday(&tv, NULL);
    st = (double) tv.tv_sec + (0.000001f * tv.tv_usec);
    G = gen(D, V);
    gettimeofday(&tv, NULL);
    et = (double) tv.tv_sec + (0.000001f * tv.tv_usec);
    if ((out = fopen("graph.raw", "w")) == NULL)
    {
        fprintf(stderr,
                "cannot open file to output the graph, use stdout instead\n");
        out = stdout;
    }
    pg(G, out);
    printf("\n%d-regular graph with %d vertices generated in %fs\nweights are randomly selected between 1 to %d\nGraph data are stored in graph.raw\n------------------------------------------\n",
           D, V, et - st, MAX_EDGE_WEIGHT);


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
        printf("In main: creating thread %d\n", index);
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
        printf("In main: thread %d has completed\n", index);
    }

    printf("In main: All threads completed successfully\n");
    exit(EXIT_SUCCESS);


}
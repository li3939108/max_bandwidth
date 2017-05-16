//
// Created by li3939108 on 5/15/17.
//
#include "graph.h"
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


void infect_dfs(Graph *G, Vertex *v, char infected[],
                int threshold, int *Ninfected_ptr)
{
    int j;

    *Ninfected_ptr = *Ninfected_ptr + 1;
    infected[v->label] = 1;

    for (j = 0; j < v->degree; ++j)
    {
        if (v->list[j][2] == 0
            && v->list[j][1] > threshold
            && (!infected[v->list[j][0]]))
        {
            infect_dfs(G, G->adj_list[v->list[j][0]],
                       infected, threshold, Ninfected_ptr);
        }
    }
}

int infect(Graph *G, int Nseed, char seed[], char infected[], int threshold)
{
    int i, Ninfected = 0;
    for (i = 0; i < Nseed; ++i)
    {
        infect_dfs(G, G->adj_list[seed[i]], infected, threshold, &Ninfected);
    }
    return Ninfected ;
}

int main(int argc, char *argv[])
{
    int i, j, *result, D = 1000, V = 5000, s_label, t_label;

    Graph *G;
    FILE *out;
    struct timeval tv;
    double st, et;
    if (argc < 3)
    {
        printf("less arguments\n$ maxbw n r\n");
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

    /*
     * Record whether a vertex is infected
     */
    char infected[V];


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

    int threshold = getRandTo(MAX_EDGE_WEIGHT);
    printf("threshold: %d\n", threshold);

    char seed[1] = {1};
    printf("infected : %d\n", infect(G, 1, seed, infected, threshold) );

}
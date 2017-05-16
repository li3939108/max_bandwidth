//
// Created by li3939108 on 5/15/17.
//
#include "graph.h"
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

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
    D = atoi(argv[2]);
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

}
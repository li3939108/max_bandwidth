//
// Created by li3939108 on 5/15/17.
//
#include <pthread.h>

#include <sys/time.h>
#include <memory.h>
#include <limits.h>
#include <stdlib.h>

#include "graph.h"
#include "infect.h"


int main(int argc, char *argv[])
{
    int D = 0;
    FILE *out_graph;
    struct timeval tv;
    double st, et;
    char out2str[200];
    enum objective objtype = MEAN;

    srand((unsigned int) time(NULL));


    gettimeofday(&tv, NULL);
    st = (double) tv.tv_sec + (0.000001f * tv.tv_usec);
    gettimeofday(&tv, NULL);
    et = (double) tv.tv_sec + (0.000001f * tv.tv_usec);
    if (argc == 3)
    {
        FILE *fp = fopen(argv[1], "r");
        G = read_graph(fp);

#ifdef WITH_INFECTED_COUNT
        strcpy(out2str, argv[1]);
        sprintf(out2str, "%s.%d", argv[1], DEFAULT_INIT_SEED);
#else
        strcpy(out2str, "/dev/null");
#endif
        switch (argv[2][0])
        {
            case 'M' :
            case 'm' :
                objtype = MEAN;
                break;
            case 'U' :
            case 'u' :
                objtype = U_MEAN;
                break;
            default:
                objtype = MEAN;
                break;
        }

    } else if (argc == 4)
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
    } else
    {
        printf("less arguments\n$ maxbw n r\n");
        exit(EXIT_FAILURE);
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
    seed_vertices[0] = DEFAULT_INIT_SEED;
    seed_vertices[1] = DEFAULT_INIT_SEED + 1002;
    //multithread_infect(out2str);
    stable_infect(INT_MAX, (unsigned int) (G->V * 4 / 5), objtype);

    fclose(out_graph);

    exit(EXIT_SUCCESS);

}


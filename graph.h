/***************************************
 April 2014 Chaofan Li <chaof@tamu.edu>
***************************************/
#ifndef __GRAPH_H__
#define __GRAPH_H__

#define MAX_EDGE_WEIGHT 5000000

#include <stdio.h>

#ifdef __CYGWIN__
typedef unsigned int reent;
#else
typedef struct drand48_data reent;
#endif

#define LIST_SIZE  3

typedef struct _Vertex {
  int degree;
  int label;
  int prob;//for use with Make-Set-Find
  float obj;
  int (*list)[LIST_SIZE];//[label, weight, direction]
  struct _Vertex *parent;//for use with Make-Set-Find
} Vertex;

typedef struct _Graph {
  int V;
  int E;
  int (*edge_list)[2]; // [label,weight]
  int (*edge_pair)[2]; //[v1, v2]
  Vertex **adj_list;
} Graph;


extern Graph *new_graph(int V, Vertex *vertex_list[]);

extern Vertex *new_vertex(int label);

extern void free_vertex(Vertex *v);

extern void free_graph(Graph *G);

extern void add_adjacency_vertex(Vertex *v, int label, int weight);

extern Graph *gen(int D, int V);

extern void pg(Graph *, FILE *fp);

extern void pv(Vertex *, FILE *fp);

extern void edges(Graph *, FILE *output);

extern int getRandTo(int Ceiling);

extern int getRandTo_r(int Ceiling, reent *seedp);

extern Graph *read_graph(FILE *fp);

#endif

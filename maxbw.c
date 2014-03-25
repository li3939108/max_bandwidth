#include "heap.h"
#include "graph.h"
#include "uf.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int *maxbw_dkt(Graph *G, int s_label, int t_label, bool use_heap) {
	Heap *h ;
	int *parent = (int *)malloc( ( G->V + 1 ) * sizeof *parent), i, j ;
	if(use_heap == false){
		//return maxbw_dkt_no_heap(G, s_label, t_label) ;
	}
	h = new_heap(G->V, 0, NULL, MAX_h) ;
	for(i = 1; i <= G->V; i++){
		Vertex *v = G->adj_list[i] ;
		insert(h, v->label, 0) ;
		parent[ v->label ] = 0;
	}
	insert(h, s_label, MAX_EDGE_WEIGHT) ;
	while( has(h, t_label) ){
		int u_label_weight[2] = {h->keys[1], pop(h, 0)} ;
		Vertex *u ;
		int parent_bandwidth = h->values[ u_label_weight[0] ] ;
		if(parent_bandwidth == 0){
			/* max bandwidth is 0 no route */
			break ;
		}
		u = G->adj_list[ u_label_weight[0] ] ;
		for(i = 0; i < u->degree; i++){
			int v_label_weight[2] = {u->list[ i ][ 0 ], u->list[ i ][ 1 ] };
			int new_bandwidth = parent_bandwidth > v_label_weight[ 1 ] ? v_label_weight[ 1 ] : parent_bandwidth ;
			if(new_bandwidth > h->values[ v_label_weight[ 0 ] ]){
				insert(h, v_label_weight[ 0 ], new_bandwidth) ;
				parent[ v_label_weight[ 0 ] ] = u_label_weight[ 0 ] ;
			}
		}
	}
	parent[0] = h->values[ t_label ] ;
	free_heap(h) ;
	return parent ;
}
/*
 *	Using Kruskal's algorithm to find the maximal bandwidth problem
 */
 int dfs(Graph *G, int s_label, int t_label, int bw, int *p){
 	int i, j;
	if( t_label == s_label ){
		return bw ;
	}
 	for(i = 0; i < G->adj_list[ s_label ]->degree; i++){
 		int *adj = (int *)G->adj_list[ s_label ]->list[ i ] ;
 		if( adj[0] == t_label){
 			return bw > adj[1] ? adj[1] : bw ;
 		}
 		if(p[ adj[0] ] == 0){
			int ret ;
 			p[adj[0] ] = s_label ;
 			ret = dfs(G, adj[0], t_label, bw > adj[1] ? adj[1] : bw, p ) ;
 			if(ret != -1){
 				return ret ;
 			}
 		}
 	}
 	return -1 ;
 }
int *maxbw_krsk(Graph *G, int s_label, int t_label){
	Heap *h ;
	Vertex **mst = (Vertex **)malloc( (G->V + 1) * sizeof *mst) ;
	int *parent = (int *)malloc( ( G->V + 1 ) * sizeof *parent), i, j,(*e)[2], (*ep)[2] ;
	edges(G, NULL) ;
	e = G->edge_list ;
	ep = G->edge_pair ;
	h = new_heap(G->E, G->E, e + 1, MAX_h) ;
	for(i = 0; i <= G->V; i++){
		mst[i] = new_vertex(i) ;
		parent[i] = 0 ;
	}
	/*
	*/
	while(h->size > 1 && find(G->adj_list[ s_label ]) != find( G->adj_list[ t_label ])){
		int key = h->keys[1] ;
		int value = pop(h, 0) ;
		Vertex *u = find( G->adj_list[ ep[ key ][0] ]) ;
		Vertex *v = find( G->adj_list[ ep[ key ][1] ]) ;
		if( u != v){
			add_adjacency_vertex(mst[ ep[key][0] ], ep[key][1], e[key][1] );
			add_adjacency_vertex(mst[ ep[key][1] ], ep[key][0], e[key][1] );
			merge(u, v) ;
		}
	}
	if(find(G->adj_list[ s_label ]) != find( G->adj_list[ t_label ]) ){
		parent [0] = 0 ;
		return parent ;
	}else{
		Graph *G_mst = new_graph(G->V, mst + 1) ;
	//	pg(G_mst) ;
		parent[ 0 ] = dfs(G_mst, s_label, t_label, MAX_EDGE_WEIGHT, parent) ;
		free(G_mst) ;
		free(mst) ;
		return parent ;
	}
}

int main(){
	int i, *result, D = 1000, V = 5000, s_label, t_label ;
	Graph *G = gen(D, V) ;
	struct timeval tv ;
	double st, et ;
	//pg(G) ;
	s_label = 1 + rand() % V ;
	t_label = 1 + rand() % V ;
	printf("dkt:\n" );
	gettimeofday(&tv, NULL);
	st =  (double)tv.tv_sec + (0.000001f * tv.tv_usec);
	result = maxbw_dkt(G, s_label, t_label, true) ;
	gettimeofday(&tv, NULL);
	et =  (double)tv.tv_sec + (0.000001f * tv.tv_usec);
	printf("\n[");
	for(i = 0; i <= 0; i++){
		printf("%d:%d ",i, result[i] ) ;
	}
	printf(", %fs]\n", et - st);
	free(result) ;

	printf("krsk:\n" );

	gettimeofday(&tv, NULL);
	st =  (double)tv.tv_sec + (0.000001f * tv.tv_usec);
	result = maxbw_krsk(G, s_label, t_label) ;
	gettimeofday(&tv, NULL);
	et =  (double)tv.tv_sec + (0.000001f * tv.tv_usec);
	printf("\n[");
	for(i = 0; i <= 0; i++){
		printf("%d:%d ",i,  result[i] ) ;
	}
	printf(", %fs]\n", et - st);
	free_graph(G) ;
	free(result) ;
	return 0 ;
}

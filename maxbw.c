#include "heap.h"
#include "graph.h"
#include "uf.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

typedef struct _node{
	int key ;
	int value ;
	struct _node *next ;
	bool in_tree ;
} node ;
int *maxbw_dkt_no_heap(Graph *G, int s_label, int t_label){
	/*
	 * Use Dijkstra's algorithm to find maximal bandwidth
	 * doesn't use the heap data structrue
	 * simple use a linked list as a set 
	 */
	int *parent = (int *)calloc((G->V + 1) , sizeof *parent), i, j ;
	Vertex *s = G->adj_list[s_label] ;
	node *head = (node *)calloc(1, sizeof *head), *new_node = NULL, *iter,
	**hash = (node **)calloc(G->V + 1, sizeof *hash);

	head->key = s_label;
	head->value = MAX_EDGE_WEIGHT ;
	head->in_tree = true ;
	hash[s_label] = head ;
	parent[s_label ] = s_label ;
	for(i = 0; i < s->degree; i++){
		new_node = calloc(1, sizeof *new_node) ;
		new_node->next = head->next ;
		head->next = new_node ;
		new_node->key = s->list[i][0] ;
		new_node->value = s->list[i][1] ;
		parent[ s->list[i][0] ] = s_label ;
		hash[ s->list[i][0] ] = new_node ;
	}
	while( ! (hash[ t_label ] && hash[t_label]->in_tree)){
		node *temp = head, *max_node=head  ;
		int max = 0, label, parent_bandwidth ;
		Vertex *u ;
		while(temp->next != NULL){
			if(temp->next->value > max){
				max = temp->next->value ;
				max_node = temp ;
			}
			temp = temp->next ;
		}//find the max

		label = max_node->next->key ;
		max_node->next->in_tree = true ;		
		//remove the max node 
		max_node->next = max_node->next->next ;
		parent_bandwidth = hash[label]->value ;
		if(parent_bandwidth == 0){
			break ;
		}
		u = G->adj_list[ label ] ;
		for( i = 0; i < u->degree; i++){
			int v_label_weight[2] = {u->list[ i ][ 0 ], u->list[ i ][ 1 ] };
			int new_bandwidth = parent_bandwidth > v_label_weight[ 1 ] ? v_label_weight[ 1 ] : parent_bandwidth ;
			if( hash[v_label_weight[0] ] && new_bandwidth > hash[ v_label_weight[ 0 ] ]->value  ){
				hash[v_label_weight[0] ]->value = new_bandwidth ;
				parent[ v_label_weight[ 0 ] ] = label ;
			}else if( hash[ v_label_weight[0] ] == NULL){
				new_node = calloc(1, sizeof *new_node) ;
				new_node->next = head->next ;
				head->next = new_node ;
				new_node->key = v_label_weight[0] ;
				new_node->value = new_bandwidth ;
				parent[ v_label_weight[0] ] = label ;
				hash[ v_label_weight[0] ] = new_node ;
			}
		}
	}
	parent[0] = hash[ t_label ]->value ;
	for(i = 0; i <= G->V; i++ ){
		free(hash[i]) ;
	}
	free(hash) ;
	return parent ;
}
int *maxbw_dkt(Graph *G, int s_label, int t_label, bool use_heap) {
	Heap *h ;
	int *parent, i, j ;
	if(use_heap == false){
		return maxbw_dkt_no_heap(G, s_label, t_label) ;
	}else{
		parent =  (int *)malloc( ( G->V + 1 ) * sizeof *parent) ;
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
/*
 *	Using Kruskal's algorithm to find the maximal bandwidth problem
 */
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
		make_set(G->adj_list[i]) ;
	}
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
		parent[ 0 ] = dfs(G_mst, s_label, t_label, MAX_EDGE_WEIGHT, parent) ;
		free_graph(G_mst) ;
		free_vertex(mst[0]) ;
		free(mst) ;
		free_heap(h) ;
		return parent ;
	}
}

int main(){
	int i,j, *result, D = 6, V = 5000, s_label, t_label ;
	Graph *G = gen(D, V) ;
	struct timeval tv ;
	double st, et ;

for(j = 0; j < 5; j++){
	s_label = 1 + rand() % V ;
	t_label = 1 + rand() % V ;
	//pg(G);
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

	printf("dkt_no_heap:\n" );
	gettimeofday(&tv, NULL);
	st =  (double)tv.tv_sec + (0.000001f * tv.tv_usec);
	result = maxbw_dkt(G, s_label, t_label, false) ;
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
	result = maxbw_krsk(G, t_label, s_label) ;
	gettimeofday(&tv, NULL);
	et =  (double)tv.tv_sec + (0.000001f * tv.tv_usec);
	printf("\n[");
	for(i = 0; i <= 0; i++){
		printf("%d:%d ",i,  result[i] ) ;
	}
	printf(", %fs]\n", et - st);
	free(result) ;

	printf("-------------------------\n");
}
	free_graph(G) ;
	return 0 ;
}

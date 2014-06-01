#include "uf.h"

void make_set(Vertex *v){
	v->parent = v ;
	v->rank = 0;
}
Vertex *find(Vertex *v){
	if(v->parent == v){
		return v;
	}else{
		return	v->parent = find(v->parent) ;
	}
}
void merge(Vertex *x, Vertex *y){
	Vertex *x_root = find(x) ;
	Vertex *y_root = find(y) ;
	if(x_root->rank > y_root->rank){
		y_root->parent = x_root ;
	}else if(x_root->rank < y_root->rank){
		x_root->parent = y_root ;
	}else if(x_root != y_root){
		y_root->parent = x_root ;
		x_root->rank = x_root->rank + 1 ;
	}
}


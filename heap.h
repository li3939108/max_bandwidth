#ifndef __HEAP_H__
#define __HEAP_H__

#define VALUE_RANGE 1000 


typedef enum _heap_type {
	MAX_h = 0, 
	MIN_h,
	UNSET_h 
} heap_t;

#ifndef __cplusplus
typedef enum _bool{
	false = 0, 
	true  
} bool ;
#endif

typedef struct _Heap{
	bool sorted ;
	int max_size ;
	int size ;
	int *keys ;
	int *values ;
	int *indices ;
	heap_t t ;
	void (*heapify)(struct _Heap *h, int index) ;
	void (*update)(struct _Heap *h, int key, int value) ;
} Heap ;


extern void sort(Heap *h) ;
extern int pop(Heap *h, int key) ;
extern void insert(Heap *h, int key, int value) ;
extern Heap *new_heap(int max_size, int size, int (*key_value_pair)[2], heap_t min_or_max) ;
extern bool has(Heap *h, int key) ;
extern void ph(Heap *h) ;
extern void free_heap(Heap *h) ;

#endif

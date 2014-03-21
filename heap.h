

#define VALUE_RANGE 1000 

typedef enum _heap_type {
	MAX_h = 1221, 
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

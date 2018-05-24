#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H


#define INIT_CAP 10
typedef struct Arraylist {
	int size; //slots used so far
	int capacity; //total available slots
	int element_size;
	void * data;
} Arraylist;
void init_arraylist(Arraylist* arraylist, int element_size);
void append(Arraylist* arraylist, void* element);
void insert(Arraylist*arraylist, void* element, int positon);
void delete(Arraylist* arraylist, int delete_position);
void freeArrayList(Arraylist* arraylist);
void* getIthElement(Arraylist*arraylist,int ith_element);
void printCharPointArraylist(Arraylist* arraylist);
void printCharArraylist(Arraylist* arraylist);
void printIntArraylist(Arraylist* arraylist);


#endif
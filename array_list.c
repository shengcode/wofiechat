#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"array_list.h"




void init_arraylist(Arraylist* arraylist, int element_size){
	arraylist->size=0;
	arraylist->capacity=INIT_CAP;
	arraylist->element_size=element_size;
	arraylist->data=malloc(arraylist->capacity*element_size);
}

void append(Arraylist* arraylist, void* element){
	if(arraylist->size==arraylist->capacity){
		void * new_location=realloc(arraylist->data, arraylist->capacity*2);
		arraylist->capacity=arraylist->capacity*2;
		arraylist->data=new_location;
	}
	memcpy(arraylist->data+arraylist->element_size*(arraylist->size), element,arraylist->element_size);
	arraylist->size=arraylist->size+1;
}

void insert(Arraylist*arraylist, void* element, int position){ // position starts from 1 not 0
	if(arraylist->size==arraylist->capacity){
		void* new_location=realloc(arraylist->data, arraylist->capacity*2);
		arraylist->capacity=arraylist->capacity*2;
		arraylist->data=new_location;
	}
	memmove(arraylist->data+(position)*arraylist->element_size, arraylist->data+(position-1)*arraylist->element_size, (arraylist->size-position+1)*arraylist->element_size );
	memmove(arraylist->data+(position-1)*arraylist->element_size,element,arraylist->element_size);
	arraylist->size=arraylist->size+1;
}

void delete(Arraylist* arraylist, int delete_position){	
	memmove(arraylist->data+(delete_position-1)*arraylist->element_size,arraylist->data+(delete_position)*arraylist->element_size, (arraylist->size-delete_position)*arraylist->element_size);
	arraylist->size--;
}

void* getIthElement(Arraylist*arraylist,int ith_element){ 
	return arraylist->data+(ith_element-1)*arraylist->element_size;
}

void freeArrayList(Arraylist* arraylist){
	free(arraylist->data);
}

void printCharArraylist(Arraylist* arraylist){
	for(int i=1;i<=arraylist->size;i++)
		printf("the %dth element is %c\n", i,*((char*)getIthElement(arraylist,i)));

}

void printCharPointArraylist(Arraylist* arraylist){
	for(int i=1;i<=arraylist->size;i++)
		printf("the %dth elment is %s\n",i,  *((char**)getIthElement(arraylist,i)));
}
void printIntArraylist(Arraylist* arraylist){
	for(int i=1;i<=arraylist->size;i++)
		printf("the %dth element is %d\n",i, *((int *)getIthElement(arraylist,i)));
}
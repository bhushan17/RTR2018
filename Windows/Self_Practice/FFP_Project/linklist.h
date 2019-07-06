#ifndef __FUN_H
#define __FUN_H

//#include<gl/GL.h>

typedef struct node
{
	const char *filename;
	struct node *prev;
	struct node *next;

}node_t;

typedef struct 
{
	node_t *head;
	node_t *tail;

}list_t;


void init(list_t *list);
void add_last(list_t *list,const char *songfile);
//void add_first(list_t *list,int data);
//void display(list_t *list);
//void sort(list_t *list);
node_t* creat_newnode(list_t *list,const char *songfile);
/*void rev_display(node_t *list);
void rev_link(list_t *list);
void delet_first(list_t *list);
void delet_last(list_t *list);
void delet_data(list_t *list, int data);

void add_circular_first(list_t *list1,int data);
void add_circular_last(list_t *list1,int data);
void display_circular(list_t *list1);*/
#endif

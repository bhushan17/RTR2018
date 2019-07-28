
//#include<stdio.h>
#include<stdlib.h>

#include"linklist.h"


void init(list_t *list)
	{
 		list->head=NULL;
		list->tail=NULL;
	}

void add_last(list_t *list,const char *songfile)
	{
		node_t *trav,*newnode=creat_newnode(list,songfile);
    
		if(list->head == NULL)
		{
			list->head = newnode;
		}
		else
		{
			trav=list->head;
			while(trav->next!=NULL)
			{
				trav=trav->next;
			}

			trav->next=newnode;
			newnode->prev=trav;

		}
	}
	
	
	node_t* creat_newnode(list_t *list,const char *songfile)
	{
		node_t *newnode = (node_t*)malloc(sizeof(node_t));
        newnode->filename=songfile;
		newnode->next=NULL;
		newnode->prev=NULL;
		return newnode;
	}

/*void add_first(list_t *list,int data)
{
  node_t *newnode=creat_newnode(list,data);

	if(list->head==NULL)
	{
		list->head=newnode;
	}

	newnode->next=list->head;
	list->head=newnode;
}

void display(list_t *list)
	{
		node_t *trav;
		if(list->head==NULL)
		{
			printf("\n\tList is empty.\n");
		}
		else
		{
			trav=list->head;
			printf("\n\tStore list is:");
			while(trav!=NULL)
			{
				printf("\t%d",trav->data);
				trav=trav->next;
			}
			printf("\n\n");
		}

	}	

void sort(list_t *list)
{
	int i,j,temp,count=0;
	node_t *trav1,*trav2;
	trav1=list->head;
	while(trav1!=NULL)
	{
		count++;
		trav1=trav1->next;
	}
	  

  for(i=0;i<count-1;i++)
	{
	  trav1=list->head;
    trav2=trav1->next;
		for(j=i+1;j<count;j++)
		{
			if(trav1->data>trav2->data)
			{
				temp=trav1->data;
			  trav1->data=trav2->data;
			  trav2->data=temp;
			}

			trav1=trav1->next;
			trav2=trav2->next;
		}
	}

}



void rev_display(node_t *trav)
{
	
	if(trav==NULL)
	{
		printf("\n\tList is empty.\n");
        return;
	}
	else
	{
  		rev_display(trav->next);
		printf("\t%d",trav->data);
	}
}

void rev_link(list_t *list)
{
	if(list->head == NULL)
	{
		return;
	}
	else
	{
		node_t *trav1,*trav2,*temp;
		trav1=list->head;
		trav2=list->head->next;
		trav1->next=NULL;

		while(trav2!=NULL)
		{
			temp=trav2->next;
			trav2->next=trav1;
			trav1=trav2;
			trav2=temp;
		}

		list->head=trav1;
	}
	
}
void delet_data(list_t *list, int val)
{
	node_t *prev, *trav, *temp;
	trav = list->head;
	prev = trav;
	int status = 1;
	
	
	if(trav==NULL)
	{
		printf("\n\tList is empty we can't delete node.\n");
	}
	else{
			while(trav != NULL)
			{
				if(trav->data == val)
				{
					if(list->head==trav)
					{
						list->head=prev->next;
						free(prev);
						prev=NULL;
						trav=list->head;
						prev=list->head;
					}
					else{
						prev->next = trav->next;
						trav->next = NULL;
						free(trav);
						trav = NULL;
						trav = prev;
						status = 0;
						
					}
				}
				else{
						prev = trav;
						trav = trav->next;
				}
			}
			if(status)
				printf(" \n\t Enter data is not present in list.");
	}
}
void delet_first(list_t *list)
{
	node_t *temp;

	if(list->head==NULL)
	{
		printf("\n\tList is empty we can't delete node.\n");
	}
	else
		if(list->head->next==NULL)
		{
			temp=list->head;
			list->head=NULL;
			free(temp);
		}
		else
		{
			temp=list->head;
			list->head=list->head->next;
			free(temp);
		}
}

void delet_last(list_t *list)
{
	node_t *temp;

	if(list->head==NULL)
	{
		printf("\n\tList is empty we can't delete node.\n");
	}
	else
		if(list->head->next==NULL)
		{
			temp=list->head;
			list->head=NULL;
			free(temp);
		}
		else
		{
			node_t *trav;
			trav=list->head;

			while(trav->next->next!=NULL)
			{
				trav=trav->next;
			}

      temp=trav->next;
			trav->next=NULL;
			free(temp);
		}

}

void add_circular_first(list_t *list1,int data)
{
	node_t *newnode=creat_newnode(list1,data);
	if(list1->head==NULL)
	{
		list1->head=newnode;
		list1->tail=newnode;
		newnode->next=newnode;
	}
	else
	{
		newnode->next=list1->head;
		list1->head=newnode;
		list1->tail->next=list1->head;
	}
}

void display_circular(list_t *list1)
{
	if(list1->head==NULL)
	{
		printf("\n\tList is empty.\n");
	}
	else
	{
		node_t *trav;
		trav=list1->head;
		printf("\n\tCircular list is.\t");
		do
		{
			printf("\t%d",trav->data);
			trav=trav->next;

		}while(trav!=list1->head);

		printf("\n\n");
	}
}
void add_circular_last(list_t *list1,int data)
{

	node_t *newnode=creat_newnode(list1,data);
	if(list1->head==NULL)
	{
	  list1->head=newnode;
		list1->tail=newnode;
		newnode->next=list1->head;
	}
	else
	{
		newnode->next=list1->head;
		list1->tail->next=newnode;
		list1->tail=newnode;

	}
}*/




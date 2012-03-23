#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "jailadmin.h"
#include "linkedlist.h"

LISTNODE *AddNode(LINKEDLIST *list, char *name, void *data, unsigned long sz)
{
	LISTNODE *node;

	if (!(list))
		return NULL;

	if (!(list->head)) {/*uninitialized list! set up head & tail nodes.*/
		list->head = malloc(sizeof(LISTNODE));
		if (!(list->head))
			return NULL;
		list->tail = list->head; /*initial degenerate case, head==tail*/
		node = list->tail;
	} else {
		list->tail->next = malloc(sizeof(LISTNODE));/*make room for the next one*/
		if (!(list->tail->next))
			return NULL;
		list->tail = list->tail->next; /*tail must always point to the last node*/
		node = list->tail; /*tail is always the last node by definition*/
	}
	memset(node, 0x00, sizeof(LISTNODE));

    /* Yeah, small memory leak here */
    if ((name))
        if (!(node->name = strdup(name)))
            return NULL;
    
	node->data = malloc(sz);
	memset(node->data, 0x00, sz);
	node->sz = sz;
	memcpy(node->data, data, sz);

	return node;
}

void DeleteNode(LINKEDLIST *list, LISTNODE *node)
{
	LISTNODE *del;

	if (!(list))
		return;

    if ((node->name))
        fprintf(stderr, "[*] Deleting node %s\n", node->name);

	if (node == list->head) {
		list->head = node->next;

        if ((node->name))
            free(node->name);

        if ((node->data))
    		free(node->data);

		free(node);

		return;
	}

	del = list->head;
	while (del != NULL && del->next != node)
		del = del->next;

	if (!(del))
		return;
	
	if(del->next == list->tail)
		list->tail = del;

	del->next = del->next->next;
	
    if ((node->name))
        free(node->name);

    if ((node->data))
    	free(node->data);

	free(node);
}

LISTNODE *FindNodeByRef(LINKEDLIST *list, void *data)
{
	LISTNODE *node;

	if (!(list))
		return NULL;

	node = list->head;
	while ((node)) {
		if (node->data == data)
			return node;

		node = node->next;
	}

	return NULL;
}

LISTNODE *FindNodeByValue(LINKEDLIST *list, void *data, unsigned long sz)
{
	LISTNODE *node;

	if (!(list))
		return NULL;

	node = list->head;
	while ((node)) {
		if (!memcmp(node->data, data, (sz > node->sz) ? node->sz : sz))
			return node;

		node = node->next;
	}

	return NULL;
}

LISTNODE *FindNodeByName(LINKEDLIST *list, char *name)
{
    LISTNODE *node;

    if (!(list))
        return NULL;

    node = list->head;
    while ((node)) {
        if ((node->name))
            if (!strcmp(node->name, name))
                return node;

        node = node->next;
    }

    return NULL;
}

/*frees all nodes in a list, including their data*/
void FreeNodes(LINKEDLIST *list, int FreeParameterAsWell)
{
	if(!(list))
		return;

	while((list->head))
        DeleteNode(list, list->head);

	if(FreeParameterAsWell) free(list);
	
	return;
}

void FreeList(LINKEDLIST *list)
{
	FreeNodes(list, 0);
	return;
}

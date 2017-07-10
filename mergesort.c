#include"bar.h"

void insertToList(Bar **head, Bar **prev, Bar *insert){
	if(*head==NULL)
		(*head) = insert;
	else
		(*prev)->next = insert;
	(*prev) = insert;
}

void divideList(Bar *head, Bar **firstPart, Bar **secondPart){
	*firstPart = NULL;
	*secondPart = NULL;
	if(!head)
		return;
	Bar *slow = head;
	Bar *fast = head->next;

	*firstPart = head;
	while(fast){
		if(fast->next)
			fast = fast->next->next;
		else
			break;
		slow = slow->next;
	}
	*secondPart = slow->next;
	slow->next = NULL;
}

Bar* sortedMerge(Bar *a, Bar*b){
	Bar *ret = NULL, *prev = NULL, *temp = NULL;
	if(a==NULL)
		return b;
	if(b==NULL)
		return a;
	while(a!=NULL && b!=NULL){
		if(a->count<b->count){
			temp = a;
			a = a->next;
		}
		else{
			temp = b;
			b = b->next;
		}
		temp->next = NULL;
		insertToList(&ret, &prev, temp);
	}
	if(a==NULL)
		prev->next = b;
	else if(b==NULL)
		prev->next = a;
	return ret;
}

void mergesort(Bar **head){
	if((*head)==NULL || (*head)->next==NULL)
		return;
	Bar *a = NULL, *b = NULL;
	//printf("\nDividing : ");
	//printList(*head);
	divideList(*head, &a, &b);
	//printf("\nDivided as : ");
	//printList(a);
	//printList(b);
	mergesort(&a);
	mergesort(&b);
	*head = sortedMerge(a, b);
	//printf("\nMerged as : ");
	//printList(*head);
}

void mergesort_ite(Bar **head){
	Bar *temp = *head;
	while(temp!=NULL && temp->next!=NULL){
		Bar *a = NULL, *b = NULL;
		divideList(temp, &a, &b);
	}
}

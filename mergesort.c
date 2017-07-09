#include"bar.h"

Bar* SortedMerge(Bar* a, Bar* b);
void FrontBackSplit(Bar* source, Bar** frontRef, Bar** backRef);

void mergesort_Bar(Bar **headRef){
	Bar* head = *headRef;
	Bar* a = NULL;
	Bar* b = NULL;

	/* Base case -- length 0 or 1 */
	if ((head == NULL) || (head->next == NULL)){
		return;
	}

	/* Split head into 'a' and 'b' sublists */
	FrontBackSplit(head, &a, &b); 

	/* Recursively sort the sublists */
	mergesort_Bar(&a);
	mergesort_Bar(&b);

	/* answer = merge the two sorted lists together */
	*headRef = SortedMerge(a, b);
}

Bar* SortedMerge(Bar* a, Bar* b){
	Bar* result = NULL;

	/* Base cases */
	if (a == NULL)
		return(b);
	else if (b==NULL)
		return(a);

	/* Pick either a or b, and recur */
	if (a->count <= b->count){
		result = a;
		result->next = SortedMerge(a->next, b);
	}
	else{
		result = b;
		result->next = SortedMerge(a, b->next);
	}
	return(result);
}

void FrontBackSplit(Bar* source, Bar** frontRef, Bar** backRef){
	Bar* fast = NULL;
	Bar* slow = NULL;
	if (source==NULL || source->next==NULL){
		/* length < 2 cases */
		*frontRef = source;
		*backRef = NULL;
	}
	else{
		slow = source;
		fast = source->next;

		/* Advance 'fast' two nodes, and advance 'slow' one node */
		while (fast != NULL){
			fast = fast->next;
			if (fast != NULL){
				slow = slow->next;
				fast = fast->next;
			}
		}

		/* 'slow' is before the midpoint in the list, so split it in two
		   at that point. */
		*frontRef = source;
		*backRef = slow->next;
		slow->next = NULL;
	}
}

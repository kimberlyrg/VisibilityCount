#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>
#include"C-Thread-Pool/thpool.h"
#include"bar.h"
#include"mergesort.c"

void checkVisibility(Bar *present, Bar *target, double *tc, double *yc){

	double ta = present->xValue;
	double ya = present->yValue;

	if(target==present->next){
		present->count++;
		target->count++;
		*tc = target->xValue;
		*yc = target->yValue;
	}
	else{
		double tb = target->xValue;
		double yb = target->yValue;
		double div = ((ya - yb) * ((tb - *tc) / (tb - ta))) + yb;

		if(*yc < div){
			present->count++;
			target->count++;

			if(yb > *yc){
				*tc = target->xValue;
				*yc = target->yValue;
			}

		}
	}

}

void checkVisibilityForBar(void *pre){
	Bar *present = (Bar *)pre;
	Bar *target = present->next;

	double tc = present->xValue;
	double yc = present->yValue;
	while(target!=NULL){
		checkVisibility(present, target, &tc, &yc);
		target = target->next;	
	}
}

void calculateVisibility(Bar *start, long long count){
	Bar *present = start;
	threadpool pool = thpool_init(4);
	while(present!=NULL){
		thpool_add_work(pool, checkVisibilityForBar, (void *)present);
		present = present->next;
	}
	thpool_wait(pool);
	thpool_destroy(pool);
}


void readFromFile(Bar **start, char *fileName, long long *count){
	FILE *f = fopen(fileName, "rb");
	if(f==NULL){
		printf("\nError : Unable to open input file %s!\n", fileName);
		exit(1);
	}
	double temp;
	Bar *tempbar, *prevbar = NULL;
	long long c = 1;
	char line[256];
	while(fgets(line, sizeof(line), f)){
		temp = atof(line);
		tempbar = (Bar *)malloc(sizeof(Bar));
		if(tempbar==NULL){
			printf("\nError : Insufficient memory!\n");
			fclose(f);
			exit(2);
		}
		tempbar->xValue = c;
		tempbar->yValue = temp;
		tempbar->count = 0;
		tempbar->next = NULL;

		if(prevbar)
			prevbar->next = tempbar;
		else
			(*start) = tempbar;

		prevbar = tempbar;
		c++;
	}
	if(!feof(f)){
		printf("\nError : Unable to reach end of file!\n");
		fclose(f);
		exit(3);
	}
	fclose(f);
	*count = c;
}

void writeToFile(Bar *start, char *fileName){
	FILE *f = fopen(fileName, "w");
	if(f==NULL){
		printf("\nError : Unable to create file!\n");
		exit(1);
	}
	int count = 1;
	while(start!=NULL){
		fprintf(f, "Bar : %3d\tValue : %12.6f\tDegree : %3d\n", count, start->yValue, start->count);
		start = start->next;
		count++;
	}
	fclose(f);
}

void printUsage(){
	printf("\n CHECK PROGRAM ARGUMENTS\n");
}

void freeBars(Bar *head){
	while(head!=NULL){
		Bar *backup = head;
		head = head->next;
		free(backup);
	}
}
void sortList(Bar *list){
	printf("\rPreparing graphs (sorting data)..");
	Bar *temp = list;
	while(temp->next!=NULL){
		Bar *t2 = temp->next;
		while(t2!=NULL){
			if(temp->count>t2->count){
				double x = temp->xValue;
				double y = temp->yValue;
				int c = temp->count;
				temp->xValue = t2->xValue;
				temp->yValue = t2->yValue;
				temp->count = t2->count;
				t2->xValue = x;
				t2->yValue = y;
				t2->count = c;
			}
			t2 = t2->next;
		}
		temp = temp->next;
	}
}
typedef struct Data{
	int degree;
	int count;
	struct Data * next;
} Data;

void calculateVisibilityFrequency(Bar *head, Data **dataHead){
	printf("\rPreparing graphs (calculating frequency)..");
	Bar *temp = head;
	Data * newHead = NULL, *prev = NULL, *aVal = NULL;
	while(temp!=NULL){
		aVal = (Data *)malloc(sizeof(Data));
		aVal->degree = temp->count;
		aVal->count = 1;
		aVal->next = NULL;
		Bar *temp2 = temp->next;
		while(temp2!=NULL && temp2->count==temp->count){
			temp2 = temp2->next;
			aVal->count = aVal->count+1;
		}
		temp = temp2;

		if(newHead==NULL)
			newHead = aVal;
		else
			prev->next = aVal;
		prev = aVal;
	}
	(*dataHead) = newHead;
}

void freeData(Data *head){
	while(head!=NULL){
		Data *backup = head;
		head = head->next;
		free(backup);
	}
}


double getDiff(clock_t start){
	return ((double) (clock() - start)) / CLOCKS_PER_SEC;
}

void plot(Bar *head, long long count){
	clock_t start = clock();
	long long totalCount = count;
	//sortList(head);
	mergesort(&head);
	Data *freqHead;
	calculateVisibilityFrequency(head, &freqHead);
	printf("\rGraphs prepared (%g seconds)..                   ", getDiff(start));
	char choice = '1';
	while(choice>'0' && choice<'4') {
		printf("\nSelect the graph you want to view : ");
		printf("\n1. log(Pk) vs log(1/k)");
		printf("\n2. log(Pk) vs k");
		printf("\n3. Pk vs k");
		printf("\nYour choice : ");
		scanf(" %c", &choice);
		if (choice == '1' || choice == '2' || choice == '3') {
#ifdef WIN32
			FILE *gnuplotPipe = _popen("gnuplot -p", "w");
#else
			FILE * gnuplotPipe = popen("gnuplot -p", "w");
#endif
			if(choice=='1') {
				fprintf(gnuplotPipe, "set ylabel \"log(Pk)\"");
				fprintf(gnuplotPipe, "\nset xlabel \"log(1/k)\"");
				fprintf(gnuplotPipe, "\nset title \"log(Pk) vs log(1/k)\"");
			}
			else if(choice=='2') {
				fprintf(gnuplotPipe, "set ylabel \"log(Pk)\"");
				fprintf(gnuplotPipe, "\nset xlabel \"k\"");
				fprintf(gnuplotPipe, "\nset title \"log(Pk) vs k\"");
			}
			else{
				fprintf(gnuplotPipe, "set ylabel \"Pk\"");
				fprintf(gnuplotPipe, "\nset xlabel \"k\"");
				fprintf(gnuplotPipe, "\nset title \"Pk vs k\"");
			}
			fprintf(gnuplotPipe, "\nplot '-' \n");
			Data *temp = freqHead;
			while (temp != NULL) {
				if(temp->degree>=6){
					float pk = (float) temp->count / totalCount;
					if(pk>0.001){
						float x = log10f((float) 1 / temp->degree);
						float logk = log10f(temp->degree);
						float logpk = log10f(pk);

						if (choice == '1')
							fprintf(gnuplotPipe, "%g %g\n", x, logpk);
						else if (choice == '2')
							fprintf(gnuplotPipe, "%d %g\n", temp->degree, logpk);
						else
							fprintf(gnuplotPipe, "%d %g\n", temp->degree, pk);
					}
				}
				temp = temp->next;
			}
			fprintf(gnuplotPipe, "e");
			fclose(gnuplotPipe);
		}
	}
	freeData(freqHead);
}

void generateRandomAndTest(int);
void generateFractalAndTest(int);

int main(int argc, char *argv[]){
	clock_t timer;
	time_t starter, end;
	double seconds;
	if((argc!=3 && argc!=4) || (argc==4 && strcmp(argv[3], "randomtest")!=0) ){
		printf("\nError : Wrong number of inputs!");
		printUsage();
		return 1;
	}
	if(strcmp(argv[1], "testrandom")==0 || strcmp(argv[1],"testfractal")==0)
	{
		if(atoi(argv[2])>=3)
		{
			if(strcmp(argv[1], "testrandom")==0){
				printf("\nGenerating %d random input values..", atoi(argv[2]));
				generateRandomAndTest(atoi(argv[2]));
			}
			else{
				printf("\nGenerating fractal input values upto %d..", atoi(argv[2]));
				if(atoi(argv[2])>50){
					printf("\nWarning : Huge number of values are to be generated!\n");
				}
				generateFractalAndTest(atoi(argv[2]));
			}
			return 0;
		}
		else
		{
			printf("\nTestdata should not be less than 3");
			exit(0);
		}
	}
	setbuf(stdout, NULL);
	printf("\nReading file..");
	Bar *start;
	long long count = 0;
	time(&starter);
	timer = clock();
	readFromFile(&start, argv[1], &count);
	seconds = getDiff(timer);
	time(&end);
	printf("\rReading completed (Execution time : %g seconds, CPU time : %g seconds)..", difftime(end, starter),seconds);

	printf("\nCalculating visibility..");
	time(&starter);
	timer = clock();
	calculateVisibility(start, count);
	seconds = getDiff(timer);
	time(&end);
	printf("\rVisibility calculated (Execution time : %g seconds, CPU time : %g seconds)..", difftime(end, starter), seconds);

	printf("\nWriting out to file..");
	time(&starter);
	timer = clock();
	writeToFile(start, argv[2]);
	seconds = getDiff(timer);
	time(&end);
	printf("\rWriting completed (Execution time : %g seconds, CPU time : %g seconds)..", difftime(end, starter), seconds);
	printf("\nPreparing graphs..");
	plot(start, count);
	printf("\nReleasing memory..");
	freeBars(start);
	if(argc==4){
		printf("\nDeleting random input file..");
		if(remove(argv[1]))
			printf("\nWarning : Unable to delete random input file!");
	}
	printf("\nCompleted successfully!\n");
	return 0;
}

char * generateRandomFileName(int isRandom){
	srand((unsigned int)time(NULL));
	char buff[28];
	time_t now = time(NULL);
	if(isRandom)
		strftime(buff, 28, "rndinpt_%Y%m%d_%H%M%S", localtime(&now));
	else
		strftime(buff, 28, "fractinpt_%Y%m%d_%H%M%S", localtime(&now));
	return strdup(buff);
}

void callMain(char *input, int isRand){
	char* args[4];
	args[0] = strdup("viscalc");
	args[1] = strdup(input);
	if(isRand)
		args[2] = strdup("testoutput_random.txt");
	else
		args[2] = strdup("testoutput_fractal.txt");
	args[3] = strdup("randomtest");
	main(4, args);
}

void generateFractalAndTest(int upto){
	int i = upto;
	int count = 0;
	int j = 1;
	int c = 1;
	int prev = 0;
	char *buff = generateRandomFileName(0);
	FILE *f = fopen(buff,"w");
	if(f==NULL){
		printf("Error : Unable to generate fractal input!");
		exit(2);
	}
	while(j<=i){
		int k = 1;
		while(k<=j*j){
			fprintf(f,"%d\n",k*k);
			k++;
		}
		j++;
	}
	/*
	   while(count<i){
	   j = (prev*prev) + c;
	   fprintf(f,"%d\n",j);
	   prev = j;
	   count++;
	   }
	   */
	fclose(f);
	callMain(buff, 0);
}

void generateRandomAndTest(int count){
	char *buff = generateRandomFileName(1);
	FILE *f = fopen(buff, "w");
	if(f==NULL){
		printf("\nError : Unable to generate random input file!");
		exit(1);
	}
	int i = 0;
	while(i<count){
		int ri = rand()%1000000;
		double rf = ri + ((double)rand()/(double)RAND_MAX);
		fprintf(f, "%lf\n", rf);
		i++;
	}
	fclose(f);
	callMain(buff, 1);
}

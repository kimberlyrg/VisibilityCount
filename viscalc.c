#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>

typedef struct Bar {
	double xValue;
	double yValue;
	int count;
	struct Bar *next;
} Bar;

void calculateVisibility(Bar *start, long long count){
	Bar *present = start;
	long long c = 0;
	int prevpercn = 0;
	while(present!=NULL){
		Bar *target = present->next;
		double ta = present->xValue;
		double ya = present->yValue;

		double tc = present->xValue;
		double yc = present->yValue;
		while(target!=NULL){
			if(target==present->next){
				present->count++;
				target->count++;
				tc = target->xValue;
				yc = target->yValue;
			}
			else{
				double tb = target->xValue;
				double yb = target->yValue;
				double div = ((ya - yb) * ((tb - tc) / (tb - ta))) + yb;

				if(yc < div){
					present->count++;
					target->count++;

					if(yb > yc){
						tc = target->xValue;
						yc = target->yValue;
					}

				}
			}
			target = target->next;
		}
		present = present->next;
		//		int percn = ((++c)*100)/count;
		//		if(percn!=prevpercn){
		//			printf("\rCalculating visibility (%d%% complete)..", percn);
		//			prevpercn = percn;
		//		}
	}
}

int startsWith(const char *prefix, const char *str){
	return strncmp(prefix, str, strlen(prefix)) == 0;
}



void writeToFile(Bar *start, char *fileName, int eventNo){
	FILE *f = fopen(fileName, "a");
	if(f==NULL){
		printf("\nError : Unable to create or open file!\n");
		exit(1);
	}
	fprintf(f, "\nEVENT NUMBER %d\n", eventNo);
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
	int totalNodes;
	struct Data * next;
} Data;

void calculateVisibilityFrequency(Bar *head, Data **dataHead, int totalNodes){
	Bar *temp = head;
	Data * newHead = NULL, *prev = NULL, *aVal = NULL;
	while(temp!=NULL){
		aVal = (Data *)malloc(sizeof(Data));
		aVal->degree = temp->count;
		aVal->count = 1;
		aVal->totalNodes = totalNodes;
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

void plot(Data *freqHead){
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
					float pk = (float) temp->count / temp->totalNodes;
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
}

void readFromFile(char *inputFileName, char *outputFileName, Data **newData){
	FILE *f = fopen(inputFileName, "rb");
	if(f==NULL){
		printf("\nError : Unable to open input file %s!\n", inputFileName);
		exit(1);
	}
	remove(outputFileName);
	double temp;
	long long c = 0;
	char line[256];
	Bar *finalFreqHead = NULL, *lastFreqHead = NULL; // Final plotting series
	printf("\n");
	int totalNodes = 0;
	while(fgets(line, sizeof(line), f)){ // Read one line
		printf("\rProcessing event %lld..", c);
		if(startsWith(" EVENT NUMBER=", line)){ // Check if it starts with EVENT NUMBER
			int i = 0;
			Bar *headbar = NULL, *tempbar = NULL, *prevbar = NULL; // Start creating new series
			printf("\rProcessing event %lld (Reading data)..         ", c);
			while(i<19){ // Process 1 value
				fgets(line, sizeof(line), f);
				temp = atof(line);
				tempbar = (Bar *)malloc(sizeof(Bar));
				if(tempbar==NULL){
					printf("\nError : Insufficient memory!\n");
					fclose(f);
					exit(2);
				}
				tempbar->xValue = i;
				tempbar->yValue = temp;
				tempbar->count = 0;
				tempbar->next = NULL;

				if(headbar)
					prevbar->next = tempbar;
				else
					headbar = tempbar;

				prevbar = tempbar;
				i++;
				totalNodes++;
			}
			printf("\rProcessing event %lld (Calculating visibility)..", c);
			calculateVisibility(headbar, i); // Find the visibility of the new series

			/* This is where the problem is happening 
			   char *e = (char *)malloc(sizeof(char)*20);
			   printf("\rProcessing event %lld (Writing to file)..       ", c);
			   sprintf(e, "_event_%lld", c);
			   char *dup = strdup(outputFileName);
			   strcat(dup, e);
			   writeToFile(headbar, dup);
			   Upto here */

			writeToFile(headbar, outputFileName, c);

			printf("\rProcessing event %lld (Finalizing result)..     ", c);
			if(finalFreqHead==NULL)
				finalFreqHead = headbar; // This is the first data series
			else{ // At the data series to the tail of the last data series
				Bar *t = lastFreqHead;
				while(t->next!=NULL)
					t = t->next;
				t->next = headbar;
			}
			lastFreqHead = headbar; // This is the last series now
			c++;
		}
	}
	if(!feof(f)){
		printf("\nError : Unable to reach end of file!\n");
		fclose(f);
		exit(3);
	}
	fclose(f);
	printf("\rProcessing events (Sorting the data)..         ");
	sortList(finalFreqHead); // Sort the new series
	printf("\rProcessing events (Calculating frequency)..    ");
	calculateVisibilityFrequency(finalFreqHead, newData, totalNodes); // Find visibility frequency of the new series
	printf("\rProcessing events (Releasing memory)..         ");
	freeBars(finalFreqHead);
	printf("\r%lld events processed (Total nodes %d).        ",c, totalNodes);
}

void generateRandomAndTest(int);
void generateFractalAndTest(int);

int main(int argc, char *argv[]){
	clock_t timer;
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
	Data * result;
	readFromFile(argv[1], argv[2], &result);
	plot(result);
	freeData(result);
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
	printf("\nError : Fractaltest is a WIP!");
	return;
	int i = upto;
	int count = 0;
	int j = 1;
	int c = 1;
	int prev = 0;
	char *buff = generateRandomFileName(0);
	FILE *f = fopen(buff,"w");
	int totalCount = 0;
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
	long totalCount = 0;
	int eventCount = 1;
	while(i<count){
		if(totalCount%20==0)
			fprintf(f, "EVENT NUMBER %d\n", eventCount++);
		int ri = rand()%1000000;
		double rf = ri + ((double)rand()/(double)RAND_MAX);
		fprintf(f, "  %lf\n", rf);
		i++;
		totalCount++;
	}
	fclose(f);
	callMain(buff, 1);
}

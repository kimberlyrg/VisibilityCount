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
		int percn = ((++c)*100)/count;
		if(percn!=prevpercn){
			printf("\rCalculating visibility (%d%% complete)..", percn);
			prevpercn = percn;
		}
	}
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
		fprintf(f, "Bar : %3d\tValue : %12.6f\tNodes : %3d\n", count, start->yValue, start->count);
		start = start->next;
		count++;
	}
	fclose(f);
}

void printUsage(){
	printf("\nUsage : viscalc input_file output_file\n");
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
				double c = temp->count;

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
	int x;
	int y;
	struct Data * next;
} Data;

void calculateVisibilityFrequency(Bar *head, Data **dataHead){
	Bar *temp = head;
	Data * newHead = NULL, *prev = NULL, *aVal = NULL;
	while(temp!=NULL){
		aVal = (Data *)malloc(sizeof(Data));
		aVal->x = temp->count;
		aVal->y = 1;
		aVal->next = NULL;
		Bar *temp2 = temp->next;
		while(temp2!=NULL && temp2->count==temp->count){
			temp2 = temp2->next;
			aVal->y = aVal->y+1;
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

void plot(Bar *head, long long count){
	sortList(head);
	Data *freqHead;
	calculateVisibilityFrequency(head, &freqHead);

	FILE * gnuplotPipe = popen("gnuplot -persistent", "w");

	fprintf(gnuplotPipe, "plot '-' \n");
	Data *temp = freqHead;
	while(temp!=NULL){
		//		printf("\nDegree : %d Nodes %d",temp->x, temp->y);
		double pk = (double)temp->y/count;
		double onebyk = (double)1/temp->x;
		//		printf("x %d y %d logx %f logpk %f\n", temp->x, temp->y, log(1/temp->x), log(pk));

		//		fprintf(gnuplotPipe, "%f %f\n", log2((double)1/temp->x), log2(pk));
		//		fprintf(gnuplotPipe, "%d %f\n", temp->x, pk);

		fprintf(gnuplotPipe, "%d %f\n", temp->x, log(pk));
		temp = temp->next;
	}
	fprintf(gnuplotPipe, "e");
	freeData(freqHead);
	//	fprintf(gnuplotPipe, " with lines");
}

void generateRandomAndTest(int);

int main(int argc, char *argv[]){
	clock_t timer;
	double seconds;
	if((argc!=3 && argc!=4) || (argc==4 && strcmp(argv[3], "randomtest")!=0) ){
		printf("\nError : Wrong number of inputs!");
		printUsage();
		return 1;
	}
	if(strcmp(argv[1], "test")==0){
		printf("\nGenerating %d random input values..", atoi(argv[2]));
		generateRandomAndTest(atoi(argv[2]));
		return 0;
	}
	setbuf(stdout, NULL);
	printf("\nReading file..");
	Bar *start;
	long long count = 0;
	timer = clock();
	readFromFile(&start, argv[1], &count);
	seconds = ((double) (clock() - timer)) / CLOCKS_PER_SEC;
	printf("\rReading completed (%g seconds)..", seconds);

	printf("\nCalculating visibility..");
	timer = clock();
	calculateVisibility(start, count);
	seconds = ((double) (clock() - timer)) / CLOCKS_PER_SEC;
	printf("\rVisibility calculated (%g seconds)..", seconds);

	printf("\nWriting out to file..");
	timer = clock();
	writeToFile(start, argv[2]);
	seconds = ((double) (clock() - timer)) / CLOCKS_PER_SEC;
	printf("\rWriting completed (%g seconds)..", seconds);

	plot(start, count);

	printf("\nReleasing memory..");
	freeBars(start);

	if(argc==4){
		printf("\nDeleting random input file..");
		if(remove(argv[1]))
			printf("\nWarning : Unable to delete random input file!");
	}

	printf("\nCompleted successfully!\n");
}


void generateRandomAndTest(int count){
	srand((unsigned int)time(NULL));
	char buff[28];
	time_t now = time(NULL);
	strftime(buff, 28, "rndinpt_%Y%m%d_%H%M%S", localtime(&now));
	FILE *f = fopen(buff, "w");
	if(f==NULL){
		printf("\nError : Unable to generate random input file!");
		exit(1);
	}
	int i = 0;
	while(i<count){
		int ri = rand()%1000000;
		double rf = ri + ((double)rand()/(double)RAND_MAX);
		fprintf(f, "%g\n", rf);
		i++;
	}
	fclose(f);
	char* args[4];
	args[0] = strdup("viscalc");
	args[1] = strdup(buff);
	args[2] = strdup("testoutput.txt");
	args[3] = strdup("randomtest");
	main(4, args);
}

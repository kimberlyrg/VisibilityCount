#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

typedef struct Bar {
	double xValue;
	double yValue;
	int count;
	struct Bar *next;
} Bar;

long long calcuateTotalIterations(long long count){
	long long ret = 0;
	while(count>1){
		ret += (count-1);
		count--;
	}
	return ret;
}

void calculateVisibility(Bar *start, long long count){
	Bar *present = start;
	long long c = 0;
	int prevpercn = 0;
	count = calcuateTotalIterations(count);
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
			int percn = ((++c)*100)/count;
			if(percn!=prevpercn){
				printf("\rCalculating visibility (%d%% complete)..", percn);
				prevpercn = percn;
			}
		}
		present = present->next;
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
	printf("\rReading took %g seconds..", seconds);

	printf("\nCalculating visibility..");
	timer = clock();
	calculateVisibility(start, count);
	seconds = ((double) (clock() - timer)) / CLOCKS_PER_SEC;
	printf("\nCalculation took %g seconds..", seconds);

	printf("\nWriting out to file..");
	timer = clock();
	writeToFile(start, argv[2]);
	seconds = ((double) (clock() - timer)) / CLOCKS_PER_SEC;
	printf("\rWriting took %g seconds..", seconds);

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

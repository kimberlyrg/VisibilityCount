#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

typedef struct Bar {
	double xValue;
	double yValue;
	int count;
} Bar;

void printBar(Bar b, int i){
	printf("\nBar %d xValue = %g yValue = %g count = %d", i, b.xValue, b.yValue, b.count);
}

void calculateVisibility(Bar *start, int count){
	int i = 0;
	while(i<count){
		Bar present = start[i];
//		printBar(present, i);
		int j = i+1;

		double ta = present.xValue;
		double ya = present.yValue;
		
		double tc = present.xValue;
		double yc = present.yValue;
		while(j<count){
			Bar target = start[j];
//			printBar(target, j);
			if(j==i+1){
				start[i].count++;	
				start[j].count++;

				tc = target.xValue;
				yc = target.yValue;
			}
			else{
				double tb = target.xValue;
				double yb = target.yValue;
				double div = ((ya - yb) * ((tb - tc) / (tb - ta))) + yb;

				if(yc < div){
					start[i].count++;
					start[j].count++;

					if(yb > yc){
						tc = target.xValue;
						yc = target.yValue;
					}
						
				}
			}
			j++;
		}
				
//		printf("\nPresent = %d", present.count);
		i++;
	}
}

typedef struct Value{
	char *string;
	struct Value *nextValue;
} Value;

void readFromFile(Bar **start, int *count, char *fileName){
	FILE *f = fopen(fileName, "rb");
	if(f==NULL){
		printf("\nError : Unable to open input file %s!\n", fileName);
		exit(1);
	}
	double temp;
	long c = 0;
	Value *head = NULL, *prev = NULL;
	char line[256];
	printf("\nParsing file..");
	while(fgets(line, sizeof(line), f)){
//		printf("\nLine : %s Head : %p Prev : %p", line, head, prev);
		Value *now = (Value *)malloc(sizeof(Value));
		now->nextValue = NULL;
		now->string = strdup(line);
		if(!head){
			head = now;
		}
		else{
			prev->nextValue = now;
		}
		prev = now;
		c++;
	}
	if(!feof(f)){
		printf("\nError : Unable to reach end of file!\n");
		fclose(f);
		exit(3);
	}
	fclose(f);
	
	(*start) = (Bar *)malloc(sizeof(Bar)*c);
	Bar *bars = (*start);
	int i = 0;
	Value *cur = head;
	while(i<c){
		temp = atof(cur->string);
//		printf("\nValue : %g", temp);
		bars[i].xValue = i+1;
		bars[i].yValue = temp;
		bars[i].count = 0;
		if(cur->nextValue!=NULL)
			cur = cur->nextValue;
		i++;
	}

	printf("\nTotal %ld values read..", c);
	(*start) = bars;
	*count = c;
}

void writeToFile(Bar *start, int count, char *fileName){
	FILE *f = fopen(fileName, "w");
	if(f==NULL){
		printf("\nError : Unable to create file!\n");
		exit(1);
	}
	int c = 0;
	while(c<count){
		Bar present = start[c];
		fprintf(f, "Bar : %3d\tValue : %12.6f\tNodes : %3d\n", (c+1), present.yValue, present.count);
		c++;
	}
	fclose(f);
}

void printUsage(){
	printf("\nUsage : viscalc input_file output_file\n");
}

void freeBars(Bar *head){
	free(head);
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
	int count;
	timer = clock();
	readFromFile(&start,&count, argv[1]);
	seconds = ((double) (clock() - timer)) / CLOCKS_PER_SEC;
	printf("\nReading took %g seconds..", seconds);

	printf("\nCalculating visibility..");
	timer = clock();
	calculateVisibility(start, count);
	seconds = ((double) (clock() - timer)) / CLOCKS_PER_SEC;
	printf("\nCalculation took %g seconds..", seconds);

	printf("\nWriting out to file..");
	timer = clock();
	writeToFile(start, count, argv[2]);
	seconds = ((double) (clock() - timer)) / CLOCKS_PER_SEC;
	printf("\nWriting took %g seconds..", seconds);

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

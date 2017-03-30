#include<stdio.h>
#include<stdlib.h>

typedef struct Bar {
	double xValue;
	double yValue;
	int count;
	struct Bar *next;
} Bar;

void calculateVisibility(Bar *start){
	Bar *present = start;
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
	}
}

void readFromFile(Bar **start, char *fileName){
	FILE *f = fopen(fileName, "rb");
	if(f==NULL){
		printf("\nError : Unable to open input file %s!\n", fileName);
		exit(1);
	}
	double temp;
	Bar *tempbar, *prevbar = NULL;
	double c = 1;
	char line[256];
	printf("\nParsing file..");
	while(fgets(line, sizeof(line), f)){
		if(line=="")
			continue;

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
	printf("\nTotal %.0f values read..", --c);
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

int main(int argc, char *argv[]){
	if(argc!=3){
		printf("\nError : Wrong number of inputs!");
		printUsage();
		return 1;
	}
	setbuf(stdout, NULL);
	printf("\nReading file..");
	Bar *start;
	readFromFile(&start, argv[1]);

	printf("\nCalculating visibility..");
	calculateVisibility(start);

	printf("\nWriting out to file..");
	writeToFile(start, argv[2]);

	printf("\nCompleted successfully!\n");
}

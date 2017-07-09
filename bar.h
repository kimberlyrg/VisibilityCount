#ifndef BAR_H
#define BAR_H
typedef struct Bar{
	double xValue;
	double yValue;
	_Atomic int count;
	struct Bar *next;
} Bar;
#endif

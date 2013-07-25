#include <stdio.h>
#include "feedback.h"

int main()
{
	int z = testNumber();
	printf("Feedback Hello %d\n", z);
	printTestString();
	return 0;
}
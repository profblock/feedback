#include <stdio.h>
#include "feedback.h"
#include "whisper.h"

int main()
{
	int z = testNumber();
	printf("Feedback Hello %d\n", z);
	printTestString();
	micSpeakerStruct ms;
	initSpeakerPos(&ms, 1,1,2);
	printf("here!\n");
	return 0;
}
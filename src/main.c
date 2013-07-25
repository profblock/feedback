#include <stdio.h>
#include "feedback.h"
#include "whisper.h"

int main()
{
	int z = testNumber();
	printf("Feedback Hello %d\n", z);
	printTestString();
	micSpeakerStruct ms;
	initSpeakerPos(&ms, 1,1,0.02);
	printf("here!\n");
	int i;
	for(i =0;i<1000;i++){
		updatePosition(&ms, 4);
		printf("\n");
	}
	return 0;
}
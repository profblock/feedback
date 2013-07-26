#include <stdio.h>
#include "feedback.h"
#include "whisper.h"

int main()
{
	int z = testNumber();
	printf("Feedback Hello %d\n", z);
	printTestString();
	initWhisperRoom(3, 2, 8, 4, 1,2000000,800000, 100000,100);
	micSpeakerStruct ms;
	initSpeakerPos(&ms, 0,1,0.02);
	printf("here!\n");
	int i;
	for(i =0;i<2540;i++){
		updatePosition(&ms, 100);
		int numberOfOperations = getNumberOfOperations(&ms);
		printf("Ops %d\n", numberOfOperations);
	}
	return 0;
}
#include <stdio.h>
#include "feedback.h"
#include "whisper.h"

int main()
{

	initWhisperRoom(3, 2, 8, 4, 1, 2000000, 800000, 0.1, 100000, 100);
	int j;
	micSpeakerStruct* ms;
	for(j = 0;j<4*8;j++){
		 ms = constructSpeakerMicPairByNumber(j);
	}
	
	int i;
	for(i =0;i<2540;i++){
		updatePosition(ms, 100);
		int numberOfOperations = getNumberOfOperations(ms);
		//printf("Ops %d\n", numberOfOperations);
	}
	return 0;
}
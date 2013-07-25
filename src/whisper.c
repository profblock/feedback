#include "whisper.h"
#include <stdio.h>

void initSpeakerPos(micSpeakerStruct* ms, int speakerNumber, int micNumber, int speed)
{
	ms->speed = speed;
	printf(" Value %d", MAX_X_WHISPER_VALUE);
}


void updatePosition(micSpeakerStruct* ms, int numOfTicks)
{
	ms->step+=numOfTicks;
}

int getNumberOfOperations(micSpeakerStruct* ms){
	return 4;
}



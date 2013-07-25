#include "whisper.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int getXSpeakerPos(micSpeakerStruct* ms){
	double xPos = cos(ms->curRadians) * ms->radius;
	return (int)xPos;
}

int getYSpeakerPos(micSpeakerStruct* ms){
	double yPos = sin(ms->curRadians) * ms->radius;
	return (int)yPos;
}

int initSpeakerPos(micSpeakerStruct* ms, int speakerNumber, int micNumber, int speed)
{
	if (NUMBER_OF_MICROPHONES !=4) {
		printf("ERROR: This only works for 4 Microphones right now");
		return 1;
	} 
		
	/* Establish the initial and current speaker Position */
	double radianOnCircle = 2*PI*((double)speakerNumber)/NUMBER_OF_SPEAKERS;
	
	ms->initRadians = radianOnCircle;
	ms->curRadians = ms->initRadians;
	ms->radius = WHISPER_SPEAKER_RADIUS;
	ms->speed = speed;
	ms->totalTics = 0;
	
	

	printf("The value is %d, %d\n", getXSpeakerPos(ms), getYSpeakerPos(ms));
	
	/* Establish the initial Microphone Position */
	
	//I know this is repetitive right now, but it won't be in the future 
	int halfRoomSize = WHISPER_ROOM_SIDE/2;
	if (NUMBER_OF_MICROPHONES==4){
		switch (micNumber)
		{
			case 0: //Upper Right
				ms->micXPos = halfRoomSize;
				ms->micYPos = halfRoomSize;
				break;
			case 1: //Upper left
				ms->micXPos = -halfRoomSize;
				ms->micYPos = halfRoomSize;
				break;
			case 2: //lower left
				ms->micXPos = -halfRoomSize;
				ms->micYPos = -halfRoomSize;
				break;
			case 3: //lower right
				ms->micXPos = halfRoomSize;
				ms->micYPos = -halfRoomSize;
				break;
			default:
				printf("Error: number of microphones is out of range\n");
				return 1;
		}
	}

	return 0;
}


void updatePosition(micSpeakerStruct* ms, int numOfTicks)
{
	ms->totalTics+=numOfTicks;
}

int getNumberOfOperations(micSpeakerStruct* ms){
	return 4;
}



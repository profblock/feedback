#include "whisper.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


//Incase I want to change how I calculate the current Radians
double getCurRadians(micSpeakerStruct* ms){
	return ms->curRadians;
}

//"private" method
int getXSpeakerPos(micSpeakerStruct* ms){
	double xPos = cos(getCurRadians(ms)) * ms->radius;
	return (int)xPos;
}

//"private" method
int getYSpeakerPos(micSpeakerStruct* ms){
	double yPos = sin(getCurRadians(ms)) * ms->radius;
	return (int)yPos;
}

//"private" method for calculating the distance between the x and y
double getMicSpeakerDistance(micSpeakerStruct* ms){
	int speakerX = getXSpeakerPos(ms);
	int speakerY = getYSpeakerPos(ms);
	int micX = ms->micXPos;
	int micY = ms->micYPos;
	
	int xDis = speakerX-micX;
	int yDis = speakerY-micY;
	
	double dist = sqrt(xDis*xDis +yDis*yDis);
	return dist;
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
	double distance = getMicSpeakerDistance(ms);
	printf("The Mic (X,Y) is (%d, %d)\n", ms->micXPos, ms->micYPos);
	printf("The distance is %f\n", distance);

	return 0;
}


void updatePosition(micSpeakerStruct* ms, int numOfTicks)
{
	ms->totalTics+=numOfTicks;
}

int getNumberOfOperations(micSpeakerStruct* ms){
	return 4;
}



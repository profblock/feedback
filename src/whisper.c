#include "whisper.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>


/****** "Private" Method Declaration, Definitions Below ********/
double getCurRadians(micSpeakerStruct* ms);
long int getXSpeakerPos(micSpeakerStruct* ms);
long int getYSpeakerPos(micSpeakerStruct* ms);
double getMicSpeakerDistanceInMeters(micSpeakerStruct* ms);



/****** "Public" Methods********/
int initSpeakerPos(micSpeakerStruct* ms, int speakerNumber, int micNumber, double speedInMetersPerSecond){
	if (NUMBER_OF_MICROPHONES !=4) {
		printf("ERROR: This only works for 4 Microphones right now");
		return 1;
	} 
		
	/* Establish the initial and current speaker Position */
	// Note M_PI is defined in math.h
	double radianOnCircle = 2*M_PI*((double)speakerNumber)/NUMBER_OF_SPEAKERS;
	
	ms->initRadians = radianOnCircle;
	ms->curRadians = ms->initRadians;
	ms->radius = WHISPER_SPEAKER_RADIUS;
	double speedInUnitsPerSecond=speedInMetersPerSecond* WHISPER_UNITS_IN_A_METER;
	ms->speedInUnitsPerTic = (long int) (speedInUnitsPerSecond/WHISPER_TICS_PER_SECOND);
	//ms->totalTics = 0;


	/* Establish the initial Microphone Position */	
	//I know this is repetitive right now, but it won't be in the future 
	long int halfRoomSize = WHISPER_ROOM_SIDE/2;
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
	double distance = getMicSpeakerDistanceInMeters(ms);
	
	printf("the Speed in Units Per Tic is %ld\n", ms->speedInUnitsPerTic);
	printf("The value is %ld, %ld\n", getXSpeakerPos(ms), getYSpeakerPos(ms));
	printf("The Mic (X,Y) is (%ld, %ld)\n", ms->micXPos, ms->micYPos);
	printf("The distance is %f\n", distance);
	
	return 0;
}


void updatePosition(micSpeakerStruct* ms, long int numOfTicks){

	long int unitsTraveledAroundArc = numOfTicks* ms->speedInUnitsPerTic;
	double fractionOfCircleTravled = ((double)unitsTraveledAroundArc)/(2* M_PI* ms->radius);
	double radiansTravled = 2*M_PI*fractionOfCircleTravled;
	ms->curRadians+=radiansTravled;	
	
	/**** Remove this later, for testing Only ****/
/*	ms->totalTics+=numOfTicks;
	unitsTraveledAroundArc = ms->totalTics* ms->speedInUnitsPerTic;
	fractionOfCircleTravled = ((double)unitsTraveledAroundArc)/(2* M_PI* ms->radius);
	radiansTravled = 2*M_PI*fractionOfCircleTravled +ms->initRadians;
	//printf("curRadians %f\ntotRadians %f\n",ms->curRadians, radiansTravled); */
}
	

int getNumberOfOperations(micSpeakerStruct* ms){
	double distanceFactor = getMicSpeakerDistanceInMeters(ms)*WHISPER_ALPHA;
	double totalComputations = WHISPER_BETA * pow(distanceFactor,2);
	return (int) totalComputations;
}


/****** "Private" Method Definition********/
//Incase I want to change how I calculate the current Radians
double getCurRadians(micSpeakerStruct* ms){
	return ms->curRadians;
}

//"private" method
long int getXSpeakerPos(micSpeakerStruct* ms){
	double xPos = cos(getCurRadians(ms)) * ms->radius;
	return (long int)xPos;
}

//"private" method
long int getYSpeakerPos(micSpeakerStruct* ms){
	double yPos = sin(getCurRadians(ms)) * ms->radius;
	return (long  int)yPos;
}

//"private" method for calculating the distance between the x and y
double getMicSpeakerDistanceInMeters(micSpeakerStruct* ms){
	long int speakerX = getXSpeakerPos(ms);
	long int speakerY = getYSpeakerPos(ms);
	long int micX = ms->micXPos;
	long int micY = ms->micYPos;
	
	long int xDis = speakerX-micX;
	long int yDis = speakerY-micY;
	
	double distInUnits = sqrt(pow(xDis,2) + pow(yDis,2));
	double distInMeters = distInUnits/WHISPER_UNITS_IN_A_METER;
	
	printf("(%ld, %ld),  Distance is Meters %f", speakerX, speakerY, distInMeters);
	return distInMeters;
}

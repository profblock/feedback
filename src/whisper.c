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

int initWhisperRoom(double alpha, double beta, int numSpeakers, int numMic, int occluding,
					long int side, long int radius, double speedInMetersPerSecond, long int unitsPerMeter, 
					long int ticksPerSecond){
	if(numMic!=4){
		printf("Error: Only supports 4 microphones currently\n");
		return 1;
	}
	SPEAKER_SPEED_IN_M_PER_SEC = speedInMetersPerSecond;
	
	NUMBER_OF_MICROPHONES = numMic;

	WHISPER_ALPHA = alpha; 
	WHISPER_BETA = beta;  
	NUMBER_OF_SPEAKERS = numSpeakers;
	
	OCCLUDING_OBJECT = occluding;
	if (!( (OCCLUDING_OBJECT==0) || (OCCLUDING_OBJECT ==1) )){
		printf("Warning: occluding should be either 0 or 1, setting to 1 and continuing");
	}
	WHISPER_ROOM_SIDE = side;
	WHISPER_SPEAKER_RADIUS = radius;
	if (side< (2*radius)){
		printf("Warning: The radius * 2 cannot be bigger than side. Setting side =2*radius and continuing");
		WHISPER_ROOM_SIDE = 2*WHISPER_SPEAKER_RADIUS;
	}
	WHISPER_UNITS_IN_A_METER = unitsPerMeter;
	WHISPER_TICS_PER_SECOND = ticksPerSecond;
}


micSpeakerStruct* constructSpeakerMicPair (int speakerNumber, int micNumber){
	micSpeakerStruct* rtnStruct = (micSpeakerStruct*) malloc(sizeof(micSpeakerStruct));
	if(rtnStruct!=0){
		int returnVal = initSpeakerMicPair(rtnStruct,speakerNumber,micNumber);
		if (returnVal==0){
			return rtnStruct;
		} else {
			printf("Error: Couldn't initialize speakerMicPair\n");
			return 0;
		}
	}
	printf("Error: Couldn't construct speakerMicPair\n");
	return 0;
}

micSpeakerStruct* constructSpeakerMicPairByNumber(int speakerPairNumber){
	int numberOfPairs = NUMBER_OF_SPEAKERS*NUMBER_OF_MICROPHONES;
	if((speakerPairNumber<0) || (speakerPairNumber>= numberOfPairs)){
		printf("Error: Number out of range\n");
		return 0;
	}
	
	int micNumber = speakerPairNumber/NUMBER_OF_SPEAKERS;
	int speakerNumber = speakerPairNumber % NUMBER_OF_SPEAKERS;
	return constructSpeakerMicPair(speakerNumber,micNumber);
}

					
int initSpeakerMicPair(micSpeakerStruct* ms, int speakerNumber, int micNumber){
	if (NUMBER_OF_MICROPHONES !=4) {
		printf("ERROR: This only works for 4 Microphones right now");
		return 1;
	} 
	
	if (NOISE_LIST==0){
		printf("NOTE: No noise list has been established. If you wish to use noise, establish that first in your code\n");
	} 
	ms->noiseList = NOISE_LIST;
	ms->lastNoiseStart = -1; //It starts at -1 So there is no noise started at 0. 
	ms->lastNoiseEnd = 0; //It ends at 0 because thats way all future noise computations can be handled correctly
	ms->quiet=1; //Starts off quiet
		
	/* Establish the initial and current speaker Position */
	// Note M_PI is defined in math.h
	double radianOnCircle = 2*M_PI*((double)speakerNumber)/NUMBER_OF_SPEAKERS;
	
	ms->initRadians = radianOnCircle;
	ms->curRadians = ms->initRadians;
	ms->radius = WHISPER_SPEAKER_RADIUS;
	
	double speedInUnitsPerSecond= SPEAKER_SPEED_IN_M_PER_SEC* WHISPER_UNITS_IN_A_METER;
	ms->speedInUnitsPerTic = (long int) (speedInUnitsPerSecond/WHISPER_TICS_PER_SECOND);
	ms->totalTics = 0;


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
	
	/****** Debugging Messages *******
	 *printf("the Speed in Units Per Tic is %ld\n", ms->speedInUnitsPerTic);
	 *printf("The value is %ld, %ld\n", getXSpeakerPos(ms), getYSpeakerPos(ms));
	 *printf("The Mic (X,Y) is (%ld, %ld)\n", ms->micXPos, ms->micYPos);
	 *printf("The distance is %f\n", distance);
	 */
	
	
	return 0;
}


void updatePosition(micSpeakerStruct* ms, long int numOfTicks){

	ms->totalTics+=numOfTicks;
	
	long int unitsTraveledAroundArc = numOfTicks* ms->speedInUnitsPerTic;
	double fractionOfCircleTravled = ((double)unitsTraveledAroundArc)/(2* M_PI* ms->radius);
	double radiansTravled = 2*M_PI*fractionOfCircleTravled;
	ms->curRadians+=radiansTravled;	
	if( (ms->noiseList!=0) && (ms->quiet==1) 
		&& (ms->totalTics >= (ms->lastNoiseEnd + ms->noiseList->noiseStartAfterLastInTics)) )
	{
		//printf("--------Starting noise at %ld\n", ms->totalTics);
		ms->quiet = 0;
		ms->lastNoiseStart = ms->lastNoiseEnd + ms->noiseList->noiseStartAfterLastInTics;
	}
	if( (ms->noiseList!=0) && (ms->quiet==0) 
		&& (ms->totalTics >= (ms->lastNoiseStart + ms->noiseList->noiseDurationInTics)) )
	{
		//printf("+++++++++ending noise at %ld\n", ms->totalTics);
		ms->quiet = 1;
		ms->lastNoiseEnd = ms->lastNoiseStart + ms->noiseList->noiseDurationInTics;
		ms->noiseList= ms->noiseList->next;
	}
	/**** Remove this later, for testing Only ****/
	/*	ms->totalTics+=numOfTicks;
	 * unitsTraveledAroundArc = ms->totalTics* ms->speedInUnitsPerTic;
	 * fractionOfCircleTravled = ((double)unitsTraveledAroundArc)/(2* M_PI* ms->radius);
	 * radiansTravled = 2*M_PI*fractionOfCircleTravled +ms->initRadians;
	 * printf("curRadians %f\ntotRadians %f\n",ms->curRadians, radiansTravled);
	 */
}
	

int getNumberOfOperations(micSpeakerStruct* ms){
	double distanceFactor = getMicSpeakerDistanceInMeters(ms)*WHISPER_ALPHA;
	double totalComputations = WHISPER_BETA * pow(distanceFactor,2);
	
	//If there is noise, the multiply the amount of computations by a factor.
	if( (ms->noiseList!=0) && (ms->quiet ==0) ) {
		totalComputations*=ms->noiseList->multiplictiveImpact;
	}
	
	return (int) totalComputations;
}

int addNoise(double startAfterSeconds, double durationInSeconds,double factor){
	noiseStruct* newNoise = (noiseStruct*)malloc(sizeof(noiseStruct));
	if(newNoise==0){
		printf("Error: Could not create noise element");
		return 1;
	}
	
	newNoise->noiseStartAfterLastInTics = (long int)(startAfterSeconds*WHISPER_TICS_PER_SECOND);
	newNoise->noiseDurationInTics = (long int)(durationInSeconds*WHISPER_TICS_PER_SECOND);
	newNoise->multiplictiveImpact=factor;
	newNoise->next = 0;
	
	if(NOISE_LIST==0){
		NOISE_LIST = newNoise;
		END_LIST = newNoise;
	} else {
		END_LIST->next = newNoise;
		END_LIST = newNoise;
	}
	return 0;
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
	
	//printf("(%ld, %ld),  Distance is Meters %f", speakerX, speakerY, distInMeters);
	return distInMeters;
}



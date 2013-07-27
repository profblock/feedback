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

int initWhisperRoom(double alpha, double beta, int numSpeakers, int numMic, int occluding, long int occludingSize,
					long int side, long int radius, double speedInMetersPerSecond, long int unitsPerMeter, 
					long int ticksPerSecond){
	if(numMic!=4){
		printf("Error: Only supports 4 microphones currently\n");
		return 1;
	}
	if (OCCLUDING_OBJECT_SIZE> radius){
		printf("ERROR: Occluding object must be less then the radius of the microphones\n");
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
	
	OCCLUDING_OBJECT_SIZE = occludingSize;
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
	if(OCCLUDING_OBJECT==1){
		OccludingPointsStruct* ops = (OccludingPointsStruct*)malloc(sizeof(OccludingPointsStruct));
		occludingPoints(ms, ops);
		if(ops->numberOfPoints==2){
			double radians = getThetaBetweenTwoPoints(ops);
			double additionalDistance = OCCLUDING_OBJECT_SIZE*radians;
			distanceFactor+=additionalDistance;
			
		} else if (ops->numberOfPoints==1){
			//Nothing happens because there is occlusion but no additional distance
		}
		//Nothing happens because there is no occlusion!
	}
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


//Value is returned in the ops datastructure. Must be created beforehand
void occludingPoints(micSpeakerStruct* ms, OccludingPointsStruct* ops){
	long int speakerX = getXSpeakerPos(ms);
	long int speakerY = getYSpeakerPos(ms);
	long int micX = ms->micXPos;
	long int micY = ms->micYPos;
	long int r = OCCLUDING_OBJECT_SIZE;
	//printf("Speaker: (%ld,%ld), Mic:(%ld, %ld), ",speakerX, speakerY, micX, micY);
	if(speakerX==micX){
		//NOTE: I haven't tested this case. But it's simple and shouldn't arrise.  
		//Special case where on the same X axis
		printf("SpecialX: UNTESTED!\n");
		double sqrTerm = pow(r,2) - pow(micX,2);
		if (sqrTerm<0) {
			//No Occlusion, nothing to see here.
			ops->numberOfPoints = 0; 
			return;
		} else if (sqrTerm==0){
			ops->numberOfPoints = 1;
			ops->x1 = micX;
			ops->y1 = 0; //Must be on the x-Axis. So It needs to be zero!
			ops->x2 = micX;
			ops->y2 = 0;
			return;
		} else {
			ops->numberOfPoints = 2;
			ops->x1 = (long int)micX;
			ops->y1 = (long int)sqrt(sqrTerm);
			ops->x2 = (long int)micX;
			ops->y2 = (long int)(-sqrt(sqrTerm));
			return;
		}			
	} else if (speakerY==micY){
		//special case where on same Y axis
		printf("SpecialY: UNTESTED\n");
		double sqrTerm = pow(r,2) - pow(micY,2);
		if (sqrTerm<0) {
			//No Occlusion, nothing to see here.
			ops->numberOfPoints = 0; 
			return;
		} else if (sqrTerm==0){
			ops->numberOfPoints = 1;
			ops->x1 = 0;//Must be on the y-Axis. So It needs to be zero!
			ops->y1 = micY; 
			ops->x2 = 0;
			ops->y2 = micY;
			return;
		} else {
			ops->numberOfPoints = 2;
			ops->x1 = (long int)sqrt(sqrTerm);
			ops->y1 = (long int)micY;
			ops->x2 = (long int)(-sqrt(sqrTerm));
			ops->y2 = (long int)micY;
			return;
		}	
	} else {
		double m = ((double)(speakerY-micY))/ (speakerX-micX);
		double c = micY - m * micX;
		double alpha = pow(m,2) + 1;
		double beta = 2*m*c;
		double gamma = pow(c,2)-pow(r,2);
		double sqrTerm = pow(beta,2) - 4*alpha*gamma;
		if (sqrTerm<0){
			//No Occlusion, nothing to see here.
			ops->numberOfPoints = 0; 
			return;
		} else if (sqrTerm==0){
			
			double xTerm = (-beta)/ (2*alpha);
			double yTerm = m*xTerm +c;
			ops->numberOfPoints = 1;
			//For saftey reason, we store the one point in both (x1,y1) and (x2,y2)
			ops->x1 = (long int)xTerm;
			ops->y1 = (long int)yTerm;
			ops->x2 = (long int)xTerm;
			ops->y2 = (long int)yTerm;
			ops->numberOfPoints = 1;
			return;

		} else {
			double xTerm1 = (-beta+ sqrt(sqrTerm))/ (2*alpha);
			double yTerm1 = m*xTerm1 +c;
			double xTerm2 = (-beta- sqrt(sqrTerm))/ (2*alpha);
			double yTerm2 = m*xTerm2 +c;
			ops->x1 = (long int)xTerm1;
			ops->y1 = (long int)yTerm1;
			ops->x2 = (long int)xTerm2;
			ops->y2 = (long int)yTerm2;
			ops->numberOfPoints = 2;
			return;
		}
	
	}
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


double getThetaBetweenTwoPoints(OccludingPointsStruct* ops){
	if (ops->numberOfPoints==2){
		double sideA = sqrt( pow(ops->x1-ops->x2,2) + pow(ops->y1-ops->y2,2));
		double sideB = sqrt( pow(ops->x2,2) + pow(ops->y2,2));
		double sideC = sqrt( pow(ops->x1,2) + pow(ops->y1,2));
		double term = (pow(sideB,2) + pow(sideC,2) - pow(sideA,2))/(2*sideB*sideC);
		double angle = acos(term);
		return angle;
	} else {
		return 0;
	}
}
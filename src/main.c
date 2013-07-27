#include <stdio.h>
#include <stdlib.h>
#include "feedback.h"
#include "whisper.h"

int main()
{

	//initWhisperRoom(3, 2, 8, 4, 1, 2000000, 800000, 1.2, 100000, 100);
	initWhisperRoom(3, 2, 8, 4, 1, 250, 2000, 500, .1, 100, 10);
	int j;
	micSpeakerStruct* ms;
	
	addNoise(0.7, 0.4, 3);
	addNoise(0.2, 0.3, 4);
	
	for(j = 0;j<4*8;j++){
		 ms = constructSpeakerMicPairByNumber(j);
	}
	
	OccludingPointsStruct* ops = (OccludingPointsStruct*)malloc(sizeof(OccludingPointsStruct));
	int i;
	for(i =0;i<2540;i++){
		updatePosition(ms, 1);
		int numberOfOperations = getNumberOfOperations(ms);
		occludingPoints(ms,ops);
		if(ops->numberOfPoints==2){
			printf("Occlusion at (%ld,%ld) and (%ld, %ld)\n", ops->x1, ops->y1, ops->x2, ops->y2);
		} else {
			//printf("No Occlusion\n");
		}
		//printf("Ops %d\n", numberOfOperations); 
	}
	return 0;
}
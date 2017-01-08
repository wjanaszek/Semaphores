#include "queue.h"
#include <stdio.h>

void queueInsert(int * queue, int value){
	queue[queue[0] + 1] = value;
	++queue[0];
	return;
}

int queuePop(int * queue){
	int result;
	result = queue[1];
	for(int i = 1; i < queue[0] + 1; i++){
		queue[i] = queue[i + 1];
	}
	--queue[0];
	return result;
}

int queueSize(int * queue){
	return queue[0];
}

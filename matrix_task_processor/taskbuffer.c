#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "taskbuffer.h"

#define MAX 200
#define BUFFSIZ 80

/* May need to be in header taskbuffer header file so tasks.c can use. */
char * tasks[MAX];
int consumerIndex = 0;
int producerIndex = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Lock shared resources among theads
pthread_cond_t fill  = PTHREAD_COND_INITIALIZER;  // Condition indicating queue is full
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; // Condition indicating queue is empty


void put(char inputBuffer[BUFFSIZ]) {
	pthread_mutex_lock(&mutex);

	//stops producting one within 5 matrices from what the consumers are processing. 
	//weird and I'm not sure if it actually works. But it does stop race conditions.
	while(producerIndex - consumerIndex > 0 || consumerIndex - producerIndex  > 5){
		pthread_cond_wait(&empty, &mutex);
	}
	strncpy(tasks[producerIndex], inputBuffer, BUFFSIZ);
	producerIndex = (producerIndex + 1) % MAX;
	pthread_cond_signal(&fill);
	pthread_mutex_unlock(&mutex);
}

char * get() {
	//needs to be freed in returning function.
	char * buffer = malloc(sizeof(char) * BUFFSIZ);
	pthread_mutex_lock(&mutex);
	while(producerIndex != consumerIndex){
		pthread_cond_wait(&fill, &mutex);
	}
	strncpy(buffer, tasks[consumerIndex], BUFFSIZ);	
	pthread_cond_signal(&empty);
	pthread_mutex_unlock(&mutex);
	return buffer;
}
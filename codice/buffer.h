#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "message.h"
#define BUFFER_ERROR (msg_t *) NULL
#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer{
	int num_messaggi;
	int N;
	int D;	//indice prod
	int T;	//indice cons
	pthread_cond_t non_pieno;
	pthread_cond_t non_vuoto;
	pthread_mutex_t USO_T;
	pthread_mutex_t USO_D;
	msg_t** cells;
}buffer_t;

/* allocazione / deallocazione buffer */

// creazione di un buffer vuoto di dim. max nota
buffer_t* buffer_init(unsigned int maxsize);

// deallocazione di un buffer
void buffer_destroy(buffer_t* buffer);

#endif

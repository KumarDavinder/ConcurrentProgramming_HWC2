#ifndef PRODUTTORECONSUMATORE_H
#define PRODUTTORECONSUMATORE_H
#include "buffer.h"

/* operazioni per creare i thread*/

typedef struct args{
	buffer_t* buffer;
	msg_t* msg;
}arguments;

void* thread_put_bloccante(void*);
void* thread_put_non_bloccante(void*);
void* thread_get_bloccante(void*);
void* thread_get_non_bloccante(void*);

#endif


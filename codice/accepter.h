#ifndef ACCEPTER_H
#define ACCEPTER_H
#include "buffer.h"
#include "list.h"
#include "poison_pill.h"

typedef struct element2{
	pthread_mutex_t list_mutex;
	list_t* readers;
}list_readers;

typedef struct element1{
	buffer_t* accepter_buffer;
	list_readers* list_readers_current;
}accepter_arg;



// richiedi la creaz. di un reader
void submitRequest(char [], list_readers*);

//inizializzazione accepter
void* accepter_init(void*); 

//estrazione messaggaio da accepter_buffer
void* accepter_buffer_get_bloccante(buffer_t*);

void list_readers_destroy(list_readers*);

#endif

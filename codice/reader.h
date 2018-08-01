#ifndef READER_H
#define READER_H
#include "buffer.h"
#include "accepter.h"
#include "list.h"
#include "poison_pill.h"

typedef struct reader{
	char* name;
	int delay;	//ritardo dei reader
	int flag_delete;	//se dispatcher trova 
	buffer_t* reader_buffer;
}reader_t;

typedef struct elem{
	reader_t* reader;
	list_readers* list_readers_current; 
}argument_thread_reader;

reader_t* reader_init(char [], int);
void* reader_thread_init(void*);
void reader_destroy(reader_t*);
void reader_update_list(list_readers*, reader_t*);


#endif

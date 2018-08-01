#ifndef DISPATCHER_H
#define DISPATCHER_H
#include "buffer.h"
#include "accepter.h"
#include "list.h"
#include "reader.h"


typedef struct element{
	list_readers* list_readers_current; 
	buffer_t* provider_buffer;
}arg_dispatcher;

void* dispatcher_init(void* args);
void send_msg_to_readers(list_readers* list_readers_current, msg_t* msg);
#endif

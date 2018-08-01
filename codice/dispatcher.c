#include "dispatcher.h"
#include "poison_pill.h"

void* dispatcher_init(void* args){
	arg_dispatcher* dispatcher = (arg_dispatcher*)args;
	list_readers* list_readers_current = dispatcher->list_readers_current; 
	buffer_t* provider_buffer = dispatcher->provider_buffer;
	msg_t* msg_read;
	while(1){
		msg_read = (msg_t*)get_bloccante(provider_buffer);
		if(msg_read == POISON_PILL){
			printf("\nDispatcher read POISON_PILL");
			send_msg_to_readers(list_readers_current, msg_read);
			//per dare tempo ai readers di suicidarsi e aggiornare la lista di readers
			int k =  size(list_readers_current->readers);
			while(k >= 0){
				if(size(list_readers_current->readers) == 0){
					pthread_exit((void*)5);
				}
			}
		}
		else{
			printf("\ndispatcher estrae il messaggio: %s", (char*)msg_read->content);
			send_msg_to_readers(list_readers_current, msg_read);
			printf("\n");
		}
	}
}


void send_msg_to_readers(list_readers* list_readers_current, msg_t* msg){
	pthread_mutex_lock(&(list_readers_current->list_mutex));
		iterator_t* iterator = iterator_init(list_readers_current->readers);
		msg_t* msgcopy;
		reader_t* reader;
		msg_t* msg_return;
		if(size(list_readers_current->readers) == 0){
			printf("\nNON CI SONO I READERS\n");
		}
		while(hasNext(iterator)){ //scansione finita?
			reader =((reader_t*)(iterator->currentNode->payload));
			if(msg == POISON_PILL)
				msgcopy = msg_copy_pill(msg);
			else
				msgcopy = msg_copy(msg);
			msg_return = (msg_t*)put_non_bloccante(reader->reader_buffer, msgcopy);
			if(msg_return) 	
				printf("\nDispatcher invia %s a %s", (char*)msg_return->content, (char*)reader->name);
			if(msg_return == BUFFER_ERROR)
				reader->flag_delete = 0;
			next(iterator);
		}
		iterator_destroy(iterator);
	pthread_mutex_unlock(&(list_readers_current->list_mutex));
}


#include "reader.h"

reader_t* reader_init(char name[], int size_buffer_reader){
	reader_t* new_reader = (reader_t*)malloc(sizeof(reader_t));
	new_reader->name = (char*)malloc(sizeof(char)*(strlen(name)+1));	
	strcpy(new_reader->name, name);
	new_reader->reader_buffer = buffer_init(size_buffer_reader);
	new_reader->flag_delete = 1;
	new_reader->delay = rand() % 3;
	return new_reader;
}

void* reader_thread_init(void* args){
	argument_thread_reader* argument = (argument_thread_reader*) args;
	reader_t* reader = argument->reader;
	list_readers* list_readers_current = argument->list_readers_current;
	buffer_t* reader_buffer = reader->reader_buffer;
	while(1){
		if(reader->flag_delete == 0){
			printf("\nFlag_deleted = 1 dispatcher kill %s\n", (char*)reader->name);
			reader_update_list(list_readers_current, reader);	//destroy richiesto dal dispatcher
			reader_destroy(reader);
			pthread_exit((void*)4);
		}
		else{
			msg_t* msg_read = (msg_t*)get_bloccante(reader_buffer);
			if(msg_read == POISON_PILL){
				printf("\n%s read POISON_PILL", (char*)reader->name);
				reader_update_list(list_readers_current, reader);
				reader_destroy(reader);
				pthread_exit((void*)3);
			}
			printf("\n%s legge il messaggio: %s",(char*)reader->name,(char*)msg_read->content);
			msg_destroy(msg_read);
			sleep(reader->delay);
		}
	}
}

void reader_destroy(reader_t* reader){
	if(reader){
		if(reader->name)
			free(reader->name);
		if(reader->reader_buffer)
			buffer_destroy(reader->reader_buffer);
		free(reader);
	}
}

void reader_update_list(list_readers* list_readers_current, reader_t* reader){
	pthread_mutex_lock(&(list_readers_current->list_mutex));
		if(removeElement(list_readers_current->readers, reader))
			printf("\nreader_destroy - update list %s\n", (char*)reader->name);
	pthread_mutex_unlock(&(list_readers_current->list_mutex));
}

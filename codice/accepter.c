#include "accepter.h"
#include "reader.h"
#include "produttoreConsumatore.h"

extern unsigned int SIZE_BUFFER_READER;

void* accepter_buffer_get_bloccante(buffer_t *accepter_buffer){
	msg_t* msg_read;
	pthread_t get_message_accepter_buffer;
	pthread_create(&get_message_accepter_buffer, NULL, thread_get_bloccante, (void*)accepter_buffer);
	pthread_join(get_message_accepter_buffer,(void*)&msg_read);
	if(msg_read == POISON_PILL){
		//msg_destroy(msg_read);
		printf("\nEstrazione di POISON_PILL da accepter_buffer\n");
		pthread_exit((void*)6);
	}
	return msg_read;
}

void* accepter_init(void* args){
	accepter_arg* accepter = (accepter_arg*)args;
	buffer_t* accepter_buffer = accepter->accepter_buffer;
	list_readers* list_readers_current = accepter->list_readers_current;
	msg_t* msg_read1;
	while(1){
		msg_read1 = accepter_buffer_get_bloccante(accepter_buffer); //estraggo il messaggio
		printf("\naccepter estrae il messaggio %s", (char*)msg_read1->content);
		submitRequest(msg_read1->content, list_readers_current); //creo reader e aggiorno la lista
		msg_destroy(msg_read1);
	}
}

void submitRequest(char name[], list_readers* list_readers_current){
	reader_t* new_reader = reader_init(name, SIZE_BUFFER_READER);
	pthread_mutex_lock(&(list_readers_current->list_mutex));
		addElement(list_readers_current->readers, (void*)new_reader);
	pthread_mutex_unlock(&(list_readers_current->list_mutex));
	//creo il thread reader
	argument_thread_reader* argument = (argument_thread_reader*)malloc(sizeof(argument_thread_reader));
	argument->reader = new_reader;
	argument->list_readers_current = list_readers_current;
	pthread_t new_thread_reader;
	pthread_create(&new_thread_reader, NULL, reader_thread_init,(void*)argument);
	printf("\naccepter crea il thread reader di nome %s", name);
}


void list_readers_destroy(list_readers* list_readers_current){
	pthread_mutex_lock(&(list_readers_current->list_mutex));
	if(list_readers_current){
		if(list_readers_current->readers){
			iterator_t* iterator = iterator_init(list_readers_current->readers);
			reader_t* reader;
			msg_t* last_poison_pill_msg;				
			while(hasNext(iterator)){
				reader =((reader_t*)(iterator->currentNode->payload));
				last_poison_pill_msg = msg_init_pill("poison_pill\0");
				put_bloccante(reader->reader_buffer,msg_copy_pill(last_poison_pill_msg));
				next(iterator);
			}
			iterator_destroy(iterator);
			pthread_mutex_unlock(&(list_readers_current->list_mutex));
			//per dare tempo ai readers di suicidarsi e aggiornare la lista di readers
			int k = size(list_readers_current->readers);			
			while(k >= 0){
				if(size(list_readers_current->readers) == 0){
					list_destroy(list_readers_current->readers);
					pthread_mutex_destroy(&list_readers_current->list_mutex);
					free(list_readers_current);
					k = -1;
				}
			}
		}
	}

}



			


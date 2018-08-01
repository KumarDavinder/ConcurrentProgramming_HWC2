#include "/home/kumardavinder/Desktop/CUnit-2.1-2/CUnit/Headers/Basic.h"
#include "provider.h"
#include "dispatcher.h"
#include <pthread.h>

unsigned int SIZE_BUFFER_ACCEPTER = 5;
unsigned int SIZE_BUFFER_PROVIDER = 4;
unsigned int SIZE_BUFFER_READER = 4;
unsigned int NUM_MESSAGES_REQUESTS = 3;
unsigned int NUM_REQUESTS_READER = 4;
int codice_ritorno_thread_provider;
int codice_ritorno_thread_accepter;
int codice_ritorno_thread_dispatcher;

int main(int argc, char **argv){

	/*=================================================================================*/
	/*============================inizializzo provider=================================*/

		buffer_t* provider_buffer = buffer_init(SIZE_BUFFER_PROVIDER);
		
	/*=================================================================================*/
	/*==========================inizializzo acceppter==================================*/
		
		buffer_t* accepter_buffer = buffer_init(SIZE_BUFFER_ACCEPTER);
		//riempo il buffer_accepter
		int i;
		char nameReader[10], numero [2];
		msg_t* new_msg;
		for(i=0; i<NUM_REQUESTS_READER; i++){
			strcpy(nameReader,"reader");
			sprintf(numero, "%d", i);
			strcat(nameReader ,numero);
			new_msg = (msg_t*)msg_init(nameReader);
			put_bloccante(accepter_buffer, new_msg);
		}

		accepter_arg* argument_accepter = (accepter_arg*)malloc(sizeof(accepter_arg));

		list_readers* list_readers_current = (list_readers*)malloc(sizeof(list_readers));
		pthread_mutex_init(&list_readers_current->list_mutex,NULL);	//inizializzo il mutex sulla lista
		list_readers_current->readers = list_init();	//inizializzo la lista dei readers
		argument_accepter->list_readers_current = list_readers_current;	

		argument_accepter->accepter_buffer = accepter_buffer; //argomenti del thread creati

	/*=================================================================================*/
	/*=========================inizializzo dispatcher==================================*/

	arg_dispatcher* argument_dispatcher = (arg_dispatcher*)malloc(sizeof(arg_dispatcher));
	argument_dispatcher->list_readers_current = list_readers_current;
	argument_dispatcher->provider_buffer = provider_buffer; //argomenti del thread creati

	/*=================================================================================*/
	/*=================================creo thread=====================================*/
	pthread_t provider_thread, accepter_thread, dispatcher_thread;

	pthread_create(&accepter_thread, NULL,accepter_init, (void*)argument_accepter);

	pthread_create(&provider_thread, NULL,provider_init, (void*)provider_buffer);
	pthread_join(provider_thread,(void*)&codice_ritorno_thread_provider); //PROVIDER TERMINA
	if(codice_ritorno_thread_provider == 0){ //NUM_MESSAGES_REQUESTS >= SIZE_BUFFER_PROVIDER
		exit(1);
	}
	put_bloccante(accepter_buffer, POISON_PILL); //POISON_PILL all'accepter

	pthread_join(accepter_thread,(void*)&codice_ritorno_thread_accepter);	//ACCEPTER TERMINA
	printf("\nSIZE LIST READERS quando accepter ha finito: %d\n",size(list_readers_current->readers));

	pthread_create(&dispatcher_thread, NULL,dispatcher_init, (void*)argument_dispatcher);	
	pthread_join(dispatcher_thread,(void*)&codice_ritorno_thread_dispatcher);	
	printf("\nSIZE LIST READERS quando dispatcher ha finito: %d\n",size(list_readers_current->readers));

	list_readers_destroy(list_readers_current);
	buffer_destroy(accepter_buffer);
	free(argument_accepter);

	buffer_destroy(provider_buffer);
	free(argument_dispatcher);
}





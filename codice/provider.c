#include "provider.h"

extern unsigned int NUM_MESSAGES_REQUESTS;

void* provider_init(void* args){
	buffer_t* provider_buffer = (buffer_t*)args;
	if(NUM_MESSAGES_REQUESTS >= provider_buffer->N){
		printf("\nNumero messaggi da creare superiori alla dimensione del buffer\n");
		pthread_exit((void*)0);
	}
	char content[10];
	char number[2];
	msg_t* new_msg;

	int i;
	for(i=0; i<NUM_MESSAGES_REQUESTS; i++){
		strcpy(content,"messaggio");
		sprintf(number, "%d", i);	//conversione int char
		strcat(content ,number);	//messaggio0, messaggio1, messaggio3...
		new_msg = (msg_t*)msg_init(content);
		put_bloccante(provider_buffer, new_msg);
		printf("\nprovider inserisce il messaggio: %s", (char*)new_msg->content);
	}
	put_bloccante(provider_buffer, POISON_PILL); //ultimo messaggio è POISON_PILL
	printf("\nprovider termina\n");
	pthread_exit((void*)1);
}

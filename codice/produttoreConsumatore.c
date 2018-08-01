#include "produttoreConsumatore.h"
#include "operazioniBuffer.h"


/*----------operazioni per la creazione dei thread---------*/

void* thread_put_bloccante(void* args){
	arguments* arg = (arguments*)args;
	put_bloccante(arg->buffer, arg->msg);
}

void* thread_put_non_bloccante(void* args){
	arguments* arg = (arguments*)args;
	put_non_bloccante(arg->buffer, arg->msg);
}

void* thread_get_bloccante(void* args){
	get_bloccante((buffer_t*)args);
}

void* thread_get_non_bloccante(void* args){
	get_non_bloccante((buffer_t*)args);
}

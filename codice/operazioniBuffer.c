#include "operazioniBuffer.h"
#include "buffer.h"

/*----------operazioni su buffer---------*/

msg_t* put_bloccante(buffer_t* buffer, msg_t* msg){
	pthread_mutex_lock(&buffer->USO_D);
	while(buffer->N == buffer->num_messaggi)
		pthread_cond_wait(&buffer->non_pieno,&buffer->USO_D);
	buffer->cells[buffer->D] = msg_copy(msg);
	buffer->num_messaggi++;
	buffer->D = ((buffer->D)+1)%buffer->N;
	pthread_cond_signal(&buffer->non_vuoto);
	pthread_mutex_unlock(&buffer->USO_D);	
	return msg;
}

msg_t* put_non_bloccante(buffer_t* buffer, msg_t* msg){
	pthread_mutex_lock(&buffer->USO_D);
	if(buffer->N == buffer->num_messaggi){
		pthread_mutex_unlock(&buffer->USO_D);
		return BUFFER_ERROR;
	}
	buffer->cells[buffer->D] = msg_copy(msg);;
	buffer->num_messaggi++;
	buffer->D = ((buffer->D)+1)%(buffer->N);
	pthread_cond_signal(&buffer->non_vuoto);
	pthread_mutex_unlock(&buffer->USO_D);
	return msg;
}

msg_t* get_bloccante(buffer_t* buffer){
	pthread_mutex_lock(&buffer->USO_T);
		while(buffer->num_messaggi == 0)
			pthread_cond_wait(&buffer->non_vuoto,&buffer->USO_T);
		msg_t* msg_return = (msg_t*)malloc(sizeof(msg_t));
		buffer->num_messaggi--;
		msg_return = buffer->cells[buffer->T];
		buffer->cells[buffer->T] = NULL;
		buffer->T = (buffer->T+1)%buffer->N;
		pthread_cond_signal(&buffer->non_pieno);
	pthread_mutex_unlock(&buffer->USO_T);

	return msg_return;
}

msg_t* get_non_bloccante(buffer_t* buffer){
	pthread_mutex_lock(&buffer->USO_T);
		if(buffer->num_messaggi == 0){
			pthread_mutex_unlock(&buffer->USO_T);
			return BUFFER_ERROR;
		}
		msg_t* msg_return = (msg_t*)malloc(sizeof(msg_t));
		buffer->num_messaggi--;
		msg_return = buffer->cells[buffer->T];
		buffer->cells[buffer->T] = NULL;
		buffer->T = (buffer->T+1)%buffer->N;
		pthread_cond_signal(&buffer->non_pieno);
	pthread_mutex_unlock(&buffer->USO_T);
	return msg_return;
}



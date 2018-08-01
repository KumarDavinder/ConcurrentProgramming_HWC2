#include "buffer.h"

/* allocazione / deallocazione buffer */
// creazione di un buffer vuoto di dim. max nota
buffer_t* buffer_init(unsigned int maxsize){
	buffer_t* buffer = (buffer_t*)malloc(sizeof(buffer_t));
	if(buffer){
		buffer->num_messaggi = 0;
		buffer->D=0;
		buffer->T=0;
		buffer->N = maxsize;
		pthread_cond_init(&buffer->non_pieno,NULL);
		pthread_cond_init(&buffer->non_vuoto,NULL);
		pthread_mutex_init(&buffer->USO_D,NULL);
		pthread_mutex_init(&buffer->USO_T,NULL);
		buffer->cells = (msg_t**)calloc(maxsize,sizeof(msg_t*));
		if(buffer->cells == NULL){
			printf("\nproblem with malloc when you create cells\n");
			exit(1);
		}
		int i;
		for(i=0; i<buffer->N; i++){
			buffer->cells[i] = NULL;
		}
	}
	else{
		printf("\nproblem with malloc when you create buffer\n");
		exit(1);
	}
	return buffer;
}
// deallocazione di un buffer
void buffer_destroy(buffer_t* buffer){
	if(buffer){
		if(buffer->cells){
			int i;
			for(i=0; i<buffer->N; i++){
				msg_t* msg = (msg_t*)buffer->cells[i];
				if(msg){
					msg->msg_destroy(msg);
				}

			}
		}
		free(buffer->cells);
		pthread_cond_destroy(&buffer->non_pieno);
		pthread_cond_destroy(&buffer->non_vuoto);
		pthread_mutex_destroy(&buffer->USO_T);
		pthread_mutex_destroy(&buffer->USO_D);
		free(buffer);
	} 
}

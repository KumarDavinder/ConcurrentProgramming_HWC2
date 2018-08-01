#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "message.h"

msg_t* msg_init(void* content) {
	//viene creata una copia "privata" della stringa
	msg_t* new_msg = (msg_t*)malloc( sizeof(msg_t) );
	if(new_msg){
		new_msg->msg_init = msg_init;
		new_msg->msg_destroy = msg_destroy;
		new_msg->msg_copy = msg_copy;
		char* string = (char*)content;
		char* new_content = (char*)malloc(strlen(string)+1); // +1 per \0 finale
		if(new_content){
			strcpy(new_content, string);
			new_msg->content = new_content;
		}
		else{
			printf("\nproblem with malloc when you create msg->content\n");
			free(new_msg);
			exit(1);
		}
	}
	else{
		printf("\nproblem with malloc when you create message\n");
		exit(1);
	}
	return new_msg;
}

void msg_destroy(msg_t* msg) {
	if(msg){
		if(msg->content)
			free(msg->content); // free copia privata
	}	
	free(msg); // free struct
}

msg_t* msg_copy(msg_t* msg) {
	if(msg && msg->content)
		return msg->msg_init(msg->content);
	else{
		printf("\nYou can't put a NULL message in buffer or a message without content\n");
		exit(1);
	}
}

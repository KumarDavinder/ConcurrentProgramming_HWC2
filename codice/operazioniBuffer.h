#include "buffer.h"
#ifndef OPERAZIONIBUFFER_H
#define OPERAZIONIBUFFER_H

/* operazioni sul buffer */

/* inserimento bloccante: sospende se pieno, quindi
 effettua l'inserimento non appena si libera dello spazio
 restituisce il messaggio inserito; N.B.: msg!=null*/
msg_t* put_bloccante(buffer_t* buffer, msg_t* msg);

/* inserimento non bloccante: restituisce BUFFER_ERROR se pieno,
 altrimenti effettua l'inserimento e restituisce il messaggio
 inserito; N.B.: msg!=null*/
msg_t* put_non_bloccante(buffer_t* buffer, msg_t* msg);

/*estrazione bloccante: sospende se vuoto, quindi
restituisce il valore estratto non appena disponibile*/
msg_t* get_bloccante(buffer_t* buffer);

/*estrazione non bloccante: restituisce BUFFER_ERROR se vuoto
ed il valore estratto in caso contrario*/
msg_t* get_non_bloccante(buffer_t* buffer);

#endif

#include "produttoreConsumatore.h"
#include <CUnit/Basic.h>


char* messaggio = "Messaggio\0";
buffer_t* buffer;
msg_t* msg_return; //la destroy dei messaggi viene invocata dal buffer_destroy

int clean(void){
	buffer_destroy(buffer);
	return 0;
}

/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 1===================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 1: (P=1; C=0; N=1) Produzione di un solo messaggio in un buffer vuoto*/
int setup_scenario1_put_bloccante(void){	
	buffer = buffer_init(1);
	msg_t* msg = msg_init(messaggio);
	arguments* args;
	args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;
	pthread_t produttore_thread;
	pthread_create(&produttore_thread, NULL,thread_put_bloccante, (void*)args);
	pthread_join(produttore_thread, (void*)&msg_return);
	return 0;
}

int setup_scenario1_put_non_bloccante(void){
	buffer = buffer_init(1);
	msg_t* msg = msg_init(messaggio);
	arguments* args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;
	pthread_t produttore_thread;
	pthread_create(&produttore_thread, NULL,thread_put_non_bloccante, (void*)args);
	pthread_join(produttore_thread, (void*)&msg_return);
	return 0;
}
/*=========================================TEST SCENARIO 1======================================================*/
void test_scenario1_check_content_message(void){
	CU_ASSERT_STRING_EQUAL((buffer->cells[buffer->T]->content),messaggio);
}

void test_scenario1_check_return_message(void){
	CU_ASSERT_STRING_EQUAL(msg_return->content,messaggio);
}

void test_scenario1_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 1);
}

/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 2===================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 2: (P=0; C=1; N=1) Consumazione di un solo messaggio da un buffer pieno */
int setup_scenario2_get_bloccante(void){
	buffer = buffer_init(1);
	msg_t* msg = msg_init(messaggio);
	arguments* args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;
	pthread_t produttore_thread;
	pthread_create(&produttore_thread, NULL,thread_put_bloccante, (void*)args);
	pthread_join(produttore_thread,NULL);		//buffer è pieno

	pthread_t consumatore_thread;
	pthread_create(&consumatore_thread, NULL,thread_get_bloccante, (void*)buffer);
	pthread_join(consumatore_thread, (void*)&msg_return);
	return 0;
}

int setup_scenario2_get_non_bloccante(void){
	//Inserisco prima un messaggio
	buffer = buffer_init(1);
	msg_t* msg = msg_init(messaggio);
	arguments* args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;
	pthread_t produttore_thread;
	pthread_create(&produttore_thread, NULL,thread_put_non_bloccante, (void*)args);
	pthread_join(produttore_thread, NULL);		//buffer è pieno

	pthread_t consumatore_thread;
	pthread_create(&consumatore_thread, NULL,thread_get_non_bloccante, (void*)buffer);
	pthread_join(consumatore_thread, (void*)&msg_return);
	return 0;
}

/*=========================================TEST SCENARIO 2======================================================*/
void test_scenario2_check_content_message(void){
	CU_ASSERT((buffer->cells[buffer->T]) == NULL);
}
void test_scenario2_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 0);
}
void test_scenario2_check_return_message(void){
	CU_ASSERT_STRING_EQUAL(msg_return->content,messaggio);
}

/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 3===================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 3: (P=1; C=0; N=1) Produzione in un buffer pieno */
int setup_scenario3_put_non_bloccante_buffer_pieno(void){	
	buffer = buffer_init(1);
	msg_t* MSG = msg_init(messaggio);
	msg_t* EXPECTED_MSG = msg_init(messaggio);
	arguments* args;
	args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = EXPECTED_MSG;
	pthread_t produttore_thread1;
	pthread_create(&produttore_thread1, NULL,thread_put_bloccante, (void*)args);
	pthread_join(produttore_thread1, NULL); //qui il buffer è pieno

	pthread_t produttore_thread2;
	args->msg = MSG;
	pthread_create(&produttore_thread2, NULL,thread_put_non_bloccante, (void*)args); //provo a inserire MSG
	pthread_join(produttore_thread2, (void*)&msg_return);
	return 0;
}

/*=========================================TEST SCENARIO 3======================================================*/
void test_scenario3_check_content_message(void){
	CU_ASSERT_STRING_EQUAL((buffer->cells[buffer->num_messaggi-1]->content),messaggio);
}
void test_scenario3_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 1);
}
void test_scenario3_check_buffer_error(void){
	CU_ASSERT(msg_return == NULL);
}

/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 4===================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 4: (P=0; C=1; N=1) Consumazione da un buffer vuoto*/
int setup_scenario4_get_bloccante_buffer_vuoto(void){
	buffer = buffer_init(1);
	msg_t* GO_MSG = msg_init(messaggio);
	arguments* args;
	args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = GO_MSG;
	pthread_t consumatore_thread;
	pthread_t produttore_thread;
	pthread_create(&consumatore_thread, NULL,thread_get_bloccante, (void*)buffer); //consumatore aspetta la signal
	//sleep(2);
	pthread_create(&produttore_thread, NULL,thread_put_bloccante, (void*)args);
	pthread_join(consumatore_thread, (void*)&msg_return);
	pthread_join(produttore_thread, NULL);
	return 0;
}
/*=========================================TEST SCENARIO 4======================================================*/
void test_scenario4_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 0);
}
void test_scenario4_check_return_message_bloccante(void){
	CU_ASSERT_STRING_EQUAL(msg_return->content,messaggio);
}
void test_scenario4_check_content_message(void){
	CU_ASSERT((buffer->cells[buffer->T]) == NULL);
}

/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 5===================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 5:(P=1; C=1; N=1) Consumazione e produzione concorrente di un messaggio da un buffer unitario pieno; 
prima il consumatore*/

//bloccante
int setup_scenario5_concorrente_bloccante(void){
	buffer = buffer_init(1);
	msg_t* msg = msg_init(messaggio);
	arguments* args;
	args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;
	pthread_t produttore_thread1;
	pthread_t consumatore_thread2;
	pthread_t produttore_thread3;
	pthread_create(&produttore_thread1, NULL,thread_put_bloccante, (void*)args);
	pthread_join(produttore_thread1, NULL);	

	pthread_create(&consumatore_thread2, NULL,thread_get_bloccante, (void*)buffer);
	pthread_create(&produttore_thread3, NULL,thread_put_bloccante, (void*)args);
	pthread_join(consumatore_thread2, (void*)&msg_return);
	pthread_join(produttore_thread3, NULL);	
	return 0;
}

/*=========================================TEST SCENARIO 5======================================================*/
void test_scenario5_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 1);
}
void test_scenario5_check_return_message_bloccante(void){
	CU_ASSERT_STRING_EQUAL(msg_return->content,messaggio);
}

/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 6===================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 6:(P=1; C=1; N=1) Consumazione e produzione concorrente di un messaggio da un buffer unitario pieno; 
prima il produttore*/

//BLOCCANTE
int setup_scenario6_concorrente_bloccante(void){
	buffer = buffer_init(1);
	msg_t* msg = msg_init(messaggio);
	arguments* args;
	args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;
	pthread_t produttore_thread1;
	pthread_create(&produttore_thread1, NULL,thread_put_bloccante, (void*)args);
	pthread_join(produttore_thread1, NULL);

	pthread_t produttore_thread2;
	pthread_t consumatore_thread3;
	pthread_create(&produttore_thread2, NULL,thread_put_bloccante, (void*)args);
	pthread_create(&consumatore_thread3, NULL,thread_get_bloccante, (void*)buffer);
	pthread_join(consumatore_thread3, (void*)&msg_return);	
	pthread_join(produttore_thread2, NULL);
	return 0;
}

/*=========================================TEST SCENARIO 6======================================================*/
//bloccante
void test_scenario6_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 1);
}

void test_scenario6_check_return_message(void){
	CU_ASSERT_STRING_EQUAL(msg_return->content,messaggio);
}

void test_scenario6_check_content_message(void){
	CU_ASSERT_STRING_EQUAL(buffer->cells[buffer->T]->content,messaggio);
}


/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 7===================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 7:(P>1; C=0; N=1) Produzione concorrente di molteplici messaggi in un
buffer unitario vuoto*/

//NON BLOCCANTE
int setup_scenario7_put_non_bloccante_concorrente_buffer_unitario(void){
	buffer = buffer_init(1);
	msg_t* msg = msg_init(messaggio);
	arguments* args;
	args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;
	pthread_t produttore_thread1;
	pthread_t produttore_thread2;
	pthread_t produttore_thread3;	
	pthread_create(&produttore_thread1, NULL,thread_put_non_bloccante, (void*)args);
	pthread_create(&produttore_thread2, NULL,thread_put_non_bloccante, (void*)args);
	pthread_create(&produttore_thread3, NULL,thread_put_non_bloccante, (void*)args);
	pthread_join(produttore_thread1, NULL);
	pthread_join(produttore_thread2, NULL);
	pthread_join(produttore_thread3, NULL);	
	return 0;
}

/*=========================================TEST SCENARIO 7=====================================================*/
//non bloccante
void test_scenario7_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 1);
}

void test_scenario7_check_content_message(void){
	CU_ASSERT_STRING_EQUAL(buffer->cells[buffer->T]->content,messaggio);
}

/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 8===================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 8: (P=0; C>1; N=1) Consumazione concorrente di molteplici messaggi da
un buffer unitario pieno*/
//BLOCCANTE
int setup_scenario8_concorrente_bloccante(void){
	buffer = buffer_init(1);
	msg_t* msg = msg_init(messaggio);
	arguments* args;
	args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;
	//inserisco prima un messaggio
	pthread_t produttore_thread1;
	pthread_create(&produttore_thread1, NULL,thread_put_bloccante, (void*)args);
	pthread_join(produttore_thread1, NULL);

	//consumazione concorrente
	pthread_t consumatore_thread1;
	pthread_t consumatore_thread2;
	pthread_create(&consumatore_thread1, NULL,thread_get_bloccante, (void*)buffer);
	pthread_create(&consumatore_thread2, NULL,thread_get_bloccante, (void*)buffer);

	//sblocco consumatore_thread2
	pthread_t produttore_thread2;
	pthread_create(&produttore_thread2, NULL,thread_put_bloccante, (void*)args);

	pthread_join(produttore_thread2, NULL);
	pthread_join(consumatore_thread1, NULL);
	pthread_join(consumatore_thread2, NULL);
	return 0;
}

//NON BLOCCANTE
int setup_scenario8_concorrente_non_bloccante(void){
	buffer = buffer_init(1);
	msg_t* msg = msg_init(messaggio);
	arguments* args;
	args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;

	//inserisco prima un messaggio
	pthread_t produttore_thread1;
	pthread_create(&produttore_thread1, NULL,thread_put_non_bloccante, (void*)args);
	pthread_join(produttore_thread1, NULL);
	//consumo 
	pthread_t consumatore_thread1;		// il primo che arriva passa
	pthread_t consumatore_thread2;
	pthread_create(&consumatore_thread1, NULL,thread_get_non_bloccante, (void*)buffer); 
	pthread_create(&consumatore_thread2, NULL,thread_get_non_bloccante, (void*)buffer);
	pthread_join(consumatore_thread1, NULL);
	pthread_join(consumatore_thread2, NULL);	
	return 0;
}

/*=========================================TEST SCENARIO 8======================================================*/
//bloccante
void test_scenario8_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 0);
}
void test_scenario8_check_content_message(void){
	CU_ASSERT((buffer->cells[buffer->T]) == NULL);
}

/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 9===================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 9: (P>1; C=0; N>1) Produzione concorrente di molteplici messaggi in un
buffer vuoto; il buffer non si riempe*/

//BLOCCANTE
int setup_scenario9_put_concorrente_bloccante(void){
	buffer = buffer_init(10);
	msg_t* msg = msg_init(messaggio);
	arguments* args;
	args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;

	pthread_t produttore_thread1;
	pthread_t produttore_thread2;
	pthread_t produttore_thread3;

	pthread_create(&produttore_thread1,NULL,thread_put_bloccante,(void*)args);
	pthread_create(&produttore_thread2,NULL,thread_put_bloccante,(void*)args);
	pthread_create(&produttore_thread3,NULL,thread_put_bloccante,(void*)args);

	pthread_join(produttore_thread1,NULL);
	pthread_join(produttore_thread2,NULL);
	pthread_join(produttore_thread3,NULL);
	return 0;
}

//NON BLOCCANTE
int setup_scenario9_put_concorrente_non_bloccante(void){
	buffer = buffer_init(10);
	msg_t* msg = msg_init(messaggio);
	arguments* args;
	args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;

	pthread_t produttore_thread1;
	pthread_t produttore_thread2;
	pthread_t produttore_thread3;

	pthread_create(&produttore_thread1,NULL,thread_put_non_bloccante,(void*)args);
	pthread_create(&produttore_thread2,NULL,thread_put_non_bloccante,(void*)args);
	pthread_create(&produttore_thread3,NULL,thread_put_non_bloccante,(void*)args);

	pthread_join(produttore_thread1,NULL);
	pthread_join(produttore_thread2,NULL);
	pthread_join(produttore_thread3,NULL);
	return 0;
}
/*=========================================TEST SCENARIO 9======================================================*/
void test_scenario9_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 3);
}
void test_scenario9_check_indice_D(void){
	CU_ASSERT(buffer->D == 3);
}

/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 10===================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 10: (P>1; C=0; N>1) Produzione concorrente di molteplici messaggi in un
buffer pieno; il buffer è già saturo*/

int setup_scenario10_put_concorrente_buffer_pieno(void){
	buffer = buffer_init(3);
	msg_t* msg = msg_init(messaggio);
	arguments* args;
	args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;

	pthread_t produttore_thread1;
	pthread_t produttore_thread2;
	pthread_t produttore_thread3;
	pthread_create(&produttore_thread1,NULL,thread_put_bloccante,(void*)args);
	pthread_create(&produttore_thread2,NULL,thread_put_bloccante,(void*)args);
	pthread_create(&produttore_thread3,NULL,thread_put_bloccante,(void*)args);
	pthread_join(produttore_thread1,NULL);
	pthread_join(produttore_thread2,NULL);
	pthread_join(produttore_thread3,NULL); //buffer pieno

	pthread_t produttore_thread4;
	pthread_t produttore_thread5;
	pthread_create(&produttore_thread4,NULL,thread_put_non_bloccante,(void*)args);
	pthread_create(&produttore_thread5,NULL,thread_put_non_bloccante,(void*)args);
	pthread_join(produttore_thread4,NULL);
	pthread_join(produttore_thread5,NULL);
	return 0;
}
/*=========================================TEST SCENARIO 10======================================================*/
void test_scenario10_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 3);
}
void test_scenario10_check_indice_D(void){
	CU_ASSERT(buffer->D == 0);
}


/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 11===================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 11: (P>1; C=0; N>1) Produzione concorrente di molteplici messaggi in un buffer vuoto; il buffer si satura in corso*/

int setup_scenario11_put_concorrente_bloccante(void){
	buffer = buffer_init(4);
	msg_t* msg = msg_init(messaggio);
	arguments* args;
	args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;
	pthread_t produttore_thread1;
	pthread_t produttore_thread2;
	pthread_t produttore_thread3;
	pthread_t produttore_thread4;
	pthread_t produttore_thread5;
	pthread_t consumatore_thread6;
	pthread_create(&produttore_thread1,NULL,thread_put_bloccante,(void*)args);
	pthread_create(&produttore_thread2,NULL,thread_put_bloccante,(void*)args);
	pthread_create(&produttore_thread3,NULL,thread_put_bloccante,(void*)args);
	pthread_create(&produttore_thread4,NULL,thread_put_bloccante,(void*)args);
	pthread_create(&produttore_thread5,NULL,thread_put_bloccante,(void*)args);
	//consumo 1 messaggio e sblocco uno dei produttori in caso che arrivino tutti prima del consumatore
	pthread_create(&consumatore_thread6,NULL,thread_get_bloccante,(void*)buffer);
	pthread_join(produttore_thread1,NULL);
	pthread_join(produttore_thread2,NULL);
	pthread_join(produttore_thread3,NULL);
	pthread_join(produttore_thread4,NULL);
	pthread_join(produttore_thread5,NULL);
	pthread_join(consumatore_thread6,NULL);
	return 0;
}
/*=========================================TEST SCENARIO 11======================================================*/
void test_scenario11_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 4);
}
void test_scenario11_check_indice_D(void){
	CU_ASSERT(buffer->D == 1);	//5 mod 4 = 1
}
void test_scenario11_check_indice_T(void){
	CU_ASSERT(buffer->T == 1);	//1 mod 4 = 1
}

/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 12==================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 12: (P=0; C>1; N>1) Consumazione concorrente di molteplici messaggi da
un buffer pieno*/
int setup_scenario12_get_concorrente_bloccante(void){
	//riempio il buffer
	buffer = buffer_init(5);
	msg_t* msg = msg_init(messaggio);
	arguments* args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;
	pthread_t produttore_thread1;
	pthread_t produttore_thread2;
	pthread_t produttore_thread3;
	pthread_t produttore_thread4;
	pthread_create(&produttore_thread1, NULL,thread_put_bloccante, (void*)args);
	pthread_create(&produttore_thread2, NULL,thread_put_bloccante, (void*)args);
	pthread_create(&produttore_thread3, NULL,thread_put_bloccante, (void*)args);
	pthread_create(&produttore_thread4, NULL,thread_put_bloccante, (void*)args);
	pthread_t consumatore_thread1;
	pthread_t consumatore_thread2;
	pthread_t consumatore_thread3;
	pthread_create(&consumatore_thread1, NULL,thread_get_bloccante, (void*)buffer);
	pthread_create(&consumatore_thread2, NULL,thread_get_bloccante, (void*)buffer);
	pthread_create(&consumatore_thread3, NULL,thread_get_bloccante, (void*)buffer);
	pthread_join(produttore_thread1,NULL);
	pthread_join(produttore_thread2,NULL);
	pthread_join(produttore_thread3,NULL);
	pthread_join(produttore_thread4,NULL);
	pthread_join(consumatore_thread1,NULL); 
	pthread_join(consumatore_thread2,NULL); 
	pthread_join(consumatore_thread3,NULL); 
	return 0;
}

/*=========================================TEST SCENARIO 12======================================================*/
//bloccante
void test_scenario12_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 1);
}
void test_scenario12_check_content_message(void){
	CU_ASSERT((buffer->cells[0]) == NULL);
	CU_ASSERT((buffer->cells[1]) == NULL);
	CU_ASSERT((buffer->cells[2]) == NULL);
	CU_ASSERT((buffer->cells[3]) != NULL);
}
void test_scenario12_check_indice_D(void){
	CU_ASSERT(buffer->D == 4);
}
void test_scenario12_check_indice_T(void){
	CU_ASSERT(buffer->T == 3);
}

/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 13==================================================*/
/*==============================================================================================================*/

/*SETUP SCENARIO 13:(P>1; C>1; N=1) Consumazioni e produzioni concorrenti di molteplici messaggi in un buffer unitario*/

int setup_scenario13_put_get_concorrente_bloccante_buffer_unitario(void){
	//riempio il buffer
	buffer = buffer_init(1);
	msg_t* msg = msg_init(messaggio);
	arguments* args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;
	pthread_t produttore_thread1;
	pthread_t produttore_thread2;
	pthread_t produttore_thread3;
	pthread_create(&produttore_thread1, NULL,thread_put_bloccante, (void*)args);
	pthread_create(&produttore_thread2, NULL,thread_put_bloccante, (void*)args);
	pthread_create(&produttore_thread3, NULL,thread_put_bloccante, (void*)args);

	pthread_t consumatore_thread1;
	pthread_t consumatore_thread2;
	pthread_t consumatore_thread3;
	pthread_create(&consumatore_thread1, NULL,thread_get_bloccante, (void*)buffer);
	pthread_create(&consumatore_thread2, NULL,thread_get_bloccante, (void*)buffer);
	pthread_create(&consumatore_thread3, NULL,thread_get_bloccante, (void*)buffer);
	pthread_join(produttore_thread1,NULL);
	pthread_join(produttore_thread2,NULL);
	pthread_join(produttore_thread3,NULL);
	pthread_join(consumatore_thread1,NULL); 
	pthread_join(consumatore_thread2,NULL); 
	pthread_join(consumatore_thread3,NULL); 
	return 0;
}

/*=========================================TEST SCENARIO 13======================================================*/

void test_scenario13_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 0);
}
void test_scenario13_check_content_message(void){
	CU_ASSERT((buffer->cells[0]) == NULL);
}
void test_scenario13_check_indice_D(void){
	CU_ASSERT(buffer->D == 0);
}
void test_scenario13_check_indice_T(void){
	CU_ASSERT(buffer->T == 0);
}
/*==============================================================================================================*/
/*===========================================SETUP SCENARIO 14==================================================*/
/*==============================================================================================================*/
/*SETUP SCENARIO 14:(P>1; C>1; N>1) Consumazioni e produzioni concorrenti di molteplici messaggi in un buffer*/

int setup_scenario14_put_get_concorrente_bloccante(void){
	//riempio il buffer
	buffer = buffer_init(10);
	msg_t* msg = msg_init(messaggio);
	arguments* args = (arguments*)malloc(sizeof(arguments));
	args->buffer = buffer;
	args->msg = msg;
	pthread_t produttore_thread1;
	pthread_t produttore_thread2;
	pthread_t produttore_thread3;
	pthread_t produttore_thread4;
	pthread_create(&produttore_thread1, NULL,thread_put_bloccante, (void*)args);
	pthread_create(&produttore_thread2, NULL,thread_put_bloccante, (void*)args);
	pthread_create(&produttore_thread3, NULL,thread_put_bloccante, (void*)args);
	pthread_create(&produttore_thread4, NULL,thread_put_bloccante, (void*)args);

	pthread_t consumatore_thread1;
	pthread_t consumatore_thread2;
	pthread_t consumatore_thread3;
	pthread_create(&consumatore_thread1, NULL,thread_get_bloccante, (void*)buffer);
	pthread_create(&consumatore_thread2, NULL,thread_get_bloccante, (void*)buffer);
	pthread_create(&consumatore_thread3, NULL,thread_get_bloccante, (void*)buffer);
	pthread_join(produttore_thread1,NULL);
	pthread_join(produttore_thread2,NULL);
	pthread_join(produttore_thread3,NULL);
	pthread_join(produttore_thread4,NULL);
	pthread_join(consumatore_thread1,NULL); 
	pthread_join(consumatore_thread2,NULL); 
	pthread_join(consumatore_thread3,NULL); 
	return 0;
}

/*=========================================TEST SCENARIO 14======================================================*/

void test_scenario14_check_number_message_buffer(void){
	CU_ASSERT(buffer->num_messaggi == 1);
}
void test_scenario14_check_content_message(void){
	CU_ASSERT((buffer->cells[0]) == NULL);
	CU_ASSERT((buffer->cells[1]) == NULL);
	CU_ASSERT((buffer->cells[2]) == NULL);
	CU_ASSERT((buffer->cells[3]) != NULL);
}
void test_scenario14_check_indice_D(void){
	CU_ASSERT(buffer->D == 4);
}
void test_scenario14_check_indice_T(void){
	CU_ASSERT(buffer->T == 3);
}
/*=====================================fine setup e test=========================================*/
/*===============================================================================================*/
/*===============================================================================================*/

/*=========================================RUN TEST SCENARI======================================================*/

int main(){
	CU_pSuite suite_scenario1_put_bloccante = NULL;
	CU_pSuite suite_scenario1_put_non_bloccante = NULL;
	CU_pSuite suite_scenario2_get_bloccante = NULL;
	CU_pSuite suite_scenario2_get_non_bloccante = NULL;
	CU_pSuite suite_scenario3_put_non_bloccante_buffer_pieno = NULL;
	CU_pSuite suite_scenario4_get_bloccante_buffer_vuoto = NULL;
	CU_pSuite suite_scenario5_concorrente_bloccante = NULL;
	CU_pSuite suite_scenario6_concorrente_bloccante = NULL;
	CU_pSuite suite_scenario7_concorrente_non_bloccante_buffer_unitario = NULL;
	CU_pSuite suite_scenario8_concorrente_bloccante = NULL;
	CU_pSuite suite_scenario8_concorrente_non_bloccante = NULL;
	CU_pSuite suite_scenario9_put_concorrente_bloccante = NULL;
	CU_pSuite suite_scenario9_put_concorrente_non_bloccante = NULL;
	CU_pSuite suite_scenario10_put_concorrente_buffer_pieno = NULL;
	CU_pSuite suite_scenario11_put_concorrente_bloccante = NULL;
	CU_pSuite suite_scenario12_get_concorrente_bloccante = NULL;
	CU_pSuite suite_scenario13_put_get_concorrente_bloccante_buffer_unitario = NULL;
	CU_pSuite suite_scenario14_put_get_concorrente_bloccante = NULL;

   	/* initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry()){
		return CU_get_error();
	}

/*========================================RUN SCENARIO 1======================================================*/

/* add a suite SCENARIO 1 BLOCCANTE to the registry */
	suite_scenario1_put_bloccante = CU_add_suite("test scenario 1: put_bloccante", setup_scenario1_put_bloccante, clean);
	if (NULL == suite_scenario1_put_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario1_put_bloccante, "controllo contenuto buffer", 
			test_scenario1_check_content_message)) ||
	 	(NULL == CU_add_test(suite_scenario1_put_bloccante, "controllo numero messaggi in buffer", 
			test_scenario1_check_number_message_buffer)) || 
		(NULL == CU_add_test(suite_scenario1_put_bloccante, "controllo messaggio di ritorno",
			test_scenario1_check_return_message))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }

	 /* add a suite SCENARIO 1 NON BLOCCANTE to the registry */
	suite_scenario1_put_non_bloccante = CU_add_suite("test scenario 1: put_non_bloccante", 
		setup_scenario1_put_non_bloccante, clean);
	if (NULL == suite_scenario1_put_non_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario1_put_non_bloccante, "controllo contenuto buffer", 
			test_scenario1_check_content_message)) ||
		(NULL == CU_add_test(suite_scenario1_put_non_bloccante, "controllo numero messaggi in buffer",
			test_scenario1_check_number_message_buffer))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }
/*========================================RUN SCENARIO 2======================================================*/

	/* add a suite SCENARIO 2 BLOCCANTE to the registry */
	suite_scenario2_get_bloccante = CU_add_suite("test scenario 2: get_bloccante", setup_scenario2_get_bloccante, clean);
	if (NULL == suite_scenario2_get_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario2_get_bloccante, "controllo contenuto buffer", 
			test_scenario2_check_content_message)) ||
		(NULL == CU_add_test(suite_scenario2_get_bloccante, "controllo numero messaggi in buffer", 
			test_scenario2_check_number_message_buffer)) ||
		(NULL == CU_add_test(suite_scenario2_get_bloccante, "controllo messaggio letto",
			test_scenario2_check_return_message))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }

	 /* add a suite SCENARIO 2 NON BLOCCANTE to the registry */
	suite_scenario2_get_non_bloccante = CU_add_suite("test scenario 2: get_non_bloccante", 
		setup_scenario2_get_non_bloccante, clean);
	if (NULL == suite_scenario2_get_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario2_get_non_bloccante, "controllo contenuto buffer", 
			test_scenario2_check_content_message)) ||
		(NULL == CU_add_test(suite_scenario2_get_non_bloccante, "controllo numero messaggi in buffer", 
			test_scenario2_check_number_message_buffer)) ||
		(NULL == CU_add_test(suite_scenario2_get_non_bloccante, "controllo messaggio letto", 
			test_scenario2_check_return_message))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }
/*========================================RUN SCENARIO 3======================================================*/

 /* add a suite SCENARIO 3 BLOCCANTE to the registry */
	suite_scenario3_put_non_bloccante_buffer_pieno = CU_add_suite("test scenario 3: put_non_bloccante buffer pieno", 
		setup_scenario3_put_non_bloccante_buffer_pieno, clean);
	if (NULL == suite_scenario3_put_non_bloccante_buffer_pieno) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario3_put_non_bloccante_buffer_pieno, "controllo contenuto buffer", 
			test_scenario3_check_content_message)) ||
		(NULL == CU_add_test(suite_scenario3_put_non_bloccante_buffer_pieno, "controllo numero messaggi in buffer", 
			test_scenario3_check_number_message_buffer)) ||
		(NULL == CU_add_test(suite_scenario3_put_non_bloccante_buffer_pieno, "nuovo inseriemento: ritorno BUFFER_ERROR", 				test_scenario3_check_buffer_error))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }

/*========================================RUN SCENARIO 4======================================================*/
 
/* add a suite SCENARIO 4 BLOCCANTE to the registry */
	suite_scenario4_get_bloccante_buffer_vuoto = CU_add_suite("test scenario 4: get_bloccante buffer vuoto", 
		setup_scenario4_get_bloccante_buffer_vuoto, clean);
	if (NULL == suite_scenario4_get_bloccante_buffer_vuoto) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario4_get_bloccante_buffer_vuoto, "controllo contenuto buffer", 
			test_scenario4_check_content_message)) || 
		(NULL == CU_add_test(suite_scenario4_get_bloccante_buffer_vuoto,
			 "controllo numero messaggi in buffer", test_scenario4_check_number_message_buffer)) || 
		(NULL == CU_add_test(suite_scenario4_get_bloccante_buffer_vuoto, "controllo messaggio  letto", 
			test_scenario4_check_return_message_bloccante))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }
/*========================================RUN SCENARIO 5======================================================*/

/* add a suite SCENARIO 5 BLOCCANTE to the registry */
	suite_scenario5_concorrente_bloccante = CU_add_suite("test scenario 5: get_bloccante, put_bloccante concorrente, prima consumatore", setup_scenario5_concorrente_bloccante, clean);
	if (NULL == suite_scenario5_concorrente_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario5_concorrente_bloccante, 
			"controllo numero messaggi in buffer", test_scenario5_check_number_message_buffer)) || 
		(NULL == CU_add_test(suite_scenario5_concorrente_bloccante, "controllo messaggio  letto", 
			test_scenario5_check_return_message_bloccante))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }

/*========================================RUN SCENARIO 6======================================================*/

/* add a suite SCENARIO 6 BLOCCANTE to the registry */
	suite_scenario6_concorrente_bloccante = CU_add_suite("test scenario 6: get_bloccante, put_bloccante concorrente, prima produttore",  setup_scenario6_concorrente_bloccante, clean);
	if (NULL == suite_scenario6_concorrente_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario6_concorrente_bloccante, "controllo contenuto buffer", 
			test_scenario6_check_content_message)) || 
		(NULL == CU_add_test(suite_scenario6_concorrente_bloccante, "controllo numero messaggi in buffer", 
			test_scenario6_check_number_message_buffer)) || 
		(NULL == CU_add_test(suite_scenario6_concorrente_bloccante, "controllo messaggio letto", 
			test_scenario6_check_return_message))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }
/*========================================RUN SCENARIO 7=====================================================*/

/* add a suite SCENARIO  NON BLOCCANTE to the registry */
	suite_scenario7_concorrente_non_bloccante_buffer_unitario = CU_add_suite("test scenario 7: put_non_bloccante concorrente", setup_scenario7_put_non_bloccante_concorrente_buffer_unitario, clean);
	if (NULL == suite_scenario7_concorrente_non_bloccante_buffer_unitario) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario7_concorrente_non_bloccante_buffer_unitario, "controllo contenuto buffer", 
			test_scenario7_check_content_message)) || 
		(NULL == CU_add_test(suite_scenario7_concorrente_non_bloccante_buffer_unitario, "controllo numero messaggi in buffer", test_scenario7_check_number_message_buffer))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }


/*========================================RUN SCENARIO 8======================================================*/

/* add a suite SCENARIO 8 BLOCCANTE to the registry */
	suite_scenario8_concorrente_bloccante = CU_add_suite("test scenario 8 bloccante: Due consumatori su un buffer unitario pieno",  setup_scenario8_concorrente_bloccante, clean);
	if (NULL == suite_scenario8_concorrente_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario8_concorrente_bloccante, "controllo contenuto buffer", 
			test_scenario8_check_content_message)) || 
		(NULL == CU_add_test(suite_scenario8_concorrente_bloccante,
			"controllo numero messaggi in buffer", test_scenario8_check_number_message_buffer))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }

/* add a suite SCENARIO 8 NON BLOCCANTE to the registry */
	suite_scenario8_concorrente_non_bloccante = CU_add_suite("test scenario 8 non_bloccante: Due consumatori su un buffer unitario pieno",  setup_scenario8_concorrente_non_bloccante, clean);
	if (NULL == suite_scenario8_concorrente_non_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario8_concorrente_non_bloccante, "controllo contenuto buffer", 
			test_scenario8_check_content_message)) || 
		(NULL == CU_add_test(suite_scenario8_concorrente_non_bloccante, "controllo numero messaggi in buffer", 
			test_scenario8_check_number_message_buffer))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 } 

/*========================================RUN SCENARIO 9======================================================*/

/* add a suite SCENARIO 9 BLOCCANTE to the registry */
	suite_scenario9_put_concorrente_bloccante = CU_add_suite("test scenario 9: put_bloccante concorrente",  
		setup_scenario9_put_concorrente_bloccante, clean);
	if (NULL == suite_scenario9_put_concorrente_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario9_put_concorrente_bloccante, "controllo numero messaggi in buffer", 
			test_scenario9_check_number_message_buffer)) || 
		(NULL == CU_add_test(suite_scenario9_put_concorrente_bloccante, "controllo indice D", 
			test_scenario9_check_indice_D))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }


/* add a suite SCENARIO 9 NON BLOCCANTE to the registry */
	suite_scenario9_put_concorrente_non_bloccante = CU_add_suite("test scenario 9: put_non_bloccante concorrente",  
		setup_scenario9_put_concorrente_non_bloccante, clean);
	if (NULL == suite_scenario9_put_concorrente_non_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario9_put_concorrente_non_bloccante, "controllo numero messaggi in buffer", 
			test_scenario9_check_number_message_buffer))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }

/*========================================RUN SCENARIO 10======================================================*/

/* add a suite SCENARIO 10 to the registry */
	suite_scenario10_put_concorrente_buffer_pieno = CU_add_suite("test scenario 10: put_bloccante e non_bloccante concorrente buffer pieno", setup_scenario10_put_concorrente_buffer_pieno, clean);
	if (NULL == suite_scenario10_put_concorrente_buffer_pieno) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario10_put_concorrente_buffer_pieno, "controllo numero messaggi in buffer", 
			test_scenario10_check_number_message_buffer)) || 
		(NULL == CU_add_test(suite_scenario10_put_concorrente_buffer_pieno, "controllo indice D", 
			test_scenario10_check_indice_D))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }

/*========================================RUN SCENARIO 11======================================================*/

/* add a suite SCENARIO 11 to the registry */
	suite_scenario11_put_concorrente_bloccante = CU_add_suite("test scenario 11: put_bloccante concorrente buffer vuoto", setup_scenario11_put_concorrente_bloccante, clean);
	if (NULL == suite_scenario11_put_concorrente_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario11_put_concorrente_bloccante, "controllo numero messaggi in buffer", 
			test_scenario11_check_number_message_buffer)) || 
		(NULL == CU_add_test(suite_scenario11_put_concorrente_bloccante, "controllo indice D", 
			test_scenario11_check_indice_D)) || 
		(NULL == CU_add_test(suite_scenario11_put_concorrente_bloccante, "controllo indice T", 
			test_scenario11_check_indice_T))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }

/*========================================RUN SCENARIO 12======================================================*/

/* add a suite SCENARIO 12 BLOCCANTE to the registry */
	suite_scenario12_get_concorrente_bloccante = CU_add_suite("test scenario 12: get_bloccante concorrente",  
		setup_scenario12_get_concorrente_bloccante, clean);
	if (NULL == suite_scenario12_get_concorrente_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario12_get_concorrente_bloccante, "controllo contenuto buffer", 
				test_scenario12_check_content_message)) || 
		(NULL == CU_add_test(suite_scenario12_get_concorrente_bloccante, "controllo indici buffer", test_scenario12_check_number_message_buffer)) ||
		(NULL == CU_add_test(suite_scenario12_get_concorrente_bloccante, "controllo indici T", test_scenario12_check_indice_T)) ||
		(NULL == CU_add_test(suite_scenario12_get_concorrente_bloccante, "controllo indici D", test_scenario12_check_indice_D))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }

/*========================================RUN SCENARIO 13======================================================*/

/* add a suite SCENARIO 13 BLOCCANTE to the registry */
	suite_scenario13_put_get_concorrente_bloccante_buffer_unitario = CU_add_suite("test scenario 13: put_bloccante, get_bloccante concorrente buffer unitario", setup_scenario13_put_get_concorrente_bloccante_buffer_unitario, clean);
	if (NULL == suite_scenario13_put_get_concorrente_bloccante_buffer_unitario) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario13_put_get_concorrente_bloccante_buffer_unitario, "controllo contenuto buffer", 
			test_scenario13_check_content_message)) || 
		(NULL == CU_add_test(suite_scenario13_put_get_concorrente_bloccante_buffer_unitario, 
			"controllo indici buffer", test_scenario13_check_number_message_buffer)) || 
		(NULL == CU_add_test(suite_scenario13_put_get_concorrente_bloccante_buffer_unitario, "controllo indici T", test_scenario13_check_indice_T)) || 
		(NULL == CU_add_test(suite_scenario13_put_get_concorrente_bloccante_buffer_unitario, "controllo indici D", test_scenario13_check_indice_D))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }

/*========================================RUN SCENARIO 14======================================================*/

/* add a suite SCENARIO 14 BLOCCANTE to the registry */
	suite_scenario14_put_get_concorrente_bloccante = CU_add_suite("test scenario 14: put_bloccante, get_bloccante concorrente", setup_scenario14_put_get_concorrente_bloccante, clean);
	if (NULL == suite_scenario14_put_get_concorrente_bloccante) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suite_scenario14_put_get_concorrente_bloccante, "controllo contenuto buffer", 
			test_scenario14_check_content_message)) || 
		(NULL == CU_add_test(suite_scenario14_put_get_concorrente_bloccante, 
			"controllo indici buffer", test_scenario14_check_number_message_buffer)) || 
		(NULL == CU_add_test(suite_scenario14_put_get_concorrente_bloccante, "controllo indici T", test_scenario14_check_indice_T)) || 
		(NULL == CU_add_test(suite_scenario14_put_get_concorrente_bloccante, "controllo indici D", test_scenario14_check_indice_D))){
		CU_cleanup_registry();
	 	return CU_get_error();
	 }

	CU_basic_set_mode(CU_BRM_VERBOSE);
   	CU_basic_run_tests();
   	CU_cleanup_registry();
   	return CU_get_error();	
}

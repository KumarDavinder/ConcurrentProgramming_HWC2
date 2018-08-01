#include <CUnit/Basic.h>
#include "provider.h"
#include "poison_pill.h"
#include "dispatcher.h"
#include "reader.h"
#include "accepter.h"
#include "list.h"
#include <pthread.h>

list_readers* list_readers_current; 
buffer_t* new_provider_buffer;
int cod_return_thread_dispatcher;
int cod_return_thread_reader;
msg_t* poison_pill_message;
arg_dispatcher* argument_dispatcher;

unsigned int SIZE_BUFFER_READER; 
unsigned int NUM_MESSAGES_REQUESTS;

int clean(void){
	buffer_destroy(new_provider_buffer);
	list_readers_destroy(list_readers_current);
	free(argument_dispatcher);
	cod_return_thread_dispatcher = 12345;
	cod_return_thread_reader = 12345;
	return 0;
}

void init_struct_dispatcher(){
	argument_dispatcher = (arg_dispatcher*)malloc(sizeof(arg_dispatcher));

	list_readers_current = (list_readers*)malloc(sizeof(list_readers));	//creo la struttura che ospita la lista e mutex
	pthread_mutex_init(&list_readers_current->list_mutex,NULL);	//inizializzo il mutex sulla lista
	list_readers_current->readers = list_init();	//inizializzo la lista dei readers

	argument_dispatcher->list_readers_current = list_readers_current;	
}

void init_struct_reader(){

}

/*==============================================================================================================*/
/*=================================SETUP0 empty_buffer empty_readers_list=======================================*/
/*==============================================================================================================*/

int setup_empty_buffer_empty_readers_list(void){
	init_struct_dispatcher();	//creo argomenti per il thread
	new_provider_buffer = buffer_init(1);
	argument_dispatcher->provider_buffer = new_provider_buffer; //argomenti del thread creati
	pthread_t dispatcher_thread;
	pthread_create(&dispatcher_thread, NULL,dispatcher_init, (void*)argument_dispatcher);

	poison_pill_message = msg_init_pill("poison_pill");
	put_bloccante(new_provider_buffer,poison_pill_message);	//se non faccio la put rimane su get_bloccante

	pthread_join(dispatcher_thread,(void*)&cod_return_thread_dispatcher);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_empty_buffer_empty_readers_list(void){
	CU_ASSERT(cod_return_thread_dispatcher == 5);
}
void test_check_content_buffer_empty_buffer_empty_readers_list(void){
	CU_ASSERT(new_provider_buffer->cells[0] == NULL);
}

/*==============================================================================================================*/
/*==============================SETUP1 more_msg_in_buffer_empty_readers_list====================================*/
/*==============================================================================================================*/

int setup_more_msg_in_buffer_empty_readers_list(void){
	init_struct_dispatcher();	//creo argomenti per il thread
	new_provider_buffer = buffer_init(5);

	msg_t* msg0 = msg_init("messaggio0");
	msg_t* msg1 = msg_init("messaggio1");
	msg_t* msg2 = msg_init("messaggio2");
	poison_pill_message = msg_init_pill("poison_pill");

	put_bloccante(new_provider_buffer,msg0);
	put_bloccante(new_provider_buffer,msg1);
	put_bloccante(new_provider_buffer,msg2);
	put_bloccante(new_provider_buffer,poison_pill_message);

	argument_dispatcher->provider_buffer = new_provider_buffer; 

	pthread_t dispatcher_thread;
	pthread_create(&dispatcher_thread, NULL,dispatcher_init, (void*)argument_dispatcher);
	pthread_join(dispatcher_thread,(void*)&cod_return_thread_dispatcher);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_more_msg_in_buffer_empty_readers_list(void){
	CU_ASSERT(cod_return_thread_dispatcher == 5);
}
void test_check_content_buffer_more_msg_in_buffer_empty_readers_list(void){
	CU_ASSERT(new_provider_buffer->cells[0] == NULL);
	CU_ASSERT(new_provider_buffer->cells[1] == NULL);
	CU_ASSERT(new_provider_buffer->cells[2] == NULL);
	CU_ASSERT(new_provider_buffer->cells[3] == NULL);
}

/*==============================================================================================================*/
/*====================================SETUP2 poisonpill_and_one_reader==========================================*/
/*==============================================================================================================*/

int setup_poisonpill_and_one_reader(void){
	init_struct_dispatcher();	//creo argomenti per il thread
	new_provider_buffer = buffer_init(1);
	argument_dispatcher->provider_buffer = new_provider_buffer; //argomenti del thread creati

	argument_thread_reader* argument_reader = (argument_thread_reader*)malloc(sizeof(argument_thread_reader));
	argument_reader->list_readers_current = list_readers_current;	
	reader_t* new_reader = reader_init("reader0", 1);
	argument_reader->reader = new_reader;

	addElement(list_readers_current->readers, new_reader);

	pthread_t reader_thread;
	pthread_create(&reader_thread, NULL,reader_thread_init, (void*)argument_reader);

	poison_pill_message = msg_init_pill("poison_pill");
	put_bloccante(new_provider_buffer,poison_pill_message);	//se non faccio la put rimane su get_bloccante

	pthread_t dispatcher_thread;
	pthread_create(&dispatcher_thread, NULL,dispatcher_init, (void*)argument_dispatcher);
	
	pthread_join(dispatcher_thread,(void*)&cod_return_thread_dispatcher);
	pthread_join(reader_thread, NULL);

	free(argument_reader);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_poisonpill_and_one_reader(void){
	CU_ASSERT(cod_return_thread_dispatcher == 5);
}
void test_check_content_buffer_poisonpill_and_one_reader(void){
	CU_ASSERT(new_provider_buffer->cells[0] == NULL);
}
void test_check_size_list_readers_poisonpill_and_one_reader(void){
	CU_ASSERT(size(list_readers_current->readers) == 0);
}

/*==============================================================================================================*/
/*===================================SETUP3 check dispatcher kill reader========================================*/
/*==============================================================================================================*/

int setup_dispatcher_kill_reader(void){
	init_struct_dispatcher();	//creo argomenti per il thread
	new_provider_buffer = buffer_init(3);
	argument_dispatcher->provider_buffer = new_provider_buffer; //argomenti del thread creati

	argument_thread_reader* argument_reader = (argument_thread_reader*)malloc(sizeof(argument_thread_reader));
	argument_reader->list_readers_current = list_readers_current;	
	reader_t* new_reader = reader_init("reader0", 1);
	argument_reader->reader = new_reader;

	addElement(list_readers_current->readers, new_reader);

	pthread_t reader_thread;
	pthread_create(&reader_thread, NULL, reader_thread_init, (void*)argument_reader);

	msg_t* msg1 = msg_init("messaggio1");
	msg_t* msg2 = msg_init("messaggio2");
	poison_pill_message = msg_init_pill("poison_pill");
	put_bloccante(new_provider_buffer, msg1);
	put_bloccante(new_provider_buffer, msg2);
	put_bloccante(new_provider_buffer, poison_pill_message);

	pthread_t dispatcher_thread;
	pthread_create(&dispatcher_thread, NULL, dispatcher_init, (void*)argument_dispatcher);

	pthread_join(dispatcher_thread, (void*)&cod_return_thread_dispatcher);
	pthread_join(reader_thread, (void*)&cod_return_thread_reader);

	free(argument_reader);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_dispatcher_kill_reader(void){
	CU_ASSERT(cod_return_thread_dispatcher == 5);
}
void test_check_return_cod_threadReader_dispatcher_kill_reader(void){
	CU_ASSERT(cod_return_thread_reader == 4);
}
void test_check_content_buffer_dispatcher_kill_reader(void){
	CU_ASSERT(new_provider_buffer->cells[0] == NULL);
	CU_ASSERT(new_provider_buffer->cells[1] == NULL);
	CU_ASSERT(new_provider_buffer->cells[2] == NULL);
}
void test_check_size_list_readers_dispatcher_kill_reader(void){
	CU_ASSERT(size(list_readers_current->readers) == 0);
}


/*==============================================================================================================*/
/*=====================================SETUP4 one_msg_and_one_reader============================================*/
/*==============================================================================================================*/

int setup_one_msg_and_one_reader(void){
	init_struct_dispatcher();	//creo argomenti per il thread
	new_provider_buffer = buffer_init(2);
	argument_dispatcher->provider_buffer = new_provider_buffer; //argomenti del thread creati

	argument_thread_reader* argument_reader = (argument_thread_reader*)malloc(sizeof(argument_thread_reader));
	argument_reader->list_readers_current = list_readers_current;	
	reader_t* new_reader = reader_init("reader0", 10);
	argument_reader->reader = new_reader;

	addElement(list_readers_current->readers, new_reader);

	pthread_t reader_thread;
	pthread_create(&reader_thread, NULL, reader_thread_init, (void*)argument_reader);

	msg_t* msg = msg_init("messaggio0");
	poison_pill_message = msg_init_pill("poison_pill");
	put_bloccante(new_provider_buffer, msg);
	put_bloccante(new_provider_buffer, poison_pill_message);

	pthread_t dispatcher_thread;
	pthread_create(&dispatcher_thread, NULL, dispatcher_init, (void*)argument_dispatcher);

	pthread_join(dispatcher_thread, (void*)&cod_return_thread_dispatcher);
	pthread_join(reader_thread, NULL);

	free(argument_reader);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_one_msg_and_one_reader(void){
	CU_ASSERT(cod_return_thread_dispatcher == 5);
}
void test_check_content_buffer_one_msg_and_one_reader(void){
	CU_ASSERT(new_provider_buffer->cells[0] == NULL);
	CU_ASSERT(new_provider_buffer->cells[1] == NULL);
}
void test_check_size_list_readers_one_msg_and_one_reader(void){
	CU_ASSERT(size(list_readers_current->readers) == 0);
}

/*==============================================================================================================*/
/*=================================SETUP5 poisonpill_and_more_readers=======================================*/
/*==============================================================================================================*/

int setup_poisonpill_and_more_readers(void){
	init_struct_dispatcher();	//creo argomenti per il thread
	new_provider_buffer = buffer_init(1);
	argument_dispatcher->provider_buffer = new_provider_buffer; //argomenti del thread creati

	argument_thread_reader* argument_reader1 = (argument_thread_reader*)malloc(sizeof(argument_thread_reader));
	argument_reader1->list_readers_current = list_readers_current;	
	reader_t* new_reader1 = reader_init("reader0", 10);
	argument_reader1->reader = new_reader1;

	argument_thread_reader* argument_reader2 = (argument_thread_reader*)malloc(sizeof(argument_thread_reader));
	argument_reader2->list_readers_current = list_readers_current;	
	reader_t* new_reader2 = reader_init("reader1", 10);
	argument_reader2->reader = new_reader2;

	addElement(list_readers_current->readers, new_reader1);
	addElement(list_readers_current->readers, new_reader2);

	pthread_t reader_thread1;
	pthread_t reader_thread2;
	pthread_create(&reader_thread1, NULL, reader_thread_init, (void*)argument_reader1);
	pthread_create(&reader_thread2, NULL, reader_thread_init, (void*)argument_reader2);

	poison_pill_message = msg_init_pill("poison_pill");
	put_bloccante(new_provider_buffer, poison_pill_message);	//se non faccio la put rimane su get_bloccante

	pthread_t dispatcher_thread;
	pthread_create(&dispatcher_thread, NULL, dispatcher_init, (void*)argument_dispatcher);

	pthread_join(reader_thread1, NULL);
	pthread_join(reader_thread2, NULL);
	pthread_join(dispatcher_thread, (void*)&cod_return_thread_dispatcher);

	free(argument_reader1);
	free(argument_reader2);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_poisonpill_and_more_readers(void){
	CU_ASSERT(cod_return_thread_dispatcher == 5);
}
void test_check_content_buffer_poisonpill_and_more_readers(void){
	CU_ASSERT(new_provider_buffer->cells[0] == NULL);
}
void test_check_size_list_readers_poisonpill_and_more_readers(void){
	CU_ASSERT(size(list_readers_current->readers) == 0);
}



/*==============================================================================================================*/
/*=================================SETUP6 more_msg_and_more_readers=======================================*/
/*==============================================================================================================*/

int setup_more_msg_and_more_readers(void){
	init_struct_dispatcher();	//creo argomenti per il thread
	new_provider_buffer = buffer_init(3);
	argument_dispatcher->provider_buffer = new_provider_buffer; //argomenti del thread creati

	//inserisco due messaggi	
	msg_t* msg1 = msg_init("messaggio0");
	msg_t* msg2 = msg_init("messaggio1");
	poison_pill_message = msg_init_pill("poison_pill");
	put_bloccante(new_provider_buffer, msg1);
	put_bloccante(new_provider_buffer, msg2);
	put_bloccante(new_provider_buffer, poison_pill_message);

	argument_thread_reader* argument_reader1 = (argument_thread_reader*)malloc(sizeof(argument_thread_reader));
	argument_reader1->list_readers_current = list_readers_current;	
	reader_t* new_reader1 = reader_init("reader0", 10);
	argument_reader1->reader = new_reader1;
	
	argument_thread_reader* argument_reader2 = (argument_thread_reader*)malloc(sizeof(argument_thread_reader));
	argument_reader2->list_readers_current = list_readers_current;	
	reader_t* new_reader2 = reader_init("reader1", 10);
	argument_reader2->reader = new_reader2;

	argument_thread_reader* argument_reader3 = (argument_thread_reader*)malloc(sizeof(argument_thread_reader));
	argument_reader3->list_readers_current = list_readers_current;	
	reader_t* new_reader3 = reader_init("reader2", 10);
	argument_reader3->reader = new_reader3;

	addElement(list_readers_current->readers, new_reader1);
	addElement(list_readers_current->readers, new_reader2);
	addElement(list_readers_current->readers, new_reader3);

	pthread_t reader_thread1;
	pthread_t reader_thread2;
	pthread_t reader_thread3;
	pthread_create(&reader_thread1, NULL,reader_thread_init, (void*)argument_reader1);
	pthread_create(&reader_thread2, NULL,reader_thread_init, (void*)argument_reader2);
	pthread_create(&reader_thread3, NULL,reader_thread_init, (void*)argument_reader3);

	pthread_t dispatcher_thread;
	pthread_create(&dispatcher_thread, NULL,dispatcher_init, (void*)argument_dispatcher);

	pthread_join(reader_thread1, NULL);
	pthread_join(reader_thread2, NULL);
	pthread_join(reader_thread3, NULL);
	pthread_join(dispatcher_thread,(void*)&cod_return_thread_dispatcher);

	free(argument_reader1);
	free(argument_reader2);
	free(argument_reader3);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_more_msg_and_more_readers(void){
	CU_ASSERT(cod_return_thread_dispatcher == 5);
}
void test_check_content_buffer_more_msg_and_more_readers(void){
	CU_ASSERT(new_provider_buffer->cells[0] == NULL);
	CU_ASSERT(new_provider_buffer->cells[1] == NULL);
	CU_ASSERT(new_provider_buffer->cells[2] == NULL);
}
void test_check_size_list_readers_more_msg_and_more_readers(void){
	CU_ASSERT(size(list_readers_current->readers) == 0);
}

/*=========================================RUN TEST SCENARI======================================================*/

int main(){
	CU_pSuite suite_empty_buffer_empty_readers_list = NULL;
	CU_pSuite suite_more_msg_in_buffer_empty_readers_list = NULL;
	CU_pSuite suite_poisonpill_and_one_reader = NULL;
	CU_pSuite suite_dispatcher_kill_reader = NULL;
	CU_pSuite suite_one_msg_and_one_reader = NULL;
	CU_pSuite suite_poisonpill_and_more_readers = NULL;
	CU_pSuite suite_more_msg_and_more_readers = NULL;


	if (CUE_SUCCESS != CU_initialize_registry()){
		return CU_get_error();
	}
/*===========================================SUITE0===============================================*/
	suite_empty_buffer_empty_readers_list = CU_add_suite("Dispatcher: empty_buffer and empty_readers_list", setup_empty_buffer_empty_readers_list, clean);
	if (NULL == suite_empty_buffer_empty_readers_list) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_empty_buffer_empty_readers_list, "thread return value", test_check_return_cod_th_empty_buffer_empty_readers_list)) || 
		(NULL == CU_add_test(suite_empty_buffer_empty_readers_list, "check content buffer", test_check_content_buffer_empty_buffer_empty_readers_list))){
		CU_cleanup_registry();
		return CU_get_error();
	}

/*===========================================SUITE1===============================================*/
	suite_more_msg_in_buffer_empty_readers_list = CU_add_suite("Dispatcher: more msg in buffer and empty readers list", setup_more_msg_in_buffer_empty_readers_list, clean);
	if (NULL == suite_more_msg_in_buffer_empty_readers_list) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_more_msg_in_buffer_empty_readers_list, "return value thread", test_check_return_cod_th_more_msg_in_buffer_empty_readers_list)) || 
		(NULL == CU_add_test(suite_more_msg_in_buffer_empty_readers_list, "check content buffer", test_check_content_buffer_more_msg_in_buffer_empty_readers_list))){
		CU_cleanup_registry();
		return CU_get_error();
	}

/*===========================================SUITE2===============================================*/
	suite_poisonpill_and_one_reader = CU_add_suite("Dispatcher: poisonpill and one reader", setup_poisonpill_and_one_reader, clean);
	if (NULL == suite_poisonpill_and_one_reader) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_poisonpill_and_one_reader, "return value thread", test_check_return_cod_th_poisonpill_and_one_reader)) || 
		(NULL == CU_add_test(suite_poisonpill_and_one_reader, "check content buffer", test_check_content_buffer_poisonpill_and_one_reader)) || 
		(NULL == CU_add_test(suite_poisonpill_and_one_reader, "check size list readers", test_check_size_list_readers_poisonpill_and_one_reader))){
		CU_cleanup_registry();
		return CU_get_error();
	}
/*===========================================SUITE3===============================================*/
	suite_dispatcher_kill_reader = CU_add_suite("Dispatcher: Dispatcher kill reader", setup_dispatcher_kill_reader, clean);
	if (NULL == suite_dispatcher_kill_reader) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_dispatcher_kill_reader, "return value thread dispatcher", test_check_return_cod_th_dispatcher_kill_reader)) || 
		(NULL == CU_add_test(suite_dispatcher_kill_reader, "return value thread reader", test_check_return_cod_threadReader_dispatcher_kill_reader)) || 
		(NULL == CU_add_test(suite_dispatcher_kill_reader, "check content buffer", test_check_content_buffer_dispatcher_kill_reader)) || 
		(NULL == CU_add_test(suite_dispatcher_kill_reader, "check size list readers", test_check_size_list_readers_dispatcher_kill_reader))){
		CU_cleanup_registry();
		return CU_get_error();
	}

/*===========================================SUITE4===============================================*/
	suite_one_msg_and_one_reader = CU_add_suite("Dispatcher: one message and one reader", setup_one_msg_and_one_reader, clean);
	if (NULL == suite_one_msg_and_one_reader) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_one_msg_and_one_reader, "return value thread", test_check_return_cod_th_one_msg_and_one_reader)) || 
		(NULL == CU_add_test(suite_one_msg_and_one_reader, "check content buffer", test_check_content_buffer_one_msg_and_one_reader)) || 
		(NULL == CU_add_test(suite_one_msg_and_one_reader, "check size list readers", test_check_size_list_readers_one_msg_and_one_reader))){
		CU_cleanup_registry();
		return CU_get_error();
	}

/*===========================================SUITE5===============================================*/
	suite_poisonpill_and_more_readers = CU_add_suite("Dispatcher: poisonpill and more readers", setup_poisonpill_and_more_readers, clean);
	if (NULL == suite_poisonpill_and_more_readers) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_poisonpill_and_more_readers, "return value thread", test_check_return_cod_th_poisonpill_and_more_readers)) || 
		(NULL == CU_add_test(suite_poisonpill_and_more_readers, "check content buffer", test_check_content_buffer_poisonpill_and_more_readers)) || 
		(NULL == CU_add_test(suite_poisonpill_and_more_readers, "check size list readers", test_check_size_list_readers_poisonpill_and_more_readers))){
		CU_cleanup_registry();
		return CU_get_error();
	}

/*===========================================SUITE6===============================================*/
	suite_more_msg_and_more_readers = CU_add_suite("Dispatcher: more message and more readers", setup_more_msg_and_more_readers, clean);
	if (NULL == suite_more_msg_and_more_readers) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_more_msg_and_more_readers, "return value thread", test_check_return_cod_th_more_msg_and_more_readers)) || 
		(NULL == CU_add_test(suite_more_msg_and_more_readers, "check content buffer", test_check_content_buffer_more_msg_and_more_readers)) || 
		(NULL == CU_add_test(suite_more_msg_and_more_readers, "check size list readers", test_check_size_list_readers_more_msg_and_more_readers))){
		CU_cleanup_registry();
		return CU_get_error();
	}

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}


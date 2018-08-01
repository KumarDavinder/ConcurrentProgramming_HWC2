#include <CUnit/Basic.h>
#include "provider.h"
#include "poison_pill.h"
#include "reader.h"
#include "accepter.h"
#include "list.h"
#include <pthread.h>

argument_thread_reader* argument_reader;
reader_t* new_reader;
list_readers* list_readers_current;
int cod_return_thread;
msg_t* poison_pill_message;

unsigned int SIZE_BUFFER_READER = 1;
unsigned int NUM_MESSAGES_REQUESTS = 3;

int clean(void){
	cod_return_thread = 123;
	list_readers_destroy(list_readers_current);
	free(argument_reader);
	return 0;
}

void init_struct(){
	argument_reader = (argument_thread_reader*)malloc(sizeof(argument_thread_reader));

	list_readers_current = (list_readers*)malloc(sizeof(list_readers));	//creo la struttura che ospita la lista e mutex
	pthread_mutex_init(&list_readers_current->list_mutex,NULL);	//inizializzo il mutex sulla lista
	list_readers_current->readers = list_init();	//inizializzo la lista dei readers

	argument_reader->list_readers_current = list_readers_current;	
}
/*==============================================================================================================*/
/*====================================SETUP0 size_buff = 1 - msgs = 0===========================================*/
/*==============================================================================================================*/

int setup_only_poisonPill_msg_in_buffer(void){
	init_struct();	//creo argomenti per il thread
	new_reader = reader_init("reader0", 1);
	argument_reader->reader = new_reader; //argomenti del thread creati
	addElement(argument_reader->list_readers_current->readers, (void*)new_reader); //add reader alla lista

	msg_t* poisonpill = msg_init_pill("poison_pill");
	put_bloccante(new_reader->reader_buffer,poisonpill);

	pthread_t reader_thread;
	pthread_create(&reader_thread, NULL,reader_thread_init, (void*)argument_reader);
	pthread_join(reader_thread,(void*)&cod_return_thread);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_only_poisonPill_msg_in_buffer(void){
	CU_ASSERT(cod_return_thread == 3);
}

/*==============================================================================================================*/
/*=====================================SETUP1 size_buff = 1 - msgs = 1===========================================*/
/*==============================================================================================================*/

int setup_1msg_and_poisonPill_in_buffer(void){
	init_struct();	//creo argomenti per il thread
	new_reader = reader_init("reader0", 1);
	argument_reader->reader = new_reader; //argomenti del thread creati
	addElement(argument_reader->list_readers_current->readers, (void*)new_reader);
	pthread_t reader_thread;
	pthread_create(&reader_thread, NULL,reader_thread_init, (void*)argument_reader);

	msg_t* msg = msg_init("messaggio0");
	put_bloccante(new_reader->reader_buffer,msg);
	msg_t* poisonpill = msg_init_pill("poison_pill");
	put_bloccante(new_reader->reader_buffer,poisonpill);
	
	pthread_join(reader_thread,(void*)&cod_return_thread);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_1msg_and_poisonPill_in_buffer(void){
	CU_ASSERT(cod_return_thread == 3);
}


/*==============================================================================================================*/
/*====================================SETUP2 size_buff = 1 - msgs = 3===========================================*/
/*==============================================================================================================*/
int setup_2msgs_and_poisonPill_in_buffer(void){
	init_struct();	//creo argomenti per il thread
	new_reader = reader_init("reader0", 1);
	argument_reader->reader = new_reader; //argomenti del thread creati
	addElement(argument_reader->list_readers_current->readers, (void*)new_reader);
	pthread_t reader_thread;
	pthread_create(&reader_thread, NULL,reader_thread_init, (void*)argument_reader);

	msg_t* msg1 = msg_init("messaggio0");
	put_bloccante(new_reader->reader_buffer,msg1);
	msg_t* msg2 = msg_init("messaggio1");
	put_bloccante(new_reader->reader_buffer,msg2);
	msg_t* poisonpill = msg_init_pill("poison_pill");
	put_bloccante(new_reader->reader_buffer,poisonpill);
	
	pthread_join(reader_thread,(void*)&cod_return_thread);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_2msgs_and_poisonPill_in_buffer(void){
	CU_ASSERT(cod_return_thread == 3);
}


/*==============================================================================================================*/
/*========================================SETUP3 flag_deleted = 0===============================================*/
/*==============================================================================================================*/
int setup_flag_deleted_0(void){
	init_struct();	//creo argomenti per il thread
	new_reader = reader_init("reader0", 1);
	new_reader->flag_delete = 0;
	argument_reader->reader = new_reader; //argomenti del thread creati
	addElement(argument_reader->list_readers_current->readers, (void*)new_reader);
	pthread_t reader_thread;
	pthread_create(&reader_thread, NULL,reader_thread_init, (void*)argument_reader);
	pthread_join(reader_thread,(void*)&cod_return_thread);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_flag_deleted_0(void){
	CU_ASSERT(cod_return_thread == 4);
}


/*=========================================RUN TEST SCENARI======================================================*/

int main(){
	CU_pSuite suite_only_poisonPill_msg_in_buffer = NULL;
	CU_pSuite suite_1msg_and_poisonPill_in_buffer = NULL;
	CU_pSuite suite_2msgs_and_poisonPill_in_buffer = NULL;
	CU_pSuite suite_flag_deleted_0 = NULL;

	if (CUE_SUCCESS != CU_initialize_registry()){
		return CU_get_error();
	}
/*===========================================SUITE 0===============================================*/
	suite_only_poisonPill_msg_in_buffer = CU_add_suite("Reader: only poison_pill msg in reader_buffer", setup_only_poisonPill_msg_in_buffer, clean);
	if (NULL == suite_only_poisonPill_msg_in_buffer) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if (NULL == CU_add_test(suite_only_poisonPill_msg_in_buffer, "return value thread", test_check_return_cod_th_only_poisonPill_msg_in_buffer)){
		CU_cleanup_registry();
		return CU_get_error();
	}

/*===========================================SUITE 1===============================================*/
	suite_1msg_and_poisonPill_in_buffer = CU_add_suite("Reader: 1msg_and_poisonPill_in_buffer", setup_1msg_and_poisonPill_in_buffer, clean);
	if (NULL == suite_1msg_and_poisonPill_in_buffer) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if (NULL == CU_add_test(suite_1msg_and_poisonPill_in_buffer, "return value thread", test_check_return_cod_th_1msg_and_poisonPill_in_buffer)){
		CU_cleanup_registry();
		return CU_get_error();
	}

/*===========================================SUITE 2===============================================*/
	suite_2msgs_and_poisonPill_in_buffer = CU_add_suite("Reader: 2msgs_and_poisonPill_in_buffer", setup_2msgs_and_poisonPill_in_buffer, clean);
	if (NULL == suite_2msgs_and_poisonPill_in_buffer) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if (NULL == CU_add_test(suite_2msgs_and_poisonPill_in_buffer, "return value thread", test_check_return_cod_th_2msgs_and_poisonPill_in_buffer)){
		CU_cleanup_registry();
		return CU_get_error();
	}

/*===========================================SUITE 3===============================================*/
	suite_flag_deleted_0 = CU_add_suite("Reader: flag_deleted = 0", setup_flag_deleted_0, clean);
	if (NULL == suite_flag_deleted_0) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if (NULL == CU_add_test(suite_flag_deleted_0, "return value thread", test_check_return_cod_th_flag_deleted_0)){
		CU_cleanup_registry();
		return CU_get_error();
	}

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}


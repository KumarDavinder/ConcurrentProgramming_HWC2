#include <CUnit/Basic.h>
#include "provider.h"
#include "poison_pill.h"
#include "reader.h"
#include "accepter.h"
#include "list.h"
#include <pthread.h>



list_readers* list_readers_current; 
buffer_t* new_accepter_buffer;
accepter_arg* argument_accepter;
unsigned int SIZE_BUFFER_READER = 1;
unsigned int NUM_MESSAGES_REQUESTS = 3;

int cod_return_thread;
msg_t* poison_pill_message;

int clean(void){
	buffer_destroy(new_accepter_buffer);
	list_readers_destroy(list_readers_current);
	free(argument_accepter);
	cod_return_thread = 12345;
	return 0;
}

void init_struct_accepter(){
	argument_accepter = (accepter_arg*)malloc(sizeof(accepter_arg));

	list_readers_current = (list_readers*)malloc(sizeof(list_readers));	//creo la struttura che ospita la lista e mutex
	pthread_mutex_init(&list_readers_current->list_mutex,NULL);	//inizializzo il mutex sulla lista
	list_readers_current->readers = list_init();	//inizializzo la lista dei readers

	argument_accepter->list_readers_current = list_readers_current;	
}


/*==============================================================================================================*/
/*======================================SETUP0 read from empty_buffer===========================================*/
/*==============================================================================================================*/

int setup_empty_buffer(void){
	init_struct_accepter();	//creo argomenti per il thread
	new_accepter_buffer = buffer_init(1);
	argument_accepter->accepter_buffer = new_accepter_buffer; //argomenti del thread creati
	pthread_t accepter_thread;
	pthread_create(&accepter_thread, NULL,accepter_init, (void*)argument_accepter);
	poison_pill_message = msg_init_pill("poison_pill");
	put_bloccante(new_accepter_buffer,poison_pill_message);	//se non faccio la put rimane su get_bloccante

	pthread_join(accepter_thread,(void*)&cod_return_thread);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_empty_buffer(void){
	CU_ASSERT(cod_return_thread == 6);
}
void test_check_content_buffer_empty_buffer(void){
	CU_ASSERT(new_accepter_buffer->cells[0] == NULL);
}



/*==============================================================================================================*/
/*=========================SETUP1 one_msg_and_poison_pill_in_buffer==========================*/
/*==============================================================================================================*/

int setup_one_msg_and_poison_pill_in_buffer(void){
	init_struct_accepter();	//creo argomenti per il thread
	new_accepter_buffer = buffer_init(1);
	argument_accepter->accepter_buffer = new_accepter_buffer; //argomenti del thread creati
	pthread_t accepter_thread;
	pthread_create(&accepter_thread, NULL,accepter_init, (void*)argument_accepter);
	//inserisco messaggi	
	msg_t* msg = msg_init("reader0");
	poison_pill_message = msg_init_pill("poison_pill");
	put_bloccante(new_accepter_buffer,msg);
	put_bloccante(new_accepter_buffer,poison_pill_message);	//se non faccio la put rimane su get_bloccante

	pthread_join(accepter_thread,(void*)&cod_return_thread);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_one_msg_and_poison_pill_in_buffer(void){
	CU_ASSERT(cod_return_thread == 6);
}
void test_check_content_buffer_one_msg_and_poison_pill_in_buffer(void){
	CU_ASSERT(new_accepter_buffer->cells[0] == NULL);
}

void test_check_size_list_readers_one_msg_and_poison_pill_in_buffer(void){
	CU_ASSERT(size(list_readers_current->readers) == 1);
}

void test_check_list_readers_one_msg_and_poison_pill_in_buffer(void){
	iterator_t* iterator = iterator_init(list_readers_current->readers);
	reader_t* reader =((reader_t*)(iterator->currentNode->payload));
	CU_ASSERT_STRING_EQUAL(reader->name,"reader0");
}

/*==============================================================================================================*/
/*=========================SETUP2 more_msg_and_poison_pill_in_buffer==========================*/
/*==============================================================================================================*/

int setup_more_msg_and_poison_pill_in_buffer(void){
	init_struct_accepter();	//creo argomenti per il thread
	new_accepter_buffer = buffer_init(2);
	argument_accepter->accepter_buffer = new_accepter_buffer; //argomenti del thread creati
	pthread_t accepter_thread;
	pthread_create(&accepter_thread, NULL,accepter_init, (void*)argument_accepter);
	//inserisco messaggi
	msg_t* msg0 = msg_init("reader0");
	msg_t* msg1 = msg_init("reader1");
	msg_t* msg2 = msg_init("reader2");
	poison_pill_message = msg_init_pill("poison_pill");
	put_bloccante(new_accepter_buffer,msg0);
	put_bloccante(new_accepter_buffer,msg1);
	put_bloccante(new_accepter_buffer,msg2);
	put_bloccante(new_accepter_buffer,poison_pill_message);	//se non faccio la put rimane su get_bloccante

	pthread_join(accepter_thread,(void*)&cod_return_thread);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_more_msg_and_poison_pill_in_buffer(void){
	CU_ASSERT(cod_return_thread == 6);
}
void test_check_content_buffer_more_msg_and_poison_pill_in_buffer(void){
	CU_ASSERT(new_accepter_buffer->cells[0] == NULL);
	CU_ASSERT(new_accepter_buffer->cells[1] == NULL);
}

void test_check_size_list_readers_more_msg_and_poison_pill_in_buffer(void){
	CU_ASSERT(size(list_readers_current->readers) == 3);
}

void test_check_readers_in_list_more_msg_and_poison_pill_in_buffer(void){
	iterator_t* iterator = iterator_init(list_readers_current->readers);
	reader_t* reader;

	reader =((reader_t*)(iterator->currentNode->payload));
	CU_ASSERT_STRING_EQUAL(reader->name,"reader0");
	next(iterator);

	reader =((reader_t*)(iterator->currentNode->payload));
	CU_ASSERT_STRING_EQUAL(reader->name,"reader1");
	next(iterator);

	reader =((reader_t*)(iterator->currentNode->payload));
	CU_ASSERT_STRING_EQUAL(reader->name,"reader2");

	iterator_destroy(iterator);
}
		
/*===============================================================*/
/*===============================================================*/

int main(){
	CU_pSuite suite_empty_buffer = NULL;
	CU_pSuite suite_one_msg_and_poison_pill_in_buffer = NULL;
	CU_pSuite suite_more_msg_and_poison_pill_in_buffer = NULL;
	

	if (CUE_SUCCESS != CU_initialize_registry()){
		return CU_get_error();
	}


	/*===========================================SUITE0===============================================*/
	suite_empty_buffer = CU_add_suite("Accepter: empty_buffer", setup_empty_buffer, clean);
	if (NULL == suite_empty_buffer) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_empty_buffer, "return value thread", test_check_return_cod_th_empty_buffer)) || 
		(NULL == CU_add_test(suite_empty_buffer, "check content buffer", test_check_content_buffer_empty_buffer))){
		CU_cleanup_registry();
		return CU_get_error();
	}
	/*===========================================SUITE1===============================================*/
	suite_one_msg_and_poison_pill_in_buffer = CU_add_suite("Accepter: one msg and poison_pill in buffer", setup_one_msg_and_poison_pill_in_buffer, clean);
	if (NULL == suite_one_msg_and_poison_pill_in_buffer) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_one_msg_and_poison_pill_in_buffer, "return value thread", test_check_return_cod_th_one_msg_and_poison_pill_in_buffer)) || 
		(NULL == CU_add_test(suite_one_msg_and_poison_pill_in_buffer, "check content buffer", test_check_content_buffer_one_msg_and_poison_pill_in_buffer)) || 
		(NULL == CU_add_test(suite_one_msg_and_poison_pill_in_buffer, "check size list readers", test_check_size_list_readers_one_msg_and_poison_pill_in_buffer)) || 
		(NULL == CU_add_test(suite_one_msg_and_poison_pill_in_buffer, "check list readers", test_check_list_readers_one_msg_and_poison_pill_in_buffer))){
		CU_cleanup_registry();
		return CU_get_error();
	}

/*===========================================SUITE2===============================================*/
	suite_more_msg_and_poison_pill_in_buffer = CU_add_suite("Accepter: more msg and poison_pill in buffer", setup_more_msg_and_poison_pill_in_buffer, clean);
	if (NULL == suite_more_msg_and_poison_pill_in_buffer) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_more_msg_and_poison_pill_in_buffer, "return value thread", test_check_return_cod_th_more_msg_and_poison_pill_in_buffer)) || 
		(NULL == CU_add_test(suite_more_msg_and_poison_pill_in_buffer, "check content buffer", test_check_content_buffer_more_msg_and_poison_pill_in_buffer)) || 
		(NULL == CU_add_test(suite_more_msg_and_poison_pill_in_buffer, "check size list readers", test_check_size_list_readers_more_msg_and_poison_pill_in_buffer)) || 
		(NULL == CU_add_test(suite_more_msg_and_poison_pill_in_buffer, "check list readers", test_check_readers_in_list_more_msg_and_poison_pill_in_buffer))){
		CU_cleanup_registry();
		return CU_get_error();
	}

	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}


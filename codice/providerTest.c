#include <CUnit/Basic.h>
#include "provider.h"
#include "poison_pill.h"
#include <pthread.h>


buffer_t* buffer_provider;
unsigned int NUM_MESSAGES_REQUESTS;
int cod_return_thread;
msg_t* poison_pill_message;

unsigned int SIZE_BUFFER_READER = 4;

int clean(void){
	buffer_destroy(buffer_provider);
	cod_return_thread = 1234; 
	return 0;
}

/*==============================================================================================================*/
/*====================================SETUP size_buff = 0 - num_msgs = 0========================================*/
/*==============================================================================================================*/

int setup_num_msgs0_size_buff0(void){
	buffer_provider = buffer_init(0);
	pthread_t provider_thread;
	NUM_MESSAGES_REQUESTS = 0;
	pthread_create(&provider_thread, NULL,provider_init, (void*)buffer_provider);
	pthread_join(provider_thread,(void*)&cod_return_thread);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_num_msgs0_size_buff0(void){
	CU_ASSERT(cod_return_thread == 0);
}

/*==============================================================================================================*/
/*====================================SETUP size_buff = 0 - num_msgs = 1========================================*/
/*==============================================================================================================*/

int setup_num_msgs1_size_buff0(void){
	buffer_provider = buffer_init(0);
	NUM_MESSAGES_REQUESTS = 1;
	pthread_t provider_thread;
	pthread_create(&provider_thread, NULL,provider_init, (void*)buffer_provider);
	pthread_join(provider_thread,(void*)&cod_return_thread);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_num_msgs1_size_buff0(void){
	CU_ASSERT(cod_return_thread == 0);
}

/*==============================================================================================================*/
/*====================================SETUP size_buff = 2 - num_msgs = 1========================================*/
/*==============================================================================================================*/

int setup_num_msgs1_size_buff2(void){
	buffer_provider = buffer_init(2);
	NUM_MESSAGES_REQUESTS = 1;
	pthread_t provider_thread;
	pthread_create(&provider_thread, NULL,provider_init, (void*)buffer_provider);
	pthread_join(provider_thread,(void*)&cod_return_thread);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_num_msgs1_size_buff2(void){
	CU_ASSERT(cod_return_thread == 1);
}

void test_check_poisonpill_in_buff_num_msgs1_size_buff2(void){
	poison_pill_message = buffer_provider->cells[1];
	CU_ASSERT(poison_pill_message == &POISON_PILL_MSG);
}

void test_check_msg_in_buff_num_msgs1_size_buff2(void){
	CU_ASSERT_STRING_EQUAL(buffer_provider->cells[0]->content, "messaggio0");
}

/*==============================================================================================================*/
/*===================================SETUP size_buff = 10 - num_msgs = 5========================================*/
/*==============================================================================================================*/

int setup_num_msgs5_size_buff10(void){
	buffer_provider = buffer_init(10);
	NUM_MESSAGES_REQUESTS = 5;
	pthread_t provider_thread;
	pthread_create(&provider_thread, NULL,provider_init, (void*)buffer_provider);
	pthread_join(provider_thread,(void*)&cod_return_thread);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_num_msgs5_size_buff10(void){
	CU_ASSERT(cod_return_thread == 1);
}

void test_check_poisonpill_in_buff_num_msgs5_size_buff10(void){
	poison_pill_message = buffer_provider->cells[5];
	CU_ASSERT(poison_pill_message == &POISON_PILL_MSG);
}

void test_check_msgs_in_buff_num_msgs5_size_buff10(void){
	CU_ASSERT_STRING_EQUAL(buffer_provider->cells[0]->content, "messaggio0");
	CU_ASSERT_STRING_EQUAL(buffer_provider->cells[1]->content, "messaggio1");
	CU_ASSERT_STRING_EQUAL(buffer_provider->cells[2]->content, "messaggio2");
	CU_ASSERT_STRING_EQUAL(buffer_provider->cells[3]->content, "messaggio3");
	CU_ASSERT_STRING_EQUAL(buffer_provider->cells[4]->content, "messaggio4");
}

/*==============================================================================================================*/
/*====================================SETUP size_buff = 10 - num_msgs = 0========================================*/
/*==============================================================================================================*/

int setup_num_msgs0_size_buff10(void){
	buffer_provider = buffer_init(10);
	pthread_t provider_thread;
	NUM_MESSAGES_REQUESTS = 0;
	pthread_create(&provider_thread, NULL,provider_init, (void*)buffer_provider);
	pthread_join(provider_thread,(void*)&cod_return_thread);
	return 0;
}

/*=============================test==================================*/
void test_check_return_cod_th_num_msgs0_size_buff10(void){
	CU_ASSERT(cod_return_thread == 1);
}

void test_check_poisonpill_in_buff_num_msgs0_size_buff10(void){
	poison_pill_message = buffer_provider->cells[0];
	CU_ASSERT(poison_pill_message == &POISON_PILL_MSG);
}
/*=====================================fine setup e test=========================================*/
/*===============================================================================================*/
/*===============================================================================================*/

/*=========================================RUN TEST SCENARI======================================================*/

int main(){
	CU_pSuite suite_num_msgs0_size_buff0 = NULL;
	CU_pSuite suite_num_msgs1_size_buff0 = NULL;
	CU_pSuite suite_num_msgs1_size_buff2 = NULL;
	CU_pSuite suite_num_msgs5_size_buff10 = NULL;
	CU_pSuite suite_num_msgs0_size_buff10 = NULL;

	if (CUE_SUCCESS != CU_initialize_registry()){
		return CU_get_error();
	}
/*===========================================SUITE0===============================================*/
	suite_num_msgs0_size_buff0 = CU_add_suite("provider: num_messaggi = 0 - size_buffer = 0", setup_num_msgs0_size_buff0, clean);
	if (NULL == suite_num_msgs0_size_buff0) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if (NULL == CU_add_test(suite_num_msgs0_size_buff0, "codice ritorno thread", test_check_return_cod_th_num_msgs0_size_buff0)){
		CU_cleanup_registry();
		return CU_get_error();
	}
/*===========================================SUITE1===============================================*/
	suite_num_msgs1_size_buff0 = CU_add_suite("provider: num_messaggi = 1 - size_buffer = 0", setup_num_msgs1_size_buff0, clean);
	if (NULL == suite_num_msgs1_size_buff0) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if (NULL == CU_add_test(suite_num_msgs1_size_buff0, "codice ritorno thread", test_check_return_cod_th_num_msgs1_size_buff0)){
		CU_cleanup_registry();
		return CU_get_error();
	}
/*===========================================SUITE2===============================================*/
	suite_num_msgs1_size_buff2 = CU_add_suite("provider: num_messaggi = 1 - size_buffer = 2", setup_num_msgs1_size_buff2, clean);
	if (NULL == suite_num_msgs1_size_buff2) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_num_msgs1_size_buff2, "codice ritorno thread", test_check_return_cod_th_num_msgs1_size_buff2)) || 
		(NULL == CU_add_test(suite_num_msgs1_size_buff2, "check message in buffer", test_check_msg_in_buff_num_msgs1_size_buff2))||
		(NULL == CU_add_test(suite_num_msgs1_size_buff2, "check poison_pill message", test_check_poisonpill_in_buff_num_msgs1_size_buff2) )){
		CU_cleanup_registry();
		return CU_get_error();
	}


/*===========================================SUITE3===============================================*/
	suite_num_msgs5_size_buff10 = CU_add_suite("provider: num_messaggi = 5 - size_buffer = 10", setup_num_msgs5_size_buff10, clean);
	if (NULL == suite_num_msgs5_size_buff10) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_num_msgs5_size_buff10, "codice ritorno thread", test_check_return_cod_th_num_msgs5_size_buff10)) ||
		(NULL == CU_add_test(suite_num_msgs5_size_buff10, "check poison_pill message", test_check_poisonpill_in_buff_num_msgs5_size_buff10))||
		(NULL == CU_add_test(suite_num_msgs5_size_buff10, "check message in buffer", test_check_msgs_in_buff_num_msgs5_size_buff10))){
		CU_cleanup_registry();
		return CU_get_error();
	}

/*===========================================SUITE4===============================================*/
	suite_num_msgs0_size_buff10 = CU_add_suite("provider: num_messaggi = 0 - size_buffer = 10", setup_num_msgs0_size_buff10, clean);
	if (NULL == suite_num_msgs0_size_buff10) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suite_num_msgs0_size_buff10, "codice ritorno thread", test_check_return_cod_th_num_msgs0_size_buff10)) ||
		(NULL == CU_add_test(suite_num_msgs0_size_buff10, "check poison_pill message", test_check_poisonpill_in_buff_num_msgs0_size_buff10))){
		CU_cleanup_registry();
		return CU_get_error();
	}
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}



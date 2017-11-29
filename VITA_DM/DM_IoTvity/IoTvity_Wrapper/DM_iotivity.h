#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
 #define EXTERNC extern "C"
 #else
 #define EXTERNC
 #endif

#include<stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_PTHREAD_H
#include <pthread.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <Windows.h>
#endif
#include <json-c/json.h>

#include<stdio.h>

#include<stdint.h>

#define MAX_NUM_OF_ATR 40


typedef void * handle_t;
//typedef void (*onGet_ptr)(json_object*);
//typedef void (*onPut_ptr)(json_object*);
//typedef void (*onobserve_ptr)(json_object*);
typedef void (*onGet_ptr)(handle_t,json_object*);
typedef void (*onPut_ptr)(handle_t,json_object*);
typedef void (*onobserve_ptr)(handle_t,json_object*);


//User level structure
typedef struct 
{
    char name[40];
	//Resource_list* handle;
	handle_t handle;
}Resource_info;

//Application structure
/*typedef struct NODE
{
    Resource_info *res;
    NODE *next;
}OnNode;
*/
typedef void (*disc_ptr)(Resource_info*);

int dm_init(int);

void DM_Discovery(disc_ptr);
void DM_Get(handle_t,onGet_ptr);
void DM_onGet(handle_t, json_object* global_json);

//void DM_Put(Resource_info*,json_object*,onPut_ptr);
void DM_Put(handle_t,json_object*);
void DM_onPut(handle_t,json_object* global_json);

void DM_observe(handle_t h, onobserve_ptr call_back);
void DM_onObserve(handle_t, json_object* global_json);




 #undef EXTERNC

#ifdef __cplusplus
}
#endif




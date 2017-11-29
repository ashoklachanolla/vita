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

#include<stdio.h>

#include<stdint.h>

#define MAX_NUM_OF_ATR 40





typedef struct
{
	char atr_name[30];
	char atr_value[30];
}Attribute;

typedef struct
{
	int NumOfElements;
	Attribute atr[MAX_NUM_OF_ATR];
}c_json;

c_json DM_json;


//typedef void (*onGet_ptr)(json_object*);
//typedef void (*onPut_ptr)(json_object*);
typedef void (*onGet_ptr1)(c_json);
typedef void (*onPut_ptr1)(c_json);
typedef void (*onobserve_ptr1)(c_json);
 

typedef void * handle_t;

//User level structure
typedef struct 
{
	char name[30];
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

EXTERNC int dm_init(int);

EXTERNC void DM_Discovery(disc_ptr);

c_json DM_Get(Resource_info*,onGet_ptr1);
void DM_onGet(c_json rep_json);

void DM_Put(Resource_info*,c_json,onPut_ptr1);
void DM_onPut(c_json);

void DM_observe(Resource_info*, onobserve_ptr1); 
void DM_onobserve(c_json);




 #undef EXTERNC

#ifdef __cplusplus
}
#endif




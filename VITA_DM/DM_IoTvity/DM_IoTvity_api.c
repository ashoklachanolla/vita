#include"IoTvity_Wrapper/DM_iotivity.h"
#include "../Headers/VITA_DM_INTERNAL.h"
#include "../DSM/Headers/VITA_DM_DSM.h"
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

sem_t disc_sem;
pthread_t discovery_thread;
void statemachine(void);
DM_IoTvity_discover_Info_t iotvity_disc_info[100];
int count = 0;

int DM_IoTVity_Init()
{	
    int res;
    res = sem_init(&disc_sem, 0, 0);
    if (res != 0) {
        perror("Semaphore initialization failed");
        return -1;
    }

    res = pthread_create(&discovery_thread, NULL, statemachine, NULL);
    if (res != 0) {
        perror("IOTVITY Task creation failed");
        return -1;
    }

    if(dm_init(2)!=0)
    {
        printf("failed to initialize\n");
        exit(1);
    }

    count = 0;
    return 0;
}
void statemachine()
{

    while(1)
    {
        sem_wait(&disc_sem);
        sleep(30);
        if(count != 0)
        {
            if(VITA_DM_DSM_IoTvity_Disc_Send(iotvity_disc_info ,count))
            {
                printf("Failed to store IoTvity resource details at SM\n");
            }
            else
            {

                printf("IoTvity Resource details Stored at SM\n");
            }
        }
        else
        {
            printf("No Resource found\n");
        }
        count = 0;
    }

}

int onFoundResource(Resource_info *node)
{
    /**********************modified S***************************/
    //printf("On found called");
    //printf("Name : %s  address %p\n\n",node->name,node->handle);

     
    if(strcmp(node->name,"/oic/d")==0||strcmp(node->name,"/oic/p")==0||strcmp(node->name,"/oic/sec/doxm")==0||strcmp(node->name,"/oic/sec/pstat")==0)
	{
	return;
	}
	printf("\n\n==========================================================================\n");
     printf("Resource name:\t%s\n",node->name);
     printf("Resource handle: %p\n",node->handle);
	printf("==========================================================================\n");
    /**********************modified E***************************/
    strcpy(iotvity_disc_info[count].name,node->name);
    strcpy(iotvity_disc_info[count].id,node->handle);
    iotvity_disc_info[count].signal = 1;
    //printf("**************** data stored ******************\n");
    count ++;
}

int DM_IoTVity_Discovery()
{

    DM_Discovery(onFoundResource);
    sem_post(&disc_sem);
}

void DM_onGet(handle_t handle,json_object* rep_json)
{
    printf("\t\t\t***resource attributes*** \n");
        printf("handle in app:%p\n",handle);
    VITA_DM_DSM_IotVity_Get_Data (handle, rep_json);

}

int DM_IoTVity_Get(const char *handle)
{
    unsigned long ulHandle = 0;


   sscanf(handle,"%lx",&ulHandle);
   printf("handle in string %s\n", handle);
   printf("handle in hex %lx\n", ulHandle);


    DM_Get((handle_t)ulHandle,DM_onGet);

}

int DM_IoTVity_Put(const char *handle,json_object *obj)
{
    unsigned long ulHandle = 0;

    sscanf(handle,"%lx",&ulHandle);
    printf("handle in string %s\n", handle);
    printf("handle in hex %lx\n", ulHandle);
    DM_Put((handle_t)ulHandle,obj);
}

int DM_IoTVity_Observe(const char *handle)
{
   // handle_t iotvity_handle;

    unsigned long ulHandle = 0;


   sscanf(handle,"%lx",&ulHandle);
   printf("handle in string %s\n", handle);
   printf("handle in hex %lx\n", ulHandle);

    DM_observe((handle_t)ulHandle,DM_onObserve);
    printf("handle in Observe: %s\n",handle);
}

void DM_onObserve(handle_t handle,json_object* rep_json)
{
    printf("\t\t\t***resource attributes*** \n");
        printf("handle in app:%p\n",handle);
   VITA_DM_DSM_IotVity_Observe_Data (handle, rep_json);

}







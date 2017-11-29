#include "DM_iotivity.h"
#include<mutex>
#include<condition_variable>
//#include<json/json.h>
#include "iotivity_config.h"
#include <string>
#include <map>
#include <cstdlib>
#include <mutex>
#include "OCPlatform.h"
#include "OCApi.h"
#include<iostream>
#include<OCRepresentation.h>
#include<memory>
#include<vector>

using namespace std;
using namespace OC;

typedef struct RESOURCE
{
    char uri[30];
    //onGet_ptr onget_instance;
    //onPut_ptr onput_instance;
    std::shared_ptr<OCResource> DM_Resource;
    struct RESOURCE *next;
}Resource_list;


string type_detection = "oic/res";	//fixed URI to found resources based on the resource type.

int dm_init(int mode_type)
{
    try
    {
        ModeType mode_new;
        if(mode_type ==0)
            mode_new = OC::ModeType::Server;
        else if(mode_type ==1)
            mode_new = OC::ModeType::Client;
        else if(mode_type ==2)
            mode_new = OC::ModeType::Both;

        PlatformConfig cfg {OC::ServiceType::InProc,mode_new,"0.0.0.0",0,OC::QualityOfService::HighQos};
        OCPlatform::Configure(cfg);
        std::cout<<"client is intialized with mode type as client"<<std::endl;
        return 0;
    }
    catch(OCException& e)
    {
        oclog() << "Exception in main: "<<e.what();
        return 1;
    }
}//init function


/******************************discover api**********************/
std::mutex dm_blocker;
Resource_list *resource_head = NULL;
disc_ptr global_call_back;

void found_resource(std::shared_ptr<OCResource> resource)
{
    std::string resourceURI;
    std::string resourceID;

    dm_blocker.lock();
    std::ostringstream resourceUnique;
    if(resource)
    {
        resourceURI = resource->uri();
        resourceID = string(resource->sid());
        Resource_list *temp = resource_head;
        Resource_list *local = (Resource_list *)malloc(sizeof(Resource_list));

        local->DM_Resource = resource;
        strcpy(local->uri,(resource->uri()).c_str());
        //local->onget_instance=NULL;
        //local->onput_instance=NULL;
        if (resource_head == NULL)
        {
            local->next = resource_head;
            resource_head = local;
            //void *back=reinterpret_cast<void>(resource_head);
            }
        else
        {
            local->next = NULL;
            while(temp->next != NULL)
            temp = temp->next;
            temp->next = local;
        }

        Resource_info *local_info=(Resource_info*)malloc(sizeof(Resource_info));
        void* conv=reinterpret_cast<void*>(local);
        local_info->handle=conv;
        strcpy(local_info->name,(resource->uri()).c_str());
        global_call_back(local_info);
    }//if loop
dm_blocker.unlock();
}//found_resource

std::mutex DM_LOCK_new;
std::condition_variable dm_cv;
std::unique_lock<std::mutex>dm_lock_new(DM_LOCK_new);

void DM_Discovery(disc_ptr call_back)
{
    global_call_back=call_back;
    std::ostringstream requestURI;
    requestURI << OC_RSRVD_WELL_KNOWN_URI;
    std::cout<<"Discovering Resources:"<<std::endl;
    OCPlatform::findResource("",requestURI.str(),CT_IP_USE_V4 ,&found_resource);
    std::cout <<"Waiting" << std::endl;
    dm_cv.wait_for(dm_lock_new,chrono::seconds(1));

    }///dm_discovery

void DM_onGet(c_json rep_json)
{

    try
    {
    //int i;
    cout<<"\t\t\tresource attributes "<<endl;
    }
    catch(std::exception& e)
    {
        cout << "Exception: " << e.what() <<"DM_onGet"<<endl;
    }
}

/////////////// conversion from OCRepresentation to Json format//////
std::mutex DM_LOCK_GET;
std::condition_variable DM_GET;
std::unique_lock<std::mutex>DM_WAIT(DM_LOCK_GET);

void on_Get(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{

        try
        {
        DM_json.NumOfElements=rep.numberOfAttributes();
        int j=0;
        map<std::string, AttributeValue> div;
        div = rep.getValues();
        std::string temp,temp1;
        for(map<std::string, AttributeValue>::iterator i = div.begin();i != div.end(); i++,j++)
             {
            temp=string(i->first);
            strcpy(DM_json.atr[j].atr_name,(temp).c_str());
            temp1=(rep.getValueToString(temp));
            strcpy(DM_json.atr[j].atr_value,temp1.c_str());
              }
        }
        catch(std::exception& e)
        {
        cout << "Exception: " << e.what() << " in on_Get failed"<<endl;
        }
        DM_GET.notify_all();
}
c_json DM_Get(Resource_info* resource1, onGet_ptr1 call_back)
{
    try
    {
    QueryParamsMap test;
    cout<<resource1->name<<endl;
    //handle = (Resource_list*);
    //Resource_list* back=reinterpret_cast<Resource_list*>(handle);
        reinterpret_cast<Resource_list*>(resource1->handle)->DM_Resource->get(test, &on_Get);
    DM_GET.wait(DM_WAIT);
    call_back(DM_json);
        }
    catch(std::exception& e)
    {
    cout << "Exception: " << e.what() << " in DM_get failed" <<endl;
    }
    return DM_json;
}

//////////////////////put/////////////////////////////////////////////

void DM_onPut(c_json dmjson)
{
        try
        {
                        int i;
            cout<<"updated information :"<<endl;
            for(i=0;i<dmjson.NumOfElements;i++)
                printf("%s : %s\n",dmjson.atr[i].atr_name,dmjson.atr[i].atr_value);
        }
        catch(std::exception& e)
        {
            cout << "Exception: " << e.what() << " in DM_onput failed" <<endl;
        }

}

void onPut(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
        DM_GET.notify_all();
return;

}

void DM_Put(Resource_info* resource1,c_json in_data, onGet_ptr1 call_back)
{
      try
       {

        QueryParamsMap test;
    OCRepresentation rep;
    int i;
            for(i=0;i<in_data.NumOfElements;i++)
            {
                string key(in_data.atr[i].atr_name);
                string value(in_data.atr[i].atr_value);
                rep.setValue(key,value);
            }

                reinterpret_cast<Resource_list*>(resource1->handle)->DM_Resource->put(rep, QueryParamsMap(), &onPut);
                DM_GET.wait(DM_WAIT);
                call_back(in_data);

       }
      catch(std::exception& e)
       {
    cout << "Exception: " << e.what() << " in Put failed" <<endl;
       }
}



static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe;
Resource_info* global_res=NULL;
onobserve_ptr1 global_on_call_back =NULL;

void DM_onobserve(c_json rep_json)
{
    try
    {
        int i;
        for(i=0;i<rep_json.NumOfElements;i++)
                printf("%s : %s\n",rep_json.atr[i].atr_name,rep_json.atr[i].atr_value);
    }
    catch(std::exception& e)
    {
            cout << "Exception: " << e.what() <<"DM_onobserve"<<endl;
    }

}

void onObserve(const HeaderOptions /*headerOptions*/, const OCRepresentation& rep,
                    const int& eCode, const int& sequenceNumber)
{
    try
    {
        c_json local_json;
        static int k=0;
        local_json.NumOfElements=rep.numberOfAttributes();
        int j=0;
        map<std::string, AttributeValue> div;
        div = rep.getValues();
        std::string temp,temp1;
        for(map<std::string, AttributeValue>::iterator i = div.begin();i != div.end(); i++,j++)
             {
                temp=string(i->first);
                strcpy(local_json.atr[j].atr_name,(temp).c_str());
                temp1=(rep.getValueToString(temp));
                strcpy(local_json.atr[j].atr_value,temp1.c_str());
              }
           global_on_call_back(local_json);
         if(k++ > 5) //Times to observe=5
            {
                std::cout<<"Cancelling Observe..."<<std::endl;
                ((reinterpret_cast<Resource_list*>(global_res->handle))->DM_Resource)->cancelObserve();
                sleep(3);
                std::cout << "DONE"<<std::endl;
               // k=0;
            }
        }
            catch(std::exception& e)
            {
                     cout << "Exception: " << e.what() << " in on_observe failed"<<endl;
            }

}

void DM_observe(Resource_info* resource1, onobserve_ptr1 call_back)
{
        global_on_call_back= call_back;
    global_res = resource1;
        try
        {
        cout<<resource1->name<<endl;
        (reinterpret_cast<Resource_list*>(resource1->handle))->DM_Resource->observe(OBSERVE_TYPE_TO_USE, QueryParamsMap(), &onObserve);
        }
        catch(std::exception& e)
        {
        cout << "Exception: " << e.what() << " in DM_get failed" <<endl;
        }
        return;
}

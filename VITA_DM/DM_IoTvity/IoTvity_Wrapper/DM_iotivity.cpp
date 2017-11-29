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
    onGet_ptr onget_instance;
    onPut_ptr onput_instance;
onobserve_ptr onobserve_instance;
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
}//dm_init function


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
        strcpy(local->uri,(resource->uri()).c_str());
        local->onget_instance=NULL;
        local->onput_instance=NULL;
        local->onobserve_instance=NULL;
        local->DM_Resource = resource;
        if (resource_head == NULL)
        {
            local->next = resource_head;
                        resource_head = local;
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

}///DM_Discovery

/////////////// conversion from OCRepresentation to Json format//////


void on_Get(const HeaderOptions& /*headerOptions*/,const OCRepresentation& rep, const int eCode)
{

    printf("On Get called at app level\n");


    try
    {
        int j=0;
        json_object *local_json= json_object_new_object();
        Resource_list duplicate_list;
        map<std::string, AttributeValue> div;
        div = rep.getValues();
        std::string temp1,temp2;
        for(map<std::string, AttributeValue>::iterator i = div.begin();i != div.end(); i++)
        {
            temp1=string(i->first);
            temp2=(rep.getValueToString(temp1));
                if((i->second).type()==typeid(int))
                {
                    json_object_object_add(local_json, temp1.c_str(), json_object_new_int(atoi(temp2.c_str())));
                }
                else if((i->second).type()==typeid(string))
                {
                    json_object_object_add(local_json, temp1.c_str(), json_object_new_string(temp2.c_str()));
                }
                else if((i->second).type()==typeid(bool))
                {
                    if(strcmp(temp2.c_str(),"true")==0)
                            j=1;
                    else if(strcmp(temp2.c_str(),"false")==0)
                        j=0;
                    json_object_object_add(local_json, temp1.c_str(), json_object_new_boolean(j));
                }
            else
                {
                    cout<<"no value"<<endl;
                }

                cout << "Key: " << temp1 << "Value :"<<temp2 <<endl;

    }//for

       /*  strcpy((duplicate_list).uri,(char*)rep.getUri().c_str());
         Resource_list *get_temp=resource_head;
         while(get_temp!=NULL)
         {
                if(strcmp(duplicate_list.uri,get_temp->uri)==0)
                {
                        get_temp->onget_instance((handle_t)get_temp,local_json);
                }
                get_temp=get_temp->next;
         }//while*/

    }//try
    catch(std::exception& e)
    {
        cout << "Exception: " << e.what() << " in on_Get failed"<<endl;
    }

}//on_Get


void DM_Get(handle_t h, onGet_ptr call_back)
{
    printf("in get at cpp 3== %p",h);

    try
    {
        printf("in get at cpp == %p",h);
            QueryParamsMap test;
            reinterpret_cast<Resource_list*>(h)->onget_instance=call_back;
            printf("in get at cpp 2 == %p",h);

            reinterpret_cast<Resource_list*>(h)->DM_Resource->get(test, &on_Get);
            printf("in get at cpp 3== %p",h);

    }
    catch(std::exception& e)
    {
        cout << "Exception: " << e.what() << " in DM_get failed" <<endl;
    }
        return;
}//DM_Get

//////////////////////put/////////////////////////////////////////////

void onPut(const HeaderOptions& /*headerOptions*/,const OCRepresentation& rep, const int eCode)
{

    return;

}//onPut

void DM_Put(handle_t h,json_object* in_data)
{
    try
        {
        QueryParamsMap test;
        OCRepresentation rep;
        string good;
        json_object_object_foreach(in_data, key, val)
        {
            switch(json_object_get_type(val))
            {
                case json_type_int: rep.setValue(key,json_object_get_int(val));
                break;
                case json_type_string: good = string(json_object_get_string(val));
                            rep.setValue(key,good);
                break;
                case json_type_boolean:  rep.setValue(key,json_object_get_boolean(val));
                break;
                case json_type_double: cout<<"double value:"<< endl;
                            break;
                case json_type_object: cout<<"object value"<<endl;
                            break;
                    case json_type_array: cout<<"array value"<<endl;
                            break;
                case json_type_null: cout<<"Null value"<<endl;
                            break;
            }
        }
        reinterpret_cast<Resource_list*>(h)->DM_Resource->put(rep, test, &onPut);
    }
        catch(std::exception& e)
        {
        cout << "Exception: " << e.what() << " in Put failed" <<endl;
        }

}//DM_Put

static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe;
onobserve_ptr global_on_call_back =NULL;
handle_t obs_h;
handle_t h;

void onObserve(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep,const int& eCode, const int& sequenceNumber)
{
    try
    {
    static int k=0;
        int j=0;
    json_object *local_json2= json_object_new_object();
    Resource_list duplicate_list2;
        map<std::string, AttributeValue> div;
        div = rep.getValues();
        std::string temp1,temp2;
        for(map<std::string, AttributeValue>::iterator i = div.begin();i != div.end(); i++,j++)
             {
                temp1=string(i->first);
                temp2=(rep.getValueToString(temp1));
            if((i->second).type()==typeid(bool))
            {
                if(strcmp(temp2.c_str(),"true")==0)
                j=1;
                else if(strcmp(temp2.c_str(),"false")==0)
                j=0;
                json_object_object_add(local_json2, temp1.c_str(), json_object_new_boolean(j));
            }
            else if((i->second).type()==typeid(int))
            {
                json_object_object_add(local_json2, temp1.c_str(), json_object_new_int(atoi(temp2.c_str())));
            }
            else if((i->second).type()==typeid(string))
            {
                json_object_object_add(local_json2, temp1.c_str(), json_object_new_string(temp2.c_str()));
            }
            else
            {
                cout<<"no value"<<endl;
            }
              }//for
        strcpy((duplicate_list2).uri,(char*)rep.getUri().c_str());
            Resource_list *temp=resource_head;
            while(temp!=NULL)
            {

                if(strcmp(duplicate_list2.uri,temp->uri)==0)
                {
                    temp->onobserve_instance((handle_t)temp,local_json2);
                }
                temp=temp->next;
            }//while
           if(k++ > 5) //Times to observe=5
            {
                std::cout<<"Cancelling Observe..."<<std::endl;
        ((reinterpret_cast<Resource_list*>(obs_h))->DM_Resource)->cancelObserve();
                sleep(1);
                std::cout << "DONE"<<std::endl;
            }//if
        }//try
            catch(std::exception& e)
            {

            }
}//onObserve

void DM_observe(handle_t h, onobserve_ptr call_back)
{
        global_on_call_back= call_back;
    obs_h=h;
        try
        {
        reinterpret_cast<Resource_list*>(h)->DM_Resource->observe(OBSERVE_TYPE_TO_USE, QueryParamsMap(), &onObserve);
        reinterpret_cast<Resource_list*>(h)->onobserve_instance=call_back;

        }
        catch(std::exception& e)
        {
            cout << "Exception: " << e.what() << " in DM_get failed" <<endl;
        }

        return;

}//DM_observe

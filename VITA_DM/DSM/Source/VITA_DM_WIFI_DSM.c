#include "VITA_DM_DSM.h"
#include "helper.h"

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   wifi_prepare_url(DM_WIFI_ScanResult_Info_t *WIFISearchData)
 *
 * DESCRIPTION           :   Prepaeres wifi json object for curl operation
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : WIFISearchData 	TYPE : DM_WIFI_ScanResult_Info_t
 *                                           DESC : It contains wifi search result data
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : json_object  	TYPE : json_object
 *                                           DESC : returns wifi json object
 *
**************************************************************************************************************************
**************************************************************************************************************************/

json_object *wifi_prepare_url(DM_WIFI_ScanResult_Info_t *WIFISearchData)
{
    int i;

    json_object *json;
    json_object *jarray;

    json_object *json_sub;
    json_object *json_sub_bt;

    json = json_object_new_object();
    
    char *deviceid;
    
    int result = DM_Get_Device_ID(&deviceid);
    if(result <= 0)
    {
        LOG_PRINT("<%s:%d>Empty device ID\n", __func__, __LINE__);
        deviceid = json_object_new_string(VITA_helper_macaddr(DEVICE_WIFI));
    }
    else
    {
        LOG_PRINT("<%s:%d>received device ID %s\n", __func__, __LINE__,deviceid);
       
    }

    json_object_object_add(json, "DeviceID", json_object_new_string(deviceid));
    json_object_object_add(json, "scan_timestamp", json_object_new_string(VITA_DM_getTimeStamp(DEVICE_WIFI)));
    json_object_object_add(json, "scan_type", json_object_new_string("WIFI"));


    //Creating a json array
    jarray = json_object_new_array();
    //!Preparing data to post from the search results

    DM_WIFI_DSM_Filter_t dm_wifi_filter_parameters = DM_get_wifi_filter();

    if(WIFISearchData[0].num_elements)
    {        
        for(i = 0; i < WIFISearchData[0].num_elements; i++) {
            //!Debug the search results
            /*LOG_PRINT("[%s:%d]Name: %s\n", __func__, __LINE__, WIFISearchData[i].wifi_device_names);
                 LOG_PRINT("[%s:%d]MACAddr: %s\n", __func__, __LINE__, WIFISearchData[i].macAddr);
                 LOG_PRINT("[%s:%d]Strength: %d\n", __func__, __LINE__, WIFISearchData[i].strength);
                 LOG_PRINT("[%s:%d]Frequency: %f\n", __func__, __LINE__, WIFISearchData[i].frequency);*/

            json_sub = json_object_new_object();

            if(dm_wifi_filter_parameters.bIncludeWifiName == 1)
            {
                json_object_object_add(json_sub, "Name", json_object_new_string(WIFISearchData[i].wifi_device_names));
            }
            if(dm_wifi_filter_parameters.bIncludeWifiDeviceID == 1)
            {
                json_object_object_add(json_sub, "pid", json_object_new_string(WIFISearchData[i].macAddr));

            }
            if(dm_wifi_filter_parameters.bIncludeWifiSignal == 1)
            {
                json_object_object_add(json_sub, "Signal", json_object_new_int(WIFISearchData[i].strength));

            }
            if(dm_wifi_filter_parameters.bIncludeWifiFrequency == 1)
            {
                json_object_object_add(json_sub, "Frequency", json_object_new_string(WIFISearchData[i].frequency));
            }
            json_object_object_add(json_sub, "type", json_object_new_string("WIFI"));
            json_object_array_add(jarray,json_sub);
        }
    }


    json_object_object_add(json,"userNetworkDetails", jarray);

    //!JSON final payload
    LOG_PRINT("The json object created: %s\n",json_object_to_json_string(json));
    return json;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_Das_WIFI_Send(DM_WIFI_ScanResult_Info_t *WIFISearchData)
 *
 * DESCRIPTION           :   Send wifi data to curl operation method triggers wifi_prepare_url
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : WIFISearchData 	TYPE : DM_WIFI_ScanResult_Info_t
 *                                           DESC : It contains wifi search result data
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : result  	TYPE : int
 *                                           DESC : returns result of send operaton
 *
**************************************************************************************************************************
**************************************************************************************************************************/
int VITA_DM_Das_WIFI_Send(DM_WIFI_ScanResult_Info_t *WIFISearchData)
{
    int result;
    json_object *jsonString;
    jsonString = wifi_prepare_url(WIFISearchData);
    char *url;

    printf("VITA_DM_DAS_WIFISEND -- 1");

    int url_result = DM_GetSM_URL(&url);
    if(url_result <= 0)
    {
        url = "http://192.168.4.19:8856/iotpoc/user/addUserDeviceDetails";
    }


    printf("VITA_DM_DAS_WIFISEND -- %s\n",url);
    result = VITA_DM_sendData(jsonString,url);

   // result = VITA_DM_sendData(jsonString);
    return result;
}

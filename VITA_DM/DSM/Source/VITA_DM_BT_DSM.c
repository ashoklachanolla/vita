#include "VITA_DM_DSM.h"
#include "helper.h"

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   bt_prepare_url(DM_BT_ScanResult_Info_t *BTSearchData)
 *
 * DESCRIPTION           :   Prepaeres bluetooth json object for curl operation
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : BTSearchData 	TYPE : DM_BT_ScanResult_Info_t
 *                                           DESC : It contains bluetooth search result data
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : json_object  	TYPE : json_object
 *                                           DESC : returns Bluetooth json object
 *
 **************************************************************************************************************************
 **************************************************************************************************************************/
json_object *bt_prepare_url(DM_BT_ScanResult_Info_t *BTSearchData)
{
    int i;

    json_object *json;
    json_object *jarray;

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
    json_object_object_add(json, "scan_timestamp", json_object_new_string(VITA_DM_getTimeStamp(DEVICE_BLUETOOTH)));
    json_object_object_add(json, "scan_type", json_object_new_string("BT"));

    
    //json_object_object_add(json, "DeviceID", json_object_new_string(VITA_helper_macaddr(DEVICE_WIFI)));

    //Creating a json array
    jarray = json_object_new_array();
    //!Preparing data to post from the search results

    DM_BT_DSM_Filter_t  dm_bt_filter_parameters = DM_get_bt_filter();


    if(BTSearchData[0].num_elements)
    {        
        for(i = 0; i < BTSearchData[0].num_elements; i++) {
            //!Debug the search results
            //LOG_PRINT("%s:%d %d. %s	%s \n", __func__, __LINE__, i, (BTSearchData[i]).bt_addr, (BTSearchData[i]).bt_name);
            json_sub_bt = json_object_new_object();
            if(dm_bt_filter_parameters.bIncludeBtName == 1)
            {
                json_object_object_add(json_sub_bt, "Name", json_object_new_string(BTSearchData[i].bt_name));
            }
            if(dm_bt_filter_parameters.bIncludeBtDeviceID == 1)
            {
                json_object_object_add(json_sub_bt, "pid", json_object_new_string(BTSearchData[i].bt_addr));
            }
            if(dm_bt_filter_parameters.bIncludeBtClass == 1)
            {
                json_object_object_add(json_sub_bt, "Class_Name", json_object_new_int(BTSearchData[i].bt_class));
            }
            json_object_object_add(json_sub_bt, "type", json_object_new_string("BLUETOOTH"));
            json_object_array_add(jarray,json_sub_bt);
        }
    }

    json_object_object_add(json,"userNetworkDetails", jarray);

    //!TODO: free the memory allocated for macaddr capture

    //!JSON final payload
    LOG_PRINT("The json object created: %s\n",json_object_to_json_string(json));
    return json;
}

/*************************************************************************************************************************
 **************************************************************************************************************************
 * NAME                  :   VITA_DM_Das_BT_Send(DM_BT_ScanResult_Info_t *BTSearchData)
 *
 * DESCRIPTION           :   Send BT data to curl operation method triggers wifi_prepare_url
 *
 * INPUTS                :
 *                           INPUT PARAMETERS:
 *
 *                                           NAME : BTSearchData 	TYPE : DM_BT_ScanResult_Info_t
 *                                           DESC : It contains bluetooth search result data
 *
 *                           OUTPUT PARAMETERS:
 *
 *                                           NAME : result  	TYPE : int
 *                                           DESC : returns result of send operaton
 *
 **************************************************************************************************************************
 **************************************************************************************************************************/

int VITA_DM_Das_BT_Send(DM_BT_ScanResult_Info_t *BTSearchData)
{
    int result;
    json_object *jsonString;
    jsonString = bt_prepare_url(BTSearchData);

    char *url;
    int url_result = DM_GetSM_URL(&url);
    if(url_result <= 0)
    {
        url = "http://192.168.4.19:8856/iotpoc/user/addUserDeviceDetails";
    }

    result = VITA_DM_sendData(jsonString,url);
    return result;
}

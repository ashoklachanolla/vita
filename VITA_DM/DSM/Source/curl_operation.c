#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "VITA_DM_DSM.h"
#include "helper.h"

//size_t curl_callback(void *contents, size_t size, size_t nmemb, void *userp) {
//    size_t realsize = size * nmemb;
//    struct curl_fetch_st *p = (struct curl_fetch_st *) userp;
//    p->payload = (char *) realloc(p->payload, p->size + realsize + 1);
//    if (p->payload == NULL) {
//        fprintf(stderr, "ERROR: Failed to expand buffer in curl_callback\n");
//        free(p->payload);
//        return -1;
//    }

//    memcpy(&(p->payload[p->size]), contents, realsize);
//    p->size += realsize;
//    p->payload[p->size] = 0;
//    return realsize;
//}

//CURLcode curl_fetch_url(CURL *ch, const char *url, struct curl_fetch_st *fetch) {
//    CURLcode rcode;
//    fetch->payload = (char *) calloc(1, sizeof(fetch->payload));
//    if (fetch->payload == NULL) {
//        fprintf(stderr, "ERROR: Failed to allocate payload in curl_fetch_url\n");
//        return CURLE_FAILED_INIT;
//    }
//    fetch->size = 0;
//    curl_easy_setopt(ch, CURLOPT_URL, url);
//    curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, curl_callback);
//    curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *) fetch);
//    curl_easy_setopt(ch, CURLOPT_USERAGENT, "libcurl-agent/1.0");
//    curl_easy_setopt(ch, CURLOPT_TIMEOUT, 5);
//    curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1);
//    curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 1);
//    rcode = curl_easy_perform(ch);
//    return rcode;
//}

///*************************************************************************************************************************
// **************************************************************************************************************************
// * NAME                  :   VITA_DM_sendData(json_object *jsonStr)
// *
// * DESCRIPTION           :   send wifi/bluetooth scan results json obj to SM
// *
// * INPUTS                :
// *                           INPUT PARAMETERS:
// *
// *                                           NAME : jsonStr 	TYPE : json_object
// *                                           DESC : contains wifi/bluetooth scan results of json object
// *
// *                           OUTPUT PARAMETERS:
// *
// *                                           NAME : result  	TYPE : int
// *                                           DESC : returns send operation results
// *
// **************************************************************************************************************************
// **************************************************************************************************************************/

//int VITA_DM_sendData(json_object *jsonStr)
//{

//    CURL *ch;
//    CURLcode rcode;
//    struct curl_fetch_st curl_fetch;
//    struct curl_fetch_st *cf = &curl_fetch;
//    struct curl_slist *headers = NULL;
//    json_object *json;
//    enum json_tokener_error jerr = json_tokener_success;

//    /*url to add details Database server*/
//    char *url;

//    int result = DM_GetSM_URL(&url);
//    if(result <= 0)
//    {
//        url = "http://192.168.4.19:8856/iotpoc/user/addUserDeviceDetails";
//    }

//    if ((ch = curl_easy_init()) == NULL) {
//        fprintf(stderr, "ERROR: Failed to create curl handle in fetch_session\n");
//        return 1;
//    }

//    headers = curl_slist_append(headers, "Accept: application/json");
//    headers = curl_slist_append(headers, "Content-Type: application/json");

//    curl_easy_setopt(ch, CURLOPT_CUSTOMREQUEST, "POST");
//    curl_easy_setopt(ch, CURLOPT_HTTPHEADER, headers);
//    curl_easy_setopt(ch, CURLOPT_POSTFIELDS, json_object_to_json_string(jsonStr));

//    /* fetch page and capture return code */
//    rcode = curl_fetch_url(ch, url, cf);
//    curl_easy_cleanup(ch);
//    curl_slist_free_all(headers);
//    json_object_put(jsonStr);

//    if (rcode != CURLE_OK || cf->size < 1) {
//        fprintf(stderr, "ERROR: Failed to fetch url (%s) - curl said: %s\n",
//                url, curl_easy_strerror(rcode));
//        return 2;
//    }

//    /* check payload */
//    if (cf->payload != NULL) {
//        LOG_PRINT("CURL Returned: \n%s\n", cf->payload);
//        jsonStr = json_tokener_parse_verbose(cf->payload, &jerr);
//        free(cf->payload);
//    } else {
//        fprintf(stderr, "ERROR: Failed to populate payload\n");
//        free(cf->payload);
//        return 3;
//    }

//    /* check error */
//    if (jerr != json_tokener_success) {
//        fprintf(stderr, "ERROR: Failed to parse json string\n");
//        json_object_put(jsonStr);
//        return 4;
//    }
//    //!data post operation
//    LOG_PRINT("Parsed JSON: %s\n", json_object_to_json_string(jsonStr));
//    /* exit */
//    return 0;
//}

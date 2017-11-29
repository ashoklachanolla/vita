/*
 * Generic helper for DM Module
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <string.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>

#include "helper.h"

char *macaddr;
FILE *fp ;
static int SESSION_TRACKER; //Keeps track of session
char *buf;

char *VITA_helper_macaddr(VITA_DM_DEVICETYPE_e deviceType)
{
    printf("<%s:%d>START\n", __func__, __LINE__);
    macaddr = malloc(30);
    unsigned char *mac;
    int fd, ctl;
    struct ifreq ifr;
    char * wifi_iface = "wlan0";

    if(deviceType == DEVICE_BLUETOOTH)
    {
        struct hci_dev_info di;
        char addr[18];
        if((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0)
        {
            perror("Can't open HCI socket. \n");
            return NULL;
        }

        if(ioctl(ctl, HCIGETDEVINFO, (void*) &di))
        {
            printf("<%s:%d>\n", __func__, __LINE__);
            return NULL;
        }
        ba2str(&di.bdaddr, addr);
        printf("BT ADDRESS:\n\t%s\t%s\n", di.name, addr);
        memcpy(macaddr, addr, strlen(addr));
    }
    else if(deviceType == DEVICE_WIFI)
    {
        fd = socket(AF_INET, SOCK_DGRAM, 0);

        ifr.ifr_addr.sa_family = AF_INET;
        strncpy(ifr.ifr_name , wifi_iface , IFNAMSIZ-1);

        ioctl(fd, SIOCGIFHWADDR, &ifr);

        close(fd);
        mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;

        snprintf(macaddr, 20, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        printf("WIFI MAC ADDRESS: %s\n", macaddr);
    }
    printf("<%s:%d>END\n", __func__, __LINE__);
    return macaddr;
}

const char *parseResponseObj(json_object *json)
{
    json_object *returnObj;

    if (json_object_object_get_ex(json, "StatusCode", &returnObj))
    {
        if(returnObj != NULL)
        {
            return json_object_to_json_string(returnObj);
        }        
    }
    return NULL;
}

void checkInternetConnection()
{
    char *hostname;
    struct hostent *hostinfo;

    hostname = "google.com";
    hostinfo = gethostbyname(hostname);

    if (hostinfo == NULL)
        printf("-> no connection!\n");
    else
        printf("-> connection established!\n");
}
/*
 * - Adding support for time logging in Utilities
 * - Test call
 * - Time stamp
 * - Logging
 */
//! Test Call
void test_log()
{
    int x = 199, i = 0;
    char *s = "Bitter Truth";

    while(i < 5)
    {
        LOG_PRINT("Hello World ");
        LOG_PRINT("Zing is back !!! %s %d", s, x++);
        i++;
    }
}

//! Time Stamp
char* print_time()
{
    int size = 0;
    time_t t;

    t = time(NULL); /* get current calendar time */

    char *timestr = asctime(localtime(&t));
    timestr[strlen(timestr) - 1] = 0;  //Getting rid of \n

    size = strlen(timestr)+ 1 + 2; //Additional +2 for square braces
    buf = (char*)malloc(size);

    memset(buf, 0x0, size);
    snprintf(buf,size,"[%s]", timestr);

    return buf;
}

//! Logging
void log_print(char* filename, int line, char *fmt,...)
{
    va_list         list;
    char            *p, *r;
    int             e;

    /*if(SESSION_TRACKER > 0)
      fp = fopen("log.txt","a+");
    else
      fp = fopen("log.txt","w");*/

    fprintf(stdout,"%s ",print_time());
    fprintf(stdout,"<%s:%d> ",filename, line);
    va_start(list, fmt);

    for (p = fmt; *p; ++p)
    {
        if ( *p != '%' )//If simple string
        {
            fputc( *p,stdout );
        }
        else
        {
            switch (*++p) {
                /* string */
            case 's':
            {
                r = va_arg(list, char *);

                fprintf(stdout,"%s", r);
                continue;
            }

            /* integer */
            case 'd':
            {
                e = va_arg(list, int);

                fprintf(stdout, "%d", e);
                continue;
            }

            default:
                fputc(*p, stdout);
            }
        }
    }
    va_end( list );
    fputc( '\n', stdout );
    SESSION_TRACKER++;
    //fclose(fp);
}

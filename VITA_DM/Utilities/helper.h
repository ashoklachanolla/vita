#ifndef HELPER_H
#define HELPER_H

#include "VITA_DM.h"
#include "../Source/json/json_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

char *VITA_helper_macaddr(VITA_DM_DEVICETYPE_e deviceType);
void checkInternetConnection();
const char *parseResponseObj(json_object *json);

void test_log();
void log_print(char* filename, int line, char *fmt,...);
char* print_time();

#define LOG_PRINT(...) log_print(__FILE__, __LINE__, __VA_ARGS__ )

#ifdef __cplusplus
}
#endif

#endif //!HELPER_H

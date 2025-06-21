
#include <stdio.h>

#define WIFI_STA_TEST_MODE
#undef  ENABLE_WIFI_STATIC_IP
#define  TCP_TEST_MODE

#undef  CONSTANT_TCP_RECEIVE_LEN 
#define TCP_RECEIVE_DATA_LENGTH 8192 // 3  //
#define TCP_RECEIVE_DATA_SUFFIX "\n"
#define TCP_RECEIVE_DATA_SUFFIX_LENGTH 1


#define TAG "Cypress"

#undef AP_MODE     
#define STA_MODE    

#define ON_VALUE "on"
#define OFF_VALUE "off"

#define ON_RESPONSE "on"
#define OFF_RESPONSE "off"

#define SEND_TO_ALL_SOCKET  -2

#define ESP_AP_WIFI_PASS        "123456789"  
#define DEVICE_MAC_ADDRESS_STR  "C0428A15753C"
#define ESP_AP_WIFI_SSID        "CYPRESS_ADC_15753C"

#define ROUTER_SSID "GalaxyA35"
#define ROUTER_PASS "Majid4321"
#define TCP_PORT    1234

#define WIFI_STA_MODE_TASK_STACK_DEPTH       20480
#define TCP_SERVER_TASK_STACK_DEPTH          20480
#define CHECK_TCP_TIMEOUT_TASK_STACK_DEPTH   4096

 

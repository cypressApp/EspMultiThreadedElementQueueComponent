#include <iostream>
#include <stdint.h>
#include <string>

extern "C"{

#include "esp_netif.h"

}

using namespace std;

class WifiInfoClass{

    static const string ESP_AP_WIFI_PASS; 
    static const string DEVICE_MAC_ADDRESS_STR;
    static const string ESP_AP_WIFI_SSID;

    uint8_t device_mac_address[10] = {0};

public:

    static const string ROUTER_SSID;
    static const string ROUTER_PASS;

    esp_netif_t *p_netif;

    uint32_t get_device_ip_info_int();
    uint32_t get_gateway_ip_info_int();
    void get_gateway_ip_info_array(int *array , uint32_t gateway_ip);
    void update_wifi_mode_ip(int tempIpSlice);
    void init_mac_address();
    int  get_ip4(char *addr_str , int len);

};
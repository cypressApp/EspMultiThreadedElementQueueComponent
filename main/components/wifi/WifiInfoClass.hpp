#include <iostream>
#include <stdint.h>
#include <string>

extern "C"{

#include "esp_netif.h"

}

using namespace std;

class WifiInfoClass{

    static const string DEVICE_MAC_ADDRESS_STR;
    uint8_t device_mac_address[10] = {0};

public:

    static const string ROUTER_SSID;
    static const string ROUTER_PASS;

    esp_netif_t *p_netif;

    /**
     * @brief set mac address of devide
     * 
     * This function get the mac address in configuration (DEVICE_MAC_ADDRESS_STR) 
     * and the relevant register of the device 
     * 
     */
    void init_mac_address();

};
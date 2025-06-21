
#include "context.h"

bool update_firmware_mode = false;
int  update_firmware_buffer_size = 0;

uint8_t device_mac_address[10] = {0};
// char    esp_wifi_ap_ssid[32];

char      remote_server_address[30]  = "34.17.39.69";
long int  remote_server_port         = 3001;
char      remote_server_url[50]      = "";

uint8_t wifi_mode_str   [256] = {0};
char    eeprom_ssid     [256] = {0};
char    eeprom_password [256] = {0};

int  wifi_if_mode                  = 1;
int  ip1                           = 0; 
int  ip2                           = 0; 
int  ip3                           = 0; 
int  ip4                           = 0; 
int  ip4_len                       = 0;
int  wire_con_alive_slave_size     = 0;
int  wire_con_alive_slave_size_len = 0;

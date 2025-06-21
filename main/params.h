
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "GPIO.h"

extern bool update_firmware_mode;
extern int  update_firmware_buffer_size;

extern uint8_t device_mac_address[10];
// extern char    esp_wifi_ap_ssid[32];

extern char touch_sensitivity_percent[5] ;

extern char      remote_server_address[30];
extern long int  remote_server_port       ;
extern char      remote_server_url[50]    ;

extern uint8_t wifi_mode_str[256];
extern char eeprom_ssid     [256];
extern char eeprom_password [256];

extern int  wifi_if_mode                 ; 
extern int  ip1                          ; 
extern int  ip2                          ; 
extern int  ip3                          ; 
extern int  ip4                          ; 
extern int  ip4_len                      ;
extern int  wire_con_alive_slave_size    ;
extern int  wire_con_alive_slave_size_len;

extern int  touch_pad_indices         [4]    ;
extern int  touch_pad_size                   ;
extern int  touch_pad_relay_pin       [5]    ; 
extern int  touch_pad_pressed_led_pin [5]    ; 
extern int  touch_pad_learning_led_pin[5]    ; 
extern int  touch_rgb_pin             [5][3] ; 

extern int  touch_rgb_off_color       [5][3] ; 

extern int  rgb_pins_array            [3][3] ;

extern int  rgb_off_color_array       [3][3] ; 
extern int  relay_pins_array          [4]    ; 

extern int  relay_level_array_tsc     [5]    ;
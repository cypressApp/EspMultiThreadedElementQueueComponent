#include <iostream>

using namespace std;

#include "components/wifi/WifiStationMode.hpp"
#include "components/tcp/TcpServerClass.hpp"

extern "C" {

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "spi_flash_mmap.h"

esp_err_t init_nvs_flash(){
  esp_err_t ret = nvs_flash_init();
  ESP_ERROR_CHECK(ret);
  return ret;

}

void app_main(){

//     init_tcp_server();    

//     init_mac_address();

//     init_gpio_pins();

	if(init_nvs_flash() != ESP_OK){
		printf("#\r\nERROR NVS FLASH\r\n#\r\n");
	}

// #ifdef STA_MODE
//     sprintf(ssid_arg , ROUTER_SSID);
//     sprintf(pass_arg , ROUTER_PASS);
//     xTaskCreate(wifi_init_sta, "wifi_init_sta_task", WIFI_STA_MODE_TASK_STACK_DEPTH, NULL, 5, NULL);
// #elif AP_MODE
//     wifi_init_accesspoint_mode();
//     xTaskCreate(tcp_server_task, "tcp_server", TCP_SERVER_TASK_STACK_DEPTH, (void*)AF_INET, 5, NULL);
// #endif

    WifiStationMode *wifiStationMode = new WifiStationMode();
    wifiStationMode->start();

    TcpServerClass *tcpServerClass = new TcpServerClass();
    tcpServerClass->start();

    cout << "Main cpp" << endl;
    
    while(1){
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

}

}
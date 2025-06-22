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

  if(init_nvs_flash() != ESP_OK){
    printf("#\r\nERROR NVS FLASH\r\n#\r\n");
  }

  WifiStationMode *wifiStationMode = new WifiStationMode();
  wifiStationMode->start();

  TcpServerClass *tcpServerClass = new TcpServerClass();
  tcpServerClass->start();

  while(1){
      vTaskDelay(10 / portTICK_PERIOD_MS);
  }

}

}
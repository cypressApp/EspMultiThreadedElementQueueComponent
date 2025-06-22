#include "WifiInfoClass.hpp"

extern "C"{

#include "esp_netif.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_mac.h"

}

const string WifiInfoClass::DEVICE_MAC_ADDRESS_STR = "C0428A15753C";
const string WifiInfoClass::ROUTER_SSID = "GalaxyA35";
const string WifiInfoClass::ROUTER_PASS = "Majid4321";

esp_netif_t *p_netif;

void WifiInfoClass::init_mac_address(){

	uint8_t device_mac_address[6];

	auto hexCharToInt = [](char c) -> uint8_t {
		if (std::isdigit(c)) return c - '0';
		return std::toupper(c) - 'A' + 10;
	};

	for (int i = 0; i < 6; ++i) {
		int index = i * 2;
		uint8_t high = hexCharToInt(WifiInfoClass::DEVICE_MAC_ADDRESS_STR[index]);
		uint8_t low  = hexCharToInt(WifiInfoClass::DEVICE_MAC_ADDRESS_STR[index + 1]);
		device_mac_address[i] = (high << 4) | low;
	}

	esp_base_mac_addr_set(device_mac_address);	

}

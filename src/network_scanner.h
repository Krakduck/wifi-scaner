#ifndef NETWORK_SCANNER_H
#define NETWORK_SCANNER_H

#include <Arduino.h>
#include <lwip/etharp.h>
#include <ESP8266WiFi.h>

// 1. Объявления всех структур
struct WifiNetwork {
  String ssid;
  int32_t rssi;
  String bssid;
  int32_t chanel;
  String encryption;
};

struct NetworkDevice {
  String ip;
  String bssid;
  const char* manufacturer;
};

struct OpenService {
  String ip;
  String description;
};

struct MacVendor {
  const char* prefix;
  const char* vendor;
};

struct KnownPort {
  uint16_t port;
  const char* serviceName;
};

// 2. Внешние переменные и массивы (память выделена в main.cpp)
extern const uint8_t max_amount_net;
extern uint8_t total_found_nets;
extern uint8_t current_net_index;
extern uint8_t curent_index;
extern uint8_t total_device;
extern bool update_static_elements;
extern const uint8_t totalItems;

extern WifiNetwork data_net[];
extern NetworkDevice data_device[];
extern OpenService service_device[];

// 3. Константная база вендоров
extern const MacVendor vendors[];

// 4. Список портов
extern const KnownPort common_ports[];
extern const uint8_t total_known_ports;

// 5. Прототипы функций (с совпадением имён и регистров)
void AddNet(String ssid, int32_t rssi, String bssid, int32_t chanel, String encryption);
void ScanNet();
void ScanTargetPorts();
void Manufacturer(String bssid, int index);
void ScanDevice();
void Connect();

#endif // NETWORK_SCANNER_H
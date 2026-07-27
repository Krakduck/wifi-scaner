#include "network_scanner.h"
#include "display_ui.h" // Нужен для доступа к объекту display

void AddNet(String ssid, int32_t rssi, String bssid, int32_t chanel, String encryption) {
  bool saved = false;
  bool space_found = false;
  for (int i = 0; i < max_amount_net; i++) {
    if (data_net[i].bssid == bssid) {
      data_net[i].ssid = ssid;
      data_net[i].rssi = rssi;
      data_net[i].chanel = chanel;
      data_net[i].encryption = encryption;

      saved = true;
      space_found = true;
      break;
    }
  }
  if (!saved) {
    for (int i = 0; i < max_amount_net; i++) {
      if (data_net[i].bssid == "") {
        data_net[i] = {ssid, rssi, bssid, chanel, encryption};
        space_found = true;
        break;
      }
    }
  }
  if (!space_found) {
    Serial.println("Переполнение списка сетей");
  }
}

void ScanNet() {
  Serial.println("сканер вызван");
  int count_net = WiFi.scanNetworks();
  if (count_net < 0) {
    Serial.println("Ошибка сканирования");
  }
  else if (count_net == 0) {
    Serial.println("Сетей не найдено");
  }
  else {
    total_found_nets = (count_net > max_amount_net) ? max_amount_net : count_net;
    for (int i = 0; i < count_net; i++) {
      String ssid = WiFi.SSID(i);
      int32_t rssi = WiFi.RSSI(i);
      String bssid = WiFi.BSSIDstr(i);
      int32_t chanel = WiFi.channel(i);
      uint8_t number_encryption = WiFi.encryptionType(i);
      String encryption;

      switch (number_encryption) {
        case 2: encryption = "WPA / PSK"; break;
        case 4: encryption = "WPA2 / PSK"; break;
        case 5: encryption = "Старый, взломанный WEP"; break;
        case 7: encryption = "Открытая сеть"; break;
        case 8: encryption = "WPA / WPA2 / PSK"; break;
        default: encryption = "Неизвестно";
      }
      AddNet(ssid, rssi, bssid, chanel, encryption);
    }
  }
  Serial.println("сканер завершил работу");
}

void ScanTargetPorts() {
  WiFiClient client;
  client.setTimeout(200);

  Serial.print("Сканирование портов для IP: ");
  uint16_t open_service_count = 0;
  for (int i = 0; i < total_device; i++) {
    IPAddress target_ip;
    target_ip.fromString(data_device[i].ip);
    for (int j = 0; j < total_known_ports; j++) {
      if (open_service_count == max_amount_net) {
        return;
      }
      uint16_t port = common_ports[j].port;
      String service = common_ports[j].serviceName;

      if (client.connect(target_ip, port)) {
        Serial.print("  [+] Порт ");
        Serial.print(port);
        Serial.print(" ОТКРЫТ — Служба: ");
        Serial.println(service);
        service_device[open_service_count].ip = target_ip.toString();
        service_device[open_service_count].description = String(port) + " - " + String(service);
        open_service_count++;

        client.stop();
      } else {
        Serial.print("  [-] Порт ");
        Serial.print(port);
        Serial.println(" закрыт.");
      }
      delay(10);
    }
  }
  DrawPorts(0);
}

void Manufacturer(String bssid, int index) {
  Serial.println("вызвана фукнция Manufacturer");
  bssid.toUpperCase();

  String prefix = bssid.substring(0, 8);
  Serial.print("Ищем префикс: ");
  Serial.println(prefix);
  Serial.print("MAC-адрес устройства: ");
  Serial.println(bssid);

  char secondChar = prefix.charAt(1);
  if (secondChar == '2' || secondChar == '6' || secondChar == 'A' || secondChar == 'E') {
    data_device[index].manufacturer = "Private / Random";
    Serial.println("Результат: Private / Random\n");
    return;
  }

  data_device[index].manufacturer = "Unknown";

  int totalItems = sizeof(vendors) / sizeof(vendors[0]);
  for (int i = 0; i < totalItems; i++) {
    if (prefix == vendors[i].prefix) {
      Serial.println(vendors[i].vendor);
      data_device[index].manufacturer = vendors[i].vendor;
      Serial.println("");
      return;
    }
  }
}

void ScanDevice() {
  struct eth_addr* ret_ethaddr;
  const ip4_addr_t* ret_ipaddr;
  Serial.println("функция scan_device вызвана");

  while (WiFi.status() != WL_CONNECTED) {
    display.fillScreen(ILI9341_WHITE);
    display.setCursor(80, 120);
    display.setTextSize(5);
    display.setTextColor(ILI9341_BLACK);
    display.print(".");
    delay(30);
    display.print(".");
    delay(30);
    display.print(".");
    delay(30);
  }

  display.fillScreen(ILI9341_WHITE);
  IPAddress local_ip = WiFi.localIP();
  IPAddress subnet_mask = WiFi.subnetMask();
  IPAddress gateway_ip = WiFi.gatewayIP();
  total_device = 0;

  DrawInfoNet(&local_ip, &subnet_mask, &gateway_ip);
  display.setCursor(200, 200);
  display.setTextColor(ILI9341_RED);
  display.print("!!!");

  for (int i = 0; i < 254; i++) {
    if (i == 1) { // Сам роутер
      continue;
    }
    if (total_device == max_amount_net) {
      break;
    }

    IPAddress target_ip(local_ip[0], local_ip[1], local_ip[2], i);
    if (target_ip == local_ip) {
      continue;
    }

    ip4_addr_t target_ip_lwip;
    target_ip_lwip.addr = (uint32_t)target_ip;
    etharp_request(netif_default, &target_ip_lwip);
    delay(100);

    s8_t result = etharp_find_addr(netif_default, &target_ip_lwip, &ret_ethaddr, &ret_ipaddr);
    String bssid = "";
    if (result >= 0) {
      for (int j = 0; j < 6; j++) {
        uint8_t current_byte = ret_ethaddr->addr[j];
        if (j > 0) {
          bssid += ":";
        }
        if (current_byte < 16) {
          bssid += "0";
        }
        bssid += String(current_byte, HEX);
      }
      data_device[total_device].ip = target_ip.toString();
      data_device[total_device].bssid = bssid;
      Manufacturer(bssid, total_device);
      total_device++;
    }
  }

  display.fillRect(200, 200, 240, 320, ILI9341_WHITE);
  Serial.println(total_device);
  update_static_elements = true;
}

void Connect() {
  WiFi.disconnect();
  Serial.println("функция Connect вызвана");
  String ssid = data_net[current_net_index].ssid;
  String encryption = data_net[current_net_index].encryption;

  if (encryption == "Открытая сеть") {
    WiFi.begin(ssid);
  } else {
    if (ssid == "dlink3") {
      WiFi.begin(ssid, "nSHDsozm3105");
    } else if (ssid == "dlink2") {
      WiFi.begin(ssid, "147852369");
    }
  }
  ScanDevice();
}


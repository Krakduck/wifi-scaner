#include "network_scanner.h"
#include "display_ui.h" // Нужен для доступа к объекту display
#include "web_server.h"

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
      if (open_service_count == max_amount_port) {
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
  WebDrawPorts();
}

void Vendor(String bssid, int index) {
  Serial.println("вызвана фукнция Manufacturer");
  bssid.toUpperCase();

  String prefix = bssid.substring(0, 8);
  Serial.print("Ищем префикс: ");
  Serial.println(prefix);
  Serial.print("MAC-адрес устройства: ");
  Serial.println(bssid);

  char secondChar = prefix.charAt(1);
  if (secondChar == '2' || secondChar == '6' || secondChar == 'A' || secondChar == 'E') {
    data_device[index].vendor = "Private / Random";
    Serial.println("Результат: Private / Random\n");
    return;
  }

  data_device[index].vendor = "Unknown";

  for (int i = 0; i < totalItems; i++) {
    if (prefix == vendors[i].prefix) {
      Serial.println(vendors[i].vendor);
      data_device[index].vendor = vendors[i].vendor;
      Serial.println("");
      return;
    }
  }
}

void ScanDevice() {
  struct eth_addr* ret_ethaddr;
  const ip4_addr_t* ret_ipaddr;
  Serial.println("функция scan_device вызвана");

  //uint16_t COLOR_BG  = ILI9341_BLACK;
  //uint16_t COLOR_TXT = ILI9341_GREEN;
  //uint16_t COLOR_ACC = ILI9341_DARKGREEN;

  // Экран ожидания подключения
  //display.fillScreen(COLOR_BG);
  //display.drawRect(2, 2, 236, 316, COLOR_TXT);

  // Хакерская заставка во время ожидания
  //display.setCursor(20, 100);
  //display.setTextSize(2);
  //display.setTextColor(COLOR_TXT);
  //display.print("CONNECTING...");

  //int connect_dots = 0;
  while (WiFi.status() != WL_CONNECTED) {
    //display.fillRect(20, 130, 200, 30, COLOR_BG);
    //display.setCursor(20, 130);
   // for (int d = 0; d <= connect_dots; d++) {
     // display.print(" >");
    //}
    //connect_dots = (connect_dots + 1) % 5;
    //delay(100);
  }

  IPAddress local_ip = WiFi.localIP(); //ip платы в этой сети
  IPAddress subnet_mask = WiFi.subnetMask();
  IPAddress gateway_ip = WiFi.gatewayIP();
  total_device = 0;

  // Отрисовываем заголовок интерфейса
  WebDrawInfoNet(&local_ip, &subnet_mask, &gateway_ip);

  // Оформляем блок сканирования сети
  //display.setCursor(10, 175);
  //display.setTextSize(1);
  //display.setTextColor(COLOR_TXT);
  //display.print("[>] ARP SCANNING NETWORK...");

  // Отрисовка контейнера прогресс-бара
  //display.drawRect(10, 195, 220, 20, COLOR_TXT);

  for (int i = 0; i < 254; i++) {
    // 1. Динамический анимированный прогресс-бар в стиле зебры/сегментов
    //int progress_width = map(i, 0, 253, 0, 216);
    //display.fillRect(12, 197, progress_width, 16, COLOR_TXT);

    // 2. Отображение текущего сканируемого IP в реальном времени
    //display.fillRect(10, 222, 220, 16, COLOR_BG);
    //display.setCursor(10, 222);
    //display.setTextSize(1);
    //display.setTextColor(COLOR_ACC);
    //display.print("PING -> ");
    //display.print(local_ip[0]); display.print(".");
    //display.print(local_ip[1]); display.print(".");
    //display.print(local_ip[2]); display.print(".");
    //display.print(i);

    if (i == 1) { // Сам роутер
      continue;
    }
    if (total_device == max_amount_device) {
      break;
    }

    IPAddress target_ip(local_ip[0], local_ip[1], local_ip[2], i);
    if (target_ip == local_ip) {
      continue;
    }

    ip4_addr_t target_ip_lwip;
    target_ip_lwip.addr = (uint32_t)target_ip;
    etharp_request(netif_default, &target_ip_lwip);
    delay(15);
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
      Vendor(bssid, total_device);
      total_device++;
    }
  }

  // Подведение итогов сканирования в инверсной плашке внизу
 // display.fillRect(8, 250, 224, 50, COLOR_TXT);
  //display.setCursor(16, 260);
  //display.setTextSize(2);
  //display.setTextColor(COLOR_BG);
  //display.print("FOUND: ");
  //display.print(total_device);
  //display.print(" DEV");

  //display.setCursor(16, 282);
  //display.setTextSize(1);
  //display.print("ARP SCAN COMPLETE");

  Serial.println(total_device);
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


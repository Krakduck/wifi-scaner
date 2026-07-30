#include "display_ui.h"

// Вспомогательная функция для отрисовки углов-фиксаторов рамки в киберпанк/Flipper стиле
static void DrawCornerBrackets(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t len, uint16_t color) {
  // Верхний левый
  display.drawFastHLine(x, y, len, color);
  display.drawFastVLine(x, y, len, color);
  // Верхний правый
  display.drawFastHLine(x + w - len, y, len, color);
  display.drawFastVLine(x + w - 1, y, len, color);
  // Нижний левый
  display.drawFastHLine(x, y + h - 1, len, color);
  display.drawFastVLine(x, y + h - len, len, color);
  // Нижний правый
  display.drawFastHLine(x + w - len, y + h - 1, len, color);
  display.drawFastVLine(x + w - 1, y + h - len, len, color);
}

void DrawMainScreen(uint8_t index) {
  Serial.println("начинается отрисовка");
  String ssid = data_net[index].ssid;
  int32_t rssi = data_net[index].rssi;
  String bssid = data_net[index].bssid;
  int32_t chanel = data_net[index].chanel;
  String encryption = data_net[index].encryption;
  
  // Хакерская палитра: глубокий черный фон, яркий монохромный зеленый / белый акцент
  uint16_t COLOR_BG       = ILI9341_BLACK;
  uint16_t COLOR_TXT      = ILI9341_GREEN;
  uint16_t COLOR_ACCENT   = ILI9341_DARKGREEN;
  uint16_t COLOR_HEADER_BG= ILI9341_GREEN;
  uint16_t COLOR_HEADER_TXT= ILI9341_BLACK;

  // 1. СТАТИКА: Рисуем структуру экрана один раз
  if (update_static_elements) {
    display.fillScreen(COLOR_BG);
    
    // Внешняя аккуратная хакерская рамка со скошенными углами
    display.drawRect(2, 2, 236, 316, COLOR_TXT);
    DrawCornerBrackets(0, 0, 240, 320, 6, COLOR_TXT);

    // Инверсная шапка в стиле Flipper Zero
    display.fillRect(4, 4, 232, 26, COLOR_HEADER_BG);
    display.setTextColor(COLOR_HEADER_TXT);
    display.setTextSize(2);
    display.setCursor(12, 9);
    display.print(">> NET_SCAN //");

    // Инверсный шильдик режима справа
    display.fillRect(178, 8, 52, 18, COLOR_HEADER_TXT);
    display.setTextColor(COLOR_HEADER_BG);
    display.setTextSize(1);
    display.setCursor(183, 13);
    display.print("SNIFF");

    // Заголовки блоков с символьным декором
    display.setTextColor(COLOR_TXT);
    display.setTextSize(1);

    display.setCursor(10, 38);  display.print("[+] TARGET SSID:");
    display.drawFastHLine(8, 76, 224, COLOR_ACCENT);

    display.setCursor(10, 84);  display.print("[+] SIGNAL LEVEL (RSSI):");
    display.drawFastHLine(8, 126, 224, COLOR_ACCENT);

    display.setCursor(10, 134); display.print("[+] MAC ADDRESS (BSSID):");
    display.drawFastHLine(8, 176, 224, COLOR_ACCENT);

    display.setCursor(10, 184); display.print("[+] RADIO CHANNEL:");
    display.drawFastHLine(8, 226, 224, COLOR_ACCENT);

    display.setCursor(10, 234); display.print("[+] ENCRYPTION PROTOCOL:");

    // Инверсный подвал для статуса
    display.fillRect(4, 292, 232, 24, COLOR_HEADER_BG);

    update_static_elements = false;
  }

  // 2. ДИНАМИКА: Точечно очищаем и перерисовываем только изменяемые блоки
  display.setTextColor(COLOR_TXT, COLOR_BG);
  display.setTextSize(2);

  // Данные SSID
  display.fillRect(9, 50, 220, 22, COLOR_BG);
  display.setCursor(10, 52);
  if (ssid.length() > 18) ssid = ssid.substring(0, 15) + "...";
  display.print(ssid);

  // Данные Сигнала (число в dBm)
  display.fillRect(9, 98, 120, 22, COLOR_BG);
  display.setCursor(10, 100);
  display.print(rssi); display.print("dBm");

  // Хакерский индикатор сигнала (пиксельные сегменты + пунктирная сетка)
  display.fillRect(140, 94, 85, 28, COLOR_BG);
  display.drawRect(140, 94, 85, 28, COLOR_ACCENT);
  
  int bars = map(rssi, -100, -50, 1, 6);
  bars = constrain(bars, 1, 6);
  for (int b = 0; b < 6; b++) {
    int barHeight = (b + 1) * 3 + 2;
    if (b < bars) {
      display.fillRect(145 + (b * 12), 118 - barHeight, 8, barHeight, COLOR_TXT);
    } else {
      // Рисуем контур незаполненного деления (дитеринг-эффект)
      display.drawRect(145 + (b * 12), 118 - barHeight, 8, barHeight, COLOR_ACCENT);
    }
  }

  // Данные MAC-адреса
  display.fillRect(9, 148, 220, 22, COLOR_BG);
  display.setCursor(10, 150);
  bssid.toUpperCase();
  display.print(bssid);

  // Данные Канала
  display.fillRect(9, 198, 220, 22, COLOR_BG);
  display.setCursor(10, 200);
  display.print("CH #"); display.print(chanel);

  // Данные Шифрования
  display.fillRect(9, 248, 220, 38, COLOR_BG);
  display.setCursor(10, 250);
  if (encryption.length() > 18) display.setTextSize(1);
  display.print(encryption);
  display.setTextSize(2);

  // Обновление подвала (информация о листании)
  display.fillRect(4, 292, 232, 24, COLOR_HEADER_BG);
  display.setTextColor(COLOR_HEADER_TXT);
  display.setTextSize(1);
  display.setCursor(10, 300);
  display.print("NET: ");
  display.print(current_net_index + 1);
  display.print("/");
  display.print(total_found_nets);
  
  display.setCursor(140, 300);
  display.print("[UP/DN] NAV");

  Serial.println("отрисовка завершена");
}

void DrawDevice(uint8_t index) {
  Serial.print("сейчас в функции draw_device будет отрисована страница:");
  Serial.println(index);

  uint16_t COLOR_BG    = ILI9341_BLACK;
  uint16_t COLOR_TXT   = ILI9341_GREEN;
  uint16_t COLOR_ACC   = ILI9341_DARKGREEN;

  display.fillScreen(COLOR_BG);
  
  // Рамка с хакерскими уголочками
  display.drawRect(2, 2, 236, 316, COLOR_TXT);
  DrawCornerBrackets(0, 0, 240, 320, 8, COLOR_TXT);

  // Инверсный заголовок
  display.fillRect(4, 4, 232, 24, COLOR_TXT);
  display.setTextColor(COLOR_BG);
  display.setTextSize(2);
  display.setCursor(10, 8);
  display.print("DEVICE INFO");

  display.setTextColor(COLOR_TXT);
  
  // Блок IP
  display.setCursor(10, 38);
  display.setTextSize(1);
  display.println("> TARGET IP ADDRESS:");
  
  display.drawRect(8, 52, 224, 30, COLOR_ACC);
  display.setCursor(14, 60);
  display.setTextSize(2);
  display.println(data_device[index].ip);

  // Блок Производителя (Vendor)
  display.setCursor(10, 100);
  display.setTextSize(1);
  display.println("> VENDOR / MANUFACTURER:");

  display.drawRect(8, 114, 224, 45, COLOR_ACC);
  display.setCursor(14, 122);
  display.setTextSize(2);
  display.println(data_device[index].vendor);

  // Статусная полоса внизу
  display.fillRect(4, 292, 232, 24, COLOR_TXT);
  display.setTextColor(COLOR_BG);
  display.setTextSize(1);
  display.setCursor(10, 300);
  display.print("NODE: ");
  display.print(index + 1);
  display.print(" / ");
  display.print(total_device);

  update_static_elements = true;
}

void DrawInfoNet(IPAddress* local_ip, IPAddress* subnet_mask, IPAddress* gateway_ip) {
  uint16_t COLOR_BG  = ILI9341_BLACK;
  uint16_t COLOR_TXT = ILI9341_GREEN;
  uint16_t COLOR_ACC = ILI9341_DARKGREEN;

  display.fillScreen(COLOR_BG);

  // Хакерские рамки
  display.drawRect(2, 2, 236, 316, COLOR_TXT);
  DrawCornerBrackets(0, 0, 240, 320, 8, COLOR_TXT);

  // Инверсный заголовок
  display.fillRect(4, 4, 232, 24, COLOR_TXT);
  display.setTextColor(COLOR_BG);
  display.setTextSize(2);
  display.setCursor(10, 8);
  display.print("NET INTERFACE");

  display.setTextColor(COLOR_TXT);

  // IP адрес
  display.setCursor(10, 36);
  display.setTextSize(1);
  display.println("[+] LOCAL IP:");
  display.setCursor(14, 48);
  display.setTextSize(2);
  display.println(*local_ip);

  display.drawFastHLine(8, 72, 224, COLOR_ACC);

  // Маска
  display.setCursor(10, 80);
  display.setTextSize(1);
  display.println("");
  display.setCursor(14, 92);
  display.setTextSize(2);
  display.println(*subnet_mask);

  display.drawFastHLine(8, 116, 224, COLOR_ACC);

  // Шлюз
  display.setCursor(10, 124);
  display.setTextSize(1);
  display.println("[+] GATEWAY (ROUTER):");
  display.setCursor(14, 136);
  display.setTextSize(2);
  display.println(*gateway_ip);

  display.drawFastHLine(8, 160, 224, COLOR_ACC);
}

void DrawPorts(uint16_t index) {
  uint16_t COLOR_BG  = ILI9341_BLACK;
  uint16_t COLOR_TXT = ILI9341_GREEN;
  uint16_t COLOR_ACC = ILI9341_DARKGREEN;

  display.fillScreen(COLOR_BG);

  display.drawRect(2, 2, 236, 316, COLOR_TXT);
  DrawCornerBrackets(0, 0, 240, 320, 8, COLOR_TXT);

  // Заголовок
  display.fillRect(4, 4, 232, 24, COLOR_TXT);
  display.setTextColor(COLOR_BG);
  display.setTextSize(2);
  display.setCursor(10, 8);
  display.print("PORT SCANNER");

  display.setTextColor(COLOR_TXT);

  display.setCursor(10, 36);
  display.setTextSize(1);
  display.println("> TARGET HOST IP:");
  display.setCursor(14, 48);
  display.setTextSize(2);
  display.println(service_device[index].ip);

  display.drawFastHLine(8, 72, 224, COLOR_ACC);

  display.setCursor(10, 80);
  display.setTextSize(1);
  display.println("> OPEN SERVICES / PORTS:");

  int y_pos = 96;
  display.setTextSize(2);
  for (int i = 0; i < max_amount_net; i++) {
    if (index + i >= max_amount_net) {
      break;
    }
    if (service_device[index].ip == service_device[index + i].ip) {
      display.setCursor(14, y_pos);
      display.println(service_device[index + i].description);
      y_pos += 22;
      if (y_pos > 280) break; // Предотвращение выхода за границы нижней панели
    } else {
      break;
    }
  }

  // Подвал
  display.fillRect(4, 292, 232, 24, COLOR_TXT);
  display.setTextColor(COLOR_BG);
  display.setTextSize(1);
  display.setCursor(10, 300);
  display.print("SCAN COMPLETE");

  update_static_elements = true;
}
#include "display_ui.h"

void DrawMainScreen(String ssid, int32_t rssi, String bssid, int32_t chanel, String encryption ) {
  Serial.println("начинается отрисовка");
  uint16_t COLOR_BG   = ILI9341_WHITE;
  uint16_t COLOR_TXT  = ILI9341_BLACK;
  uint16_t COLOR_LINE = display.color565(180, 180, 180);

  // 1. СТАТИКА: Рисуем рамки и заголовки ОДИН РАЗ, чтобы экран не мерцал
  if (update_static_elements) {
    display.fillScreen(COLOR_BG);
    display.drawRect(0, 0, 240, 320, COLOR_TXT); // Внешняя рамка
    
    // Шапка
    display.fillRect(1, 1, 238, 30, COLOR_TXT);
    display.setTextColor(COLOR_BG);
    display.setTextSize(2);
    display.setCursor(45, 8);
    display.print("WIFI SCANNER");
    
    // Заголовки блоков
    display.setTextColor(COLOR_TXT);
    display.setTextSize(1);
    
    display.setCursor(10, 42);  display.print("NETWORK NAME (SSID):");
    display.drawFastHLine(5, 80, 230, COLOR_LINE);
    
    display.setCursor(10, 90);  display.print("SIGNAL LEVEL:");
    display.drawFastHLine(5, 130, 230, COLOR_LINE);
    
    display.setCursor(10, 140); display.print("MAC ADDRESS:");
    display.drawFastHLine(5, 180, 230, COLOR_LINE);
    
    display.setCursor(10, 190); display.print("WIRELESS CHANNEL:");
    display.drawFastHLine(5, 230, 230, COLOR_LINE);
    
    display.setCursor(10, 240); display.print("ENCRYPTION TYPE:");
    
    // Подвал
    display.fillRect(1, 295, 238, 24, COLOR_TXT);
    
    update_static_elements = false; // Выключаем до следующего полного сброса
  }

  // 2. ДИНАМИКА: Точечно очищаем только зоны с меняющимися данными перед выводом
  display.setTextColor(COLOR_TXT, COLOR_BG);
  display.setTextSize(2);

  // Данные SSID
  display.fillRect(9, 54, 220, 18, COLOR_BG); // Стираем старое имя
  display.setCursor(10, 55);
  if(ssid.length() > 18) ssid = ssid.substring(0, 15) + "...";
  display.print(ssid);
  
  // Данные Сигнала
  display.fillRect(9, 102, 120, 18, COLOR_BG); // Стираем старый текст dBm
  display.setCursor(10, 103);
  display.print(rssi); display.print(" dBm");

  // Иконка сигнала (затираем область и рисуем заново)
  display.fillRect(155, 92, 40, 25, COLOR_BG); 
  int bars = map(rssi, -100, -50, 1, 5);
  bars = constrain(bars, 1, 5);
  for (int b = 0; b < 5; b++) {
    int barHeight = (b + 1) * 4;
    uint16_t barColor = (b < bars) ? COLOR_TXT : display.color565(220, 220, 220);
    display.fillRect(160 + (b * 6), 115 - barHeight, 4, barHeight, barColor);
  }

  // Данные MAC
  display.fillRect(9, 152, 220, 18, COLOR_BG);
  display.setCursor(10, 153);
  bssid.toUpperCase();
  display.print(bssid);

  // Данные Канала
  display.fillRect(9, 202, 220, 18, COLOR_BG);
  display.setCursor(10, 203);
  display.print("CH "); display.print(chanel);

  // Данные Шифрования
  display.fillRect(9, 252, 220, 35, COLOR_BG); // Чуть больше область для длинных строк
  display.setCursor(10, 253);
  if (encryption.length() > 18) display.setTextSize(1);
  display.print(encryption);
  display.setTextSize(2); // Возвращаем размер

  // Обновление подвала (информация о листании)
  display.fillRect(5, 300, 230, 15, COLOR_TXT);
  display.setTextColor(COLOR_BG);
  display.setTextSize(1);
  display.setCursor(10, 303);
  display.print("Viewing: "); 
  display.print(current_net_index + 1); 
  display.print(" / "); 
  display.print(total_found_nets);
  Serial.println("отрисовка завершена");
}

void DrawDevice(uint8_t index){
  Serial.print("сейчас в функции draw_device будет отрисована страница:");
  Serial.println(index);
  uint16_t color_elements = ILI9341_WHITE;

  display.fillScreen(ILI9341_BLACK);
  display.setTextColor(color_elements);
  display.setTextSize(2);

  display.drawLine(5,5,235,5,color_elements);
  display.drawLine(235,5,235,315,color_elements);
  display.drawLine(235,315,5,315,color_elements);
  display.drawLine(5,315,5,5,color_elements);

  display.setCursor(10,10);
  display.setTextSize(2);
  display.println("IP DEVICE:");
  display.setCursor(10,display.getCursorY());
  display.println(data_device[index].ip);
  display.println("");
  display.setCursor(10,display.getCursorY());
  display.println("MANUFACTURER DEVICE:");
  display.setCursor(10,display.getCursorY());
  display.println(data_device[index].manufacturer);
  update_static_elements = true;
}

void DrawInfoNet(IPAddress* local_ip,IPAddress* subnet_mask,IPAddress* gateway_ip){
  uint16_t color_elements = ILI9341_WHITE;

  display.fillScreen(ILI9341_BLACK);
  display.setTextColor(color_elements);
  display.setTextSize(2);

  display.drawLine(5,5,235,5,color_elements);
  display.drawLine(235,5,235,315,color_elements);
  display.drawLine(235,315,5,315,color_elements);
  display.drawLine(5,315,5,5,color_elements);

  display.setCursor(10,10);
  display.println("LOCAL IP:");
  display.setCursor(10,display.getCursorY());
  display.println(*local_ip);
  display.println("");
  display.setCursor(10,display.getCursorY());
  display.println("SUBNET MASK:");
  display.setCursor(10,display.getCursorY());
  display.println(*subnet_mask);
  display.println("");
  display.setCursor(10,display.getCursorY());
  display.println("GATEWAY (ROUTER):");
  display.setCursor(10,display.getCursorY());
  display.println(*gateway_ip);
}

void DrawPorts(uint16_t index){
  uint16_t color_elements = ILI9341_WHITE;

  display.fillScreen(ILI9341_BLACK);
  display.setTextColor(color_elements);
  display.setTextSize(2);

  display.drawLine(5,5,235,5,color_elements);
  display.drawLine(235,5,235,315,color_elements);
  display.drawLine(235,315,5,315,color_elements);
  display.drawLine(5,315,5,5,color_elements);

  display.setCursor(10,10);
  display.println("IP DEVICE:");
  display.setCursor(10,display.getCursorY());
  display.println(service_device[index].ip);
  display.println("");
  display.setCursor(10,display.getCursorY());
  display.println("service:");
  for (int i=0;i<max_amount_net;i++){
    if (index + i>=max_amount_net){
      break;
    }
    if (service_device[index].ip == service_device[index+i].ip){
      display.setCursor(10,display.getCursorY());
      display.println(service_device[index+i].description);
    }
    else{
      break;
    }
  }
  update_static_elements = true;
}

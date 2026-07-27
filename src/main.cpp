#include <Arduino.h>
#include "network_scanner.h" // Подключаем оглавление сканера
#include "display_ui.h"      // Подключаем оглавление интерфейса

// 1. ПИНЫ
const uint8_t scan_btn = D1;
const uint8_t page_btn = D6;
const uint8_t screen_mode_btn = D0;
const uint8_t TFT_DC = D2;  // Сюда подключаем DC дисплея
const uint8_t TFT_RST = D4; // Сюда подключаем RST дисплея
const uint8_t TFT_CS = D8;  // Этот пин оставляем в воздухе!

// Объект дисплея
Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// 2. ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ И МАССИВЫ (Выделяем память под них в main.cpp)
const uint8_t max_amount_net = 20;
const int amount_screen = 4;

int screen_mode = 0;
uint8_t current_net_index = 0;
uint8_t curent_index = 0; // Для перелистывания устройств и портов
uint8_t total_found_nets = 0;
uint8_t total_device = 0;
bool update_static_elements = true;

// Массивы данных
WifiNetwork data_net[max_amount_net] = {};
NetworkDevice data_device[max_amount_net] = {};
OpenService service_device[max_amount_net] = {};

// База вендоров по OUI (MAC-префиксам)
const MacVendor vendors[] = {
  {"00:1A:2B", "Ayecom"},
  {"00:0C:29", "VMware"},
  {"DC:A6:32", "Raspberry Pi"},
  {"B8:27:EB", "Raspberry Pi"},
  {"E4:5F:01", "Raspberry Pi"},
  {"00:15:5D", "Microsoft"},
  {"00:50:56", "VMware"},
  {nullptr, nullptr} // Маркер конца массива
};

// Список часто используемых портов
const KnownPort common_ports[] = {
  {80, "HTTP"},
  {443, "HTTPS"},
  {22, "SSH"},
  {21, "FTP"},
  {23, "Telnet"},
  {8080, "HTTP-Alt"},
  {53, "DNS"}
};
const uint8_t total_known_ports = sizeof(common_ports) / sizeof(common_ports[0]);

// Переменные состояния кнопок
bool last_scan_btn_result = 1;
bool last_page_btn_result = 0;
bool last_screen_mode_btn_result = 0;
void setup() {
  Serial.begin(115200);
  Serial.println(""); // Тестовый вывод для дальнейшей корректной работы
  
  display.begin(4000000);
  display.setRotation(0);  // Ориентация экрана
  display.fillScreen(ILI9341_BLACK);
  
  WiFi.mode(WIFI_STA); // Режим клиента
  WiFi.disconnect();   // Чтобы модуль не отвлекался на подключение в начале работы

  pinMode(scan_btn, INPUT_PULLUP);
  pinMode(page_btn, INPUT_PULLUP);
  pinMode(screen_mode_btn, INPUT_PULLUP);

  ScanNet();
}

void loop() {
  bool need_draw = false;
  bool scan_btn_result = digitalRead(scan_btn);
  bool page_btn_result = digitalRead(page_btn);
  bool screen_mode_btn_result = digitalRead(screen_mode_btn);
  
  if (screen_mode > amount_screen - 1) {
    screen_mode = 0;
    Serial.println(screen_mode);
  }
  
  if (screen_mode < 0) {
    screen_mode = amount_screen - 1;
    Serial.println(screen_mode);
  }

  if (screen_mode_btn_result != last_screen_mode_btn_result && screen_mode_btn_result) {
    screen_mode++;
    Serial.println("кнопка screen_mode_btn нажата");
    Serial.println("screen_mode");
    Serial.println(screen_mode);
    switch (screen_mode) {
      case 0: need_draw = true; break;
      case 1: Connect(); break;
      case 2: DrawDevice(curent_index); break;
      case 3: ScanTargetPorts(); break;
    }
  }

  if (scan_btn_result != last_scan_btn_result && scan_btn_result) {
    Serial.println("нажата кнопка D6 scan_btn");
    ScanNet();
    need_draw = true;
  }
  
  if (page_btn_result != last_page_btn_result && page_btn_result) {
    Serial.println("нажата кнопка D1 page_btn");
    Serial.print("выбран screen_mode:");
    Serial.println(screen_mode);
    Serial.print("сейчас нарисована страница:");
    if (screen_mode == 0) {
      current_net_index++;
      need_draw = true;
      Serial.println(current_net_index);
    }
    if (screen_mode == 2) {
      curent_index++;
      DrawDevice(curent_index);
      Serial.println(current_net_index);
    }
    if (screen_mode == 3) {
      curent_index++;
      DrawPorts(curent_index);
      Serial.println(current_net_index);
    }   
  }
  
  if (current_net_index > total_found_nets - 1) {
      current_net_index = 0;
      need_draw = true;
      Serial.print("сейчас нарисована страница:");
      Serial.println(current_net_index);
  }

  if (curent_index > total_device) {
    curent_index = 0;
  }

  if (need_draw) {
    Serial.println("нужно отрисовать");
    String ssid = data_net[current_net_index].ssid;
    int32_t rssi = data_net[current_net_index].rssi;
    String bssid = data_net[current_net_index].bssid;
    int32_t chanel = data_net[current_net_index].chanel;
    String encryption = data_net[current_net_index].encryption;
    Serial.println("вызвана функция отрисовки");
    DrawMainScreen(ssid, rssi, bssid, chanel, encryption);
  }

  last_scan_btn_result = scan_btn_result;
  last_page_btn_result = page_btn_result;
  last_screen_mode_btn_result = screen_mode_btn_result;
}
 
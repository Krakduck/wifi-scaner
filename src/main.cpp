#include <Arduino.h>
#include "network_scanner.h" // Подключаем оглавление сканера
#include "display_ui.h"      // Подключаем оглавление интерфейса
#include "web_server.h"      // Подключаем оглавление веб-интерфейса

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
const int amount_screen = 5;

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

// 3. Константная база вендоров
const MacVendor vendors[] = {
  // --- Apple ---
  {"50:FF:20", "Apple"}, {"00:03:93", "Apple"}, {"00:0A:27", "Apple"}, {"00:11:24", "Apple"},
  {"00:1C:10", "Apple"}, {"00:1D:4F", "Apple"}, {"00:1E:52", "Apple"}, {"00:1F:5B", "Apple"},
  {"00:1F:F3", "Apple"}, {"00:21:E9", "Apple"}, {"00:22:41", "Apple"}, {"00:23:12", "Apple"},

  // --- Xiaomi ---
  {"78:54:2E", "Xiaomi"}, {"64:09:80", "Xiaomi"}, {"AC:F7:F3", "Xiaomi"}, {"28:6C:07", "Xiaomi"},
  {"18:59:36", "Xiaomi"}, {"34:CE:00", "Xiaomi"}, {"5C:E9:1E", "Xiaomi"}, {"74:23:44", "Xiaomi"},
  {"9C:99:A0", "Xiaomi"}, {"F4:60:E2", "Xiaomi"}, {"FC:64:BA", "Xiaomi"}, {"38:A4:ED", "Xiaomi"},

  // --- Samsung ---
  {"98:59:7A", "Samsung"}, {"00:21:D2", "Samsung"}, {"A8:06:02", "Samsung"}, {"E4:E0:C5", "Samsung"},
  {"00:07:AB", "Samsung"}, {"00:12:FB", "Samsung"}, {"00:15:99", "Samsung"}, {"00:17:C9", "Samsung"},
  {"00:18:AF", "Samsung"}, {"00:1D:25", "Samsung"}, {"00:1F:CC", "Samsung"}, {"00:23:D7", "Samsung"},

  // --- ASUS ---
  {"E4:E6:6C", "ASUS"}, {"04:D9:F5", "ASUS"}, {"AC:9E:17", "ASUS"}, {"BC:EE:7B", "ASUS"},
  {"00:0E:A6", "ASUS"}, {"00:11:2F", "ASUS"}, {"00:13:D4", "ASUS"}, {"00:15:F2", "ASUS"},
  {"00:17:31", "ASUS"}, {"00:18:F3", "ASUS"}, {"00:1A:92", "ASUS"}, {"00:1B:FC", "ASUS"},

  // --- Huawei / Honor ---
  {"D4:9C:53", "Huawei"}, {"00:E0:FC", "Huawei"}, {"80:B6:86", "Huawei"}, {"F4:C4:D3", "Huawei"},
  {"00:1E:10", "Huawei"}, {"00:25:9E", "Huawei"}, {"04:25:7B", "Huawei"}, {"08:19:A6", "Huawei"},
  {"0C:37:DC", "Huawei"}, {"10:C1:72", "Huawei"}, {"14:B9:68", "Huawei"}, {"1C:1D:67", "Huawei"},

  // --- TP-Link ---
  {"A0:A3:F0", "TP-Link"}, {"50:C7:BF", "TP-Link"}, {"E8:48:B8", "TP-Link"}, {"18:D6:C7", "TP-Link"},
  {"00:0A:EB", "TP-Link"}, {"00:14:78", "TP-Link"}, {"00:19:E0", "TP-Link"}, {"00:21:27", "TP-Link"},
  {"00:23:CD", "TP-Link"}, {"00:27:19", "TP-Link"}, {"10:FE:ED", "TP-Link"}, {"14:CF:92", "TP-Link"},

  // --- Realtek ---
  {"38:B8:EB", "Realtek"}, {"E0:D5:5E", "Realtek"}, {"00:E0:4C", "Realtek"}, {"52:54:A8", "Realtek"},
  {"00:07:40", "Realtek"}, {"00:23:54", "Realtek"}, {"00:E0:4F", "Realtek"}, {"00:E0:70", "Realtek"},

  // --- Espressif ---
  {"24:0A:C4", "Espressif"}, {"30:AE:A4", "Espressif"}, {"84:0D:8E", "Espressif"}, {"CC:50:E3", "Espressif"},
  {"18:FE:34", "Espressif"}, {"24:62:AB", "Espressif"}, {"2C:3A:E8", "Espressif"}, {"3C:71:BF", "Espressif"},

  // --- Keenetic / MikroTik / Ubiquiti ---
  {"D4:6E:0E", "Keenetic"}, {"C4:AD:34", "Keenetic"}, {"00:0C:42", "MikroTik"}, {"2C:C8:1B", "MikroTik"},
  {"E8:13:2A", "MikroTik"}, {"00:15:6D", "Ubiquiti"}, {"78:8A:20", "Ubiquiti"}, {"F4:E2:C5", "Ubiquiti"},
  
  // --- Google / Raspberry Pi ---
  {"F4:F5:DB", "Google"}, {"3C:5A:B4", "Google"}, {"D8:EB:46", "Google"},
  {"B8:27:EB", "Raspberry Pi"}, {"DC:A6:32", "Raspberry Pi"}, {"E4:5F:01", "Raspberry Pi"}

};

const uint8_t totalItems = sizeof(vendors) / sizeof(vendors[0]);

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

  InitWebServer();

  ScanNet();
}

void loop() {
  HandleWebClient();

  bool need_draw = false;
  bool scan_btn_result = digitalRead(scan_btn);
  bool page_btn_result = digitalRead(page_btn);
  bool screen_mode_btn_result = digitalRead(screen_mode_btn);
  
  if (screen_mode > amount_screen - 1) {
    screen_mode = 0;
    need_draw = true;
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
      case 0: ScanNet(); break;
      case 1: need_draw = true; break;
      case 2: Connect(); break;
      case 3: DrawDevice(curent_index); break;
      case 4: ScanTargetPorts(); break;
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
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <SPI.h> 
#include <lwip/etharp.h>

#define TFT_DC   D2  // Сюда подключаем DC дисплея
#define TFT_RST  D4  // Сюда подключаем RST дисплея
#define TFT_CS   D8  // Этот пин оставляем в воздухе!

Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

//пины
#define scan_btn D1
#define page_btn D6
#define screen_mode_btn D0

//структуры
struct MakeMassivNet{
  String ssid;
  int32_t rssi;
  String bssid;
  int32_t chanel;
  String encryption;
};

struct MakeMassivDevice{
  String ip;
  String bssid;
  String manufacturer;
};

struct MacVendor {
    String prefix;
    String vendor;
};

// Структура для хранения информации об открытом порте
struct OpenService {
  String ip;
  String description;
};

// Список известных портов и их описаний
struct KnownPort {
  uint16_t port;
  String serviceName;
};

// Таблица с префиксами
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

  // --- Realtek (Сетевые адаптеры ПК, ноутбуков, ТВ) ---
  {"38:B8:EB", "Realtek"}, {"E0:D5:5E", "Realtek"}, {"00:E0:4C", "Realtek"}, {"52:54:A8", "Realtek"},
  {"00:07:40", "Realtek"}, {"00:23:54", "Realtek"}, {"00:E0:4F", "Realtek"}, {"00:E0:70", "Realtek"},

  // --- Espressif (ESP32, ESP8266, IoT) ---
  {"24:0A:C4", "Espressif"}, {"30:AE:A4", "Espressif"}, {"84:0D:8E", "Espressif"}, {"CC:50:E3", "Espressif"},
  {"18:FE:34", "Espressif"}, {"24:62:AB", "Espressif"}, {"2C:3A:E8", "Espressif"}, {"3C:71:BF", "Espressif"},

  // --- Keenetic / MikroTik / Ubiquiti ---
  {"D4:6E:0E", "Keenetic"}, {"C4:AD:34", "Keenetic"}, {"00:0C:42", "MikroTik"}, {"2C:C8:1B", "MikroTik"},
  {"E8:13:2A", "MikroTik"}, {"00:15:6D", "Ubiquiti"}, {"78:8A:20", "Ubiquiti"}, {"F4:E2:C5", "Ubiquiti"},
  
  // --- Google / Raspberry Pi ---
  {"F4:F5:DB", "Google"}, {"3C:5A:B4", "Google"}, {"D8:EB:46", "Google"},
  {"B8:27:EB", "Raspberry Pi"}, {"DC:A6:32", "Raspberry Pi"}, {"E4:5F:01", "Raspberry Pi"}
};

const uint8_t max_amount_net=15;

const uint8_t amount_screen = 4;

OpenService service_device[max_amount_net]={};

MakeMassivNet data_net[max_amount_net]={};

MakeMassivDevice data_device[max_amount_net]={};

const KnownPort common_ports[] = {
  {80,   "HTTP (Web Interface)"},
  {443,  "HTTPS (Secure Web)"},
  {22,   "SSH (Linux / Terminal)"},
  {23,   "Telnet"},
  {21,   "FTP (File Transfer)"},
  {554,  "RTSP (IP Camera Video)"},
  {1883, "MQTT (Smart Home)"},
  {8080, "HTTP-Alt"}
};
uint8_t total_known_ports = sizeof(common_ports) / sizeof(common_ports[0]);

//переменные отрисовки
uint8_t current_net_index = 0; // индекс отобржаемой сети
uint8_t curent_index=0;// индекс для отображения страниц
uint8_t total_found_nets = 0;  // кол-во найденных сетей
bool update_static_elements = true; // Флаг: нужно ли рисовать рамки и заголовки
uint8_t screen_mode =0;//текущий режим отображения
uint8_t total_device=0;//кол-во устройств в сети

//переменные состояния кнопок
bool last_scan_btn_result = 1;
bool last_page_btn_result = 0;
bool last_screen_mode_btn_result=0;

//прототипы функций
void add_net(String ssid, int32_t rssi, String bssid, int32_t chanel, String encryption);
void scan_net();
void draw_main_screen(String ssid, int32_t rssi, String bssid, int32_t chanel, String encryption );
void draw_device(uint8_t index);
void draw_info_net(IPAddress* local_ip,IPAddress* subnet_mask,IPAddress* gateway_ip);
void draw_ports(uint16_t index);
void scan_target_ports();
void Manufacturer(String bssid, int index);
void scan_device();
void conect();

void setup() {
  Serial.begin(115200);
  Serial.println("");//тестовый вывод для дальнейшей коректной работы
  
  display.begin(4000000);
  display.setRotation(0);  // Ориентация экрана
  display.fillScreen(ILI9341_BLACK);
  
  WiFi.mode(WIFI_STA);//режим клиента
  WiFi.disconnect();//чтобы модуль не отвликался на подключение в начале работы

  pinMode(scan_btn,INPUT_PULLUP);
  pinMode(page_btn,INPUT_PULLUP);
  pinMode(screen_mode_btn,INPUT_PULLUP);

  scan_net();
}

void add_net(String ssid, int32_t rssi, String bssid, int32_t chanel, String encryption){
  bool saved=false;
  bool space_found = false;
  for(int i=0;i<max_amount_net;i++){
    if (data_net[i].bssid == bssid){
      data_net[i].ssid = ssid;
      data_net[i].rssi = rssi;
      data_net[i].chanel = chanel;
      data_net[i].encryption = encryption;

      saved = true;
      space_found = true;
      break;
    }
  }
  if (!saved){
    for (int i = 0; i<max_amount_net; i++){
      if (data_net[i].bssid == ""){
        data_net[i] = {ssid,rssi,bssid,chanel,encryption};
        space_found = true;
        break;
      }
    }
  }
  if (!space_found){
    Serial.println("Переполнение списка сетей");
  }
}

void scan_net(){
  Serial.println("сканер вызван");
  int count_net = WiFi.scanNetworks();//фнукция возвращает число,кол-во найденных сетей или -1, сканирование еще продолжается (в асинхронном режиме), -2, если произошла ошибка
  //Serial.println(count_net);
  if (count_net<0){
    Serial.println("Ошибка сканирования");
  }
  else if(count_net==0){
    Serial.println("Сетей не найдено");
  }
  else{
    total_found_nets = (count_net > max_amount_net) ? max_amount_net : count_net;
    for(int i=0;i<count_net;i++){
      
      String ssid = WiFi.SSID(i); //возвращает строку
      int32_t rssi = WiFi.RSSI(i); //возвращает число. чем ближе к 0, тем лучше сигнал
      // uint8_t* bssid = WiFi.BSSID(i);//возвращает указатель на массив из 6 байтов. массив типа uint8_t [6]
      String bssid = WiFi.BSSIDstr(i); //возвращает строку
      int32_t chanel = WiFi.channel(i);//канал связи (от 1 до 13). при ошибке возвращает -1. int32_t используетя, ведь в таком формате функция возвращет число (так сделали разработчики библиотки для удобства)
      //канал связи - микрочастота внутри основной (2.4 или 5 МГц). это нужно, чтобы устройства не перебивали друг друга
      uint8_t number_encryption = WiFi.encryptionType(i);//тип шифрования. возвращет число-код. если сеть открытая возвращает ENC_TYPE_NONE
      String encryption;
      switch(number_encryption){
        case 2: encryption="WPA / PSK";break;
        case 4:encryption="WPA2 / PSK";break;
        case 5:encryption="Старый, взломанный WEP";break;
        case 7: encryption="Открытая сеть"; break;
        case 8: encryption="WPA / WPA2 / PSK"; break;
        default: encryption="Неизвестно";
      } 
      add_net(ssid,rssi,bssid,chanel,encryption);
    }
  }
  Serial.println("сканер завершил работу");
}

void draw_main_screen(String ssid, int32_t rssi, String bssid, int32_t chanel, String encryption ) {
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

void draw_device(uint8_t index){
  Serial.print("сейчас в функции draw_device будет отрисована страница:");
  Serial.println(index);
  display.fillScreen(ILI9341_WHITE);
  display.setTextColor(ILI9341_BLACK);
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("IP DEVICE:");
  display.println(data_device[index].ip);
  display.println("");
  display.println("MANUFACTURER DEVICE:");
  display.println(data_device[index].manufacturer);
  update_static_elements = true;
}

void draw_info_net(IPAddress* local_ip,IPAddress* subnet_mask,IPAddress* gateway_ip){
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("LOCAL IP:");
  display.println(*local_ip);
  display.println("");
  display.println("SUBNET MASK:");
  display.println(*subnet_mask);
  display.println("");
  display.println("GATEWAY (ROUTER):");
  display.println(*gateway_ip);
}

void draw_ports(uint16_t index){
  display.fillScreen(ILI9341_WHITE);
  display.setTextColor(ILI9341_BLACK);
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println("IP DEVICE:");
  display.println(service_device[index].ip);
  display.println("");
  display.println("service:");
  for (int i=0;i<max_amount_net;i++){
    if (index + i>max_amount_net){
      break;
    }
    if (service_device[index].ip == service_device[index+i].ip){
      display.println(service_device[index+i].description);
    }
    else{
      break;
    }
  }
  update_static_elements = true;
}

void scan_target_ports() {
  WiFiClient client;
  
  // Уменьшаем тайм-аут ожидания ответа до 200 мс (по умолчанию 5000 мс),
  // чтобы сканирование происходило быстро
  client.setTimeout(200); 

  Serial.print("Сканирование портов для IP: ");
  uint16_t open_service_count = 0;
  for (int i=0;i<total_device;i++){
    IPAddress target_ip;
    target_ip.fromString(data_device[i].ip);
    for (int j = 0; j < total_known_ports; j++) {
      if (open_service_count==max_amount_net){
        return;
      }
      uint16_t port = common_ports[j].port;
      String service = common_ports[j].serviceName;

      // Пытаемся подключиться к порту
      // client.connect принимает IPAddress и uint16_t порт, возвращает bool (1 / 0)
      if (client.connect(target_ip, port)) {
        Serial.print("  [+] Порт ");
        Serial.print(port);
        Serial.print(" ОТКРЫТ — Служба: ");
        Serial.println(service);
        service_device[open_service_count].ip=target_ip.toString();
        service_device[open_service_count].description = String(port) + " - " + String(service);;
        open_service_count++;
      
        // Закрываем соединение после успешной проверки
        client.stop(); 
      } else {
        Serial.print("  [-] Порт ");
        Serial.print(port);
        Serial.println(" закрыт.");
      }
    
      // Небольшая задержка, чтобы дать сетевому стеку обработать запросы
      delay(10); 
    }
  }
  draw_ports(0);
}

void Manufacturer(String bssid, int index) {
  Serial.println("вызвана фукнция Manufacturer");
  bssid.toUpperCase();

  // 1. Берем префикс (первые 8 символов XX:XX:XX)
  String prefix = bssid.substring(0, 8);
  Serial.print("Ищем префикс: ");
  Serial.println(prefix);
  Serial.print("MAC-адрес устройства: ");
  Serial.println(bssid);

  // 2. ПЕРВЫМ ДЕЛОМ проверяем на приватный / рандомизированный MAC
  char secondChar = prefix.charAt(1);
  if (secondChar == '2' || secondChar == '6' || secondChar == 'A' || secondChar == 'E') {
    data_device[index].manufacturer = "Private / Random";
    Serial.println("Результат: Private / Random");
    Serial.println("");
    return;
  }

  // 3. По умолчанию Unknown
  data_device[index].manufacturer = "Unknown";
  
  int totalItems = sizeof(vendors) / sizeof(vendors[0]); // Считаем количество элементов в массиве
  for (int i = 0; i < totalItems; i++) {
    if (prefix == vendors[i].prefix) {
      Serial.println(vendors[i].vendor);
      data_device[index].manufacturer = vendors[i].vendor;
      Serial.println("");
      return;
    }
  }
}

void scan_device(){
  struct eth_addr* ret_ethaddr; // Указатель, куда lwIP запишет адрес      Внутри ret_ethaddr находится структура eth_addr. А внутри нее есть массив addr из 6 байт. Это и есть 6 чисел нашего MAC-адреса (например: 0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E).       когда функия в raport() отработает, она запишет значения в эти переменные 
  const ip4_addr_t* ret_ipaddr;  
  Serial.println("функция scan_device вызвана");
  while (WiFi.status() != WL_CONNECTED) {
    display.fillScreen(ILI9341_WHITE);
    display.setCursor(80,120);
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
  IPAddress local_ip = WiFi.localIP(); //IPAddress - массив из 4 байт, каждый октет - 1 байт. можно обратиться local_ip[0]  local_ip[1]...
  IPAddress subnet_mask = WiFi.subnetMask();
  IPAddress gateway_ip = WiFi.gatewayIP();
  total_device=0;
  draw_info_net(&local_ip,&subnet_mask,&gateway_ip);
  display.setCursor(200,200);
  display.setTextColor(ILI9341_RED);
  display.print("!!!");
    for (int i=0;i<254;i++){
      if (i == 1){ //сам роутер
        continue; 
      }
      if (total_device==max_amount_net){
        break;
      }
      IPAddress target_ip(local_ip[0], local_ip[1], local_ip[2], i); //собираем новый ip по отктетам, где первые 3 зафиксированы маской сети. указатель передается вместо копии ради производительности
      if (target_ip == local_ip){
        continue;
      }
      // 1. Создаём чистую структуру lwIP
      ip4_addr_t target_ip_lwip; //создаем  структуру библиотеки LwIP,  точно также как MakeMassivNet data_net[max_amount_net]={};

      // 2. Записываем в неё числовое значение IP-адреса из нашего объекта Arduino
      target_ip_lwip.addr = (uint32_t)target_ip; //выбираем поле addrу структуры. target_ip  приводим к типу uint32_t
      etharp_request(netif_default, &target_ip_lwip); //broadcast(широковещательный) запрос всем устройствам, чтобы проверить чей ip. устройство в ответ присылает свой mac-адрес, который записывается в arp-таблицу. в функцию передаем указатель на всю структуру
      delay(100);
      s8_t result = etharp_find_addr(netif_default, &target_ip_lwip, &ret_ethaddr, &ret_ipaddr); //Принимает: сетевой интерфейс, целевой IP, указатель на переменную для MAC-адреса (&ret_ethaddr) и на IP (&ret_ipaddr). Возвращает: индекс записи в ARP-таблице (>= 0), если устройство ответило, или -1, если устройство не найдено.
      String bssid="";
      if (result >=0){
        for (int j = 0; j < 6; j++) {
          uint8_t current_byte = ret_ethaddr->addr[j];
          // 1. Если это не первый байт (j > 0), сначала добавляем двоеточие-разделитель
          if (j > 0) {
          bssid += ":";
          }
          // 2. Если байт маленький, добавляем ведущий ноль
          if (current_byte < 16) {
          bssid += "0";
          }
           // 3. Добавляем сам байт в HEX
          bssid += String(current_byte, HEX);
        }
        data_device[total_device].ip = target_ip.toString();
        data_device[total_device].bssid = bssid; 
        Manufacturer(bssid,total_device);
        total_device++;
      }
    }
  display.fillRect(200,200,240,320,ILI9341_WHITE);
  Serial.println(total_device);
  update_static_elements = true;
}

void conect(){
  WiFi.disconnect();
  Serial.println("функция conect вызвана");
  String ssid = data_net[current_net_index].ssid;
  String encryption = data_net[current_net_index].encryption;
  if (encryption == "Открытая сеть"){
    WiFi.begin(ssid);
  }
  else{
    if(ssid=="dlink3"){
      WiFi.begin(ssid,"nSHDsozm3105");
    }
    else if(ssid=="dlink2"){
      WiFi.begin(ssid,"147852369");
    }
  }
  scan_device();
}

void loop() {
  bool need_draw = false;
  bool scan_btn_result = digitalRead(scan_btn);
  bool page_btn_result = digitalRead(page_btn);
  bool screen_mode_btn_result = digitalRead(screen_mode_btn);
  
  if (screen_mode >amount_screen-1 ){
    screen_mode=0;
    Serial.println(screen_mode);
  }
  
  if (screen_mode <0 ){
    screen_mode = amount_screen-1;
    Serial.println(screen_mode);
  }

  if (screen_mode_btn_result != last_screen_mode_btn_result && screen_mode_btn_result){
    screen_mode++;
    Serial.println("кнопа screen_mode_btn нажата");
    Serial.println("screen_mode");
    Serial.println(screen_mode);
    switch (screen_mode){
      case 0: need_draw = true; break;
      case 1: conect();break;
      case 2: draw_device(curent_index);break;
      case 3: scan_target_ports(); break;
    }
  }

  if (scan_btn_result!=last_scan_btn_result && scan_btn_result){
    Serial.println("нажат кнопка D6 scan_btn");
    scan_net();
    need_draw = true;
  }
  
  if (page_btn_result!=last_page_btn_result && page_btn_result){
    Serial.println("нажата кнопка D1 page_btn");
    Serial.print("выбран screen_mode:");
    Serial.println(screen_mode);
    Serial.print("сейчас нарисована страница:");
    if (screen_mode==0){
      current_net_index++;
      need_draw=true;
      Serial.println(current_net_index);
    }
    if (screen_mode==2){
      curent_index++;
      draw_device(curent_index);
      Serial.println(current_net_index);
    }
    if (screen_mode==3){
      curent_index++;
      draw_ports(curent_index);
      Serial.println(current_net_index);
    }   
  }
  
  if (current_net_index >total_found_nets-1){
      current_net_index=0;
      need_draw=true;
      Serial.print("сейчас нарисована страница:");
      Serial.println(current_net_index);
  }

  if (curent_index>total_device){
    curent_index=0;
  }

  if (need_draw){
    Serial.println("нужно отрисовать");
    String ssid = data_net[current_net_index].ssid;
    int32_t rssi = data_net[current_net_index].rssi;
    String bssid = data_net[current_net_index].bssid;
    int32_t chanel = data_net[current_net_index].chanel;
    String encryption = data_net[current_net_index].encryption;
    Serial.println("вызвана функция отрисовки");
    draw_main_screen(ssid,rssi,bssid, chanel,encryption);
  }

  last_scan_btn_result = scan_btn_result;
  last_page_btn_result = page_btn_result;
  last_screen_mode_btn_result = screen_mode_btn_result;
}
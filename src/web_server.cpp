#include "web_server.h"
#include "network_scanner.h"
#include "display_ui.h"

ESP8266WebServer server(80);

void handleRoot(){
  String html ="";

  html+="<!DOCTYPE html>";
  html+="<html lang=\"ru\">";
  html+="<head>";
  html+="    <meta charset=\"UTF-8\">";
  html+="    <title>ESP8266</title>";
  html+="</head>";
  html+="<body>";
  html+="    <h1>Web-управление ESP8266</h1>";
  html+="    <a href=\"/nextmode\"><button>Следующий режим сканера</button></a>";
  html+="    <a href=\"/prevmode\"><button>Предыдущий режим сканера</button></a>";
  html+="</body>";
  html+="</html>";
  
  server.send(200, "text/html", html);
}

void ModeNext() {
  screen_mode++; 
  Serial.println("кнопка screen_mode_btn нажата");
  Serial.println("screen_mode");
  Serial.println(screen_mode);
  switch (screen_mode) {
      case 0: ScanNet(); break;
      case 1: WebDrawMainScreen(); break;
      case 2: Connect(); break;
      case 3: WebDrawDevice(); break;
      case 4: ScanTargetPorts(); break;
    }              
  //server.send(200, "text/plain", "OK");
}

void ModePrev() {
  screen_mode--;   
  Serial.println("кнопка screen_mode_btn нажата");
  Serial.println("screen_mode");
  Serial.println(screen_mode);  
  switch (screen_mode) {
      case 0: ScanNet(); break;
      case 1: WebDrawMainScreen(); break;
      case 2: Connect(); break;
      case 3: WebDrawDevice(); break;
      case 4: ScanTargetPorts(); break;
    }          

  //server.send(200, "text/plain", "OK");
}

void WebDrawPorts(){
  String html = "";

  html+="<!DOCTYPE html>";
  html+="<html lang=\"ru\">";
  html+="<head>";
  html+="    <meta charset=\"UTF-8\">";
  html+="    <title>ESP8266</title>";
  html+="</head>";
  html+="<body>";
  html+="    <h1>WebDrawDevice</h1>";
  for (int i =0;i<max_amount_net;i++){
    if (service_device[i].ip){
      html+="    TARGET HOST IP: "+ service_device[i].ip;
      html+="    OPEN SERVICES / PORTS: "+ service_device[i].description;
    }
  }
  html+="    <a href=\"/nextmode\"><button>Следующий режим сканера</button></a>";
  html+="    <a href=\"/prevmode\"><button>Предыдущий режим сканера</button></a>";
  html+="</body>";
  html+="</html>";
  
  server.send(200, "text/html", html);
};

void WebDrawInfoNet(IPAddress* local_ip, IPAddress* subnet_mask, IPAddress* gateway_ip){
  String html = "";

  html+="<!DOCTYPE html>";
  html+="<html lang=\"ru\">";
  html+="<head>";
  html+="    <meta charset=\"UTF-8\">";
  html+="    <title>ESP8266</title>";
  html+="</head>";
  html+="<body>";
  html+="    <h1>WebDrawDevice</h1>";
  html+="    [+] LOCAL IP: "+ (*local_ip).toString();
  html+="    [+] SUBNET MASK: "+ (*subnet_mask).toString();
  html+="    [+] GATEWAY (ROUTER): "+ (*gateway_ip).toString();
  html+="    <a href=\"/nextmode\"><button>Следующий режим сканера</button></a>";
  html+="    <a href=\"/prevmode\"><button>Предыдущий режим сканера</button></a>";
  html+="</body>";
  html+="</html>";
  
};

void WebDrawDevice(){
  String html = "";
  Serial.print("сейчас в функции draw_device будет отрисована страница:");

  html+="<!DOCTYPE html>";
  html+="<html lang=\"ru\">";
  html+="<head>";
  html+="    <meta charset=\"UTF-8\">";
  html+="    <title>ESP8266</title>";
  html+="</head>";
  html+="<body>";
  html+="    <h1>WebDrawDevice</h1>";
  for (int i =0;i<max_amount_net;i++){
    if (data_device[i].ip){
      html+="    TARGET IP ADDRESS: "+ data_device[i].ip;
      html+="    VENDOR: "+ data_device[i].vendor;
    }
  }
  html+="    <a href=\"/nextmode\"><button>Следующий режим сканера</button></a>";
  html+="    <a href=\"/prevmode\"><button>Предыдущий режим сканера</button></a>";
  html+="</body>";
  html+="</html>";
  
  server.send(200, "text/html", html);
};

void WebDrawMainScreen(){
  String html = "";
  Serial.println("начинается отрисовка");

  html+="<!DOCTYPE html>";
  html+="<html lang=\"ru\">";
  html+="<head>";
  html+="    <meta charset=\"UTF-8\">";
  html+="    <title>ESP8266</title>";
  html+="</head>";
  html+="<body>";
  html+="    <h1>WebDrawMainScreen</h1>";
  for (int i =0;i<max_amount_net;i++){
    if (data_net[i].ssid){
      html+="    ssid: "+ data_net[i].ssid;
      html+="    rssi: "+ data_net[i].rssi;
      html+="    bssid: "+ data_net[i].bssid;
      html+="    chanel: "+ data_net[i].chanel;
      html+="    encryption: "+ data_net[i].encryption;
    }
  }
  html+="    <a href=\"/nextmode\"><button>Следующий режим сканера</button></a>";
  html+="    <a href=\"/prevmode\"><button>Предыдущий режим сканера</button></a>";
  html+="</body>";
  html+="</html>";
  
  server.send(200, "text/html", html);
};

//static void DrawCornerBrackets(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t len, uint16_t color){};

void InitWebServer() {
  Serial.println("сервер настроен");
  server.on("/", handleRoot);
  server.on("/nextmode", ModeNext); 
  server.on("/prevmode", ModePrev); 
  server.begin();
}

void HandleWebClient() {
  server.handleClient(); //постоянная прверка запросов
}
#include "web_server.h"
#include "network_scanner.h"
#include "display_ui.h"

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

void handleModeNext() {
  screen_mode++; 
  if (screen_mode > amount_screen-1){
    screen_mode = 0;
  } 

  switch (screen_mode) {
    case 0: 
    case 1: server.sendHeader("Location", "/mainscreen"); break;
    case 2: need_scan_device=true;
    case 3: server.sendHeader("Location", "/device"); break;
    case 4: need_scan_ports=true;
    case 5: server.sendHeader("Location", "/ports"); break;
  }
  server.send(303); // Отправляем редирект прямо браузеру!        
}

void handleModePrev() {
  screen_mode--;            
  if (screen_mode < 0){
    screen_mode = amount_screen-1;
  } 
  server.send(200, "text/plain", "OK");
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
    if (service_device[i].ip !=""){
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
  server.send(200, "text/html", html);
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
    if (data_device[i].ip != ""){
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
    if (data_net[i].ssid != ""){
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
  server.on("/nextmode", handleModeNext); 
  server.on("/prevmode", handleModePrev); 
  server.on("/ports", WebDrawPorts); 
  //server.on("/infonet", WebDrawInfoNet); 
  server.on("/device", WebDrawDevice); 
  server.on("/mainscreen", WebDrawMainScreen); 
  server.begin();
}

void HandleWebClient() {
  server.handleClient(); //постоянная прверка запросов
}
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
  html+="    <a href=\"/next\"><button>Следующая страница</button></a>";
  html+="    <a href=\"/prev\"><button>Предыдущая страница</button></a>";
  html+="    <a href=\"/mnextmode\"><button>Следующий режим сканера</button></a>";
  html+="    <a href=\"/prevmode\"><button>Предыдущий режим сканера</button></a>";
  html+="</body>";
  html+="</html>";
  

  server.send(200, "text/html", html);
}

void handleNext() {
  current_net_index++;           
  //update_static_elements = true; 
  server.send(200, "text/plain", "OK");
}

void handlePrev() {
  current_net_index--;           
  //update_static_elements = true; 
  server.send(200, "text/plain", "OK");
}

void ModeNext() {
  screen_mode++;           
  update_static_elements = true; 
  server.send(200, "text/plain", "OK");
}

void ModePrev() {
  screen_mode--;           
  update_static_elements = true; 
  server.send(200, "text/plain", "OK");
}

void InitWebServer() {
  Serial.println("сервер настроен");
  server.on("/", handleRoot);
  server.on("/next", handleNext);  //разовая настройка инструкций маршрутизации
  server.on("/prev", handlePrev); 
  server.on("/nextmode", ModeNext); 
  server.on("/prevmode", ModePrev); 
  server.begin();
}

void HandleWebClient() {
  server.handleClient(); //постоянная прверка запросов
}
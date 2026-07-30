#ifndef WEB_SERVER_H
#define WEB_SEHVER_H

#include <ESP8266WebServer.h>

extern ESP8266WebServer server;

extern const uint8_t amount_screen;
extern bool need_scan_device;
extern bool need_scan_ports;

void handleRoot();
void handleModeNext();
void handleModePrev();

void WebDrawPorts();
void WebDrawInfoNet(IPAddress* local_ip, IPAddress* subnet_mask, IPAddress* gateway_ip);
void WebDrawDevice();
void WebDrawMainScreen();

void InitWebServer();

void HandleWebClient();


#endif
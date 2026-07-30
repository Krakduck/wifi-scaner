#ifndef WEB_SERVER_H
#define WEB_SEHVER_H

#include <ESP8266WebServer.h>

void handleRoot();
void handleNext();
void handlePrev();
void ModeNext();
void ModePrev();

void WebDrawPorts();
void WebDrawInfoNet(IPAddress* local_ip, IPAddress* subnet_mask, IPAddress* gateway_ip);
void WebDrawDevice();
void WebDrawMainScreen();

void InitWebServer();

void HandleWebClient();


#endif
#ifndef WEB_SERVER_H
#define WEB_SEHVER_H

#include <ESP8266WebServer.h>

void handleRoot();
void handleNext();
void handlePrev();
void ModeNext();
void ModePrev();

void InitWebServer();

void HandleWebClient();


#endif
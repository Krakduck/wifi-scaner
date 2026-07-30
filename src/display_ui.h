#ifndef DISPLAY_UI_H
#define DISPLAY_UI_H

#include <Arduino.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <SPI.h> 
#include "network_scanner.h" // Чтобы файл знал про структуры NetworkDevice и OpenService

// 1. Указатель/объект дисплея, созданный в main.cpp
extern Adafruit_ILI9341 display;

// 2. Внешние переменные (память под них уже выделена в main.cpp)
extern const uint8_t max_amount_net;
extern const uint8_t max_amount_device;
extern const uint8_t max_amount_port;
extern uint8_t current_net_index;      // Индекс отображаемой сети
extern uint8_t curent_index;           // Индекс для отображения страниц устройств/портов
extern uint8_t total_found_nets;       // Кол-во найденных сетей
extern bool update_static_elements;    // Флаг: нужно ли рисовать рамки и заголовки
extern int screen_mode;                // Текущий режим отображения
extern uint8_t total_device;           // Кол-во устройств в сети

// 3. Внешние массивы данных
extern NetworkDevice data_device[];
extern OpenService service_device[];

// 4. Прототипы функций отрисовки
void DrawMainScreen(uint8_t index);
void DrawDevice(uint8_t index);
void DrawInfoNet(IPAddress* local_ip, IPAddress* subnet_mask, IPAddress* gateway_ip);
void DrawPorts(uint16_t index);

#endif
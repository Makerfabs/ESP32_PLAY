#ifndef KEY_H
#define KEY_H

#include <Arduino.h>
#include <PCF8574.h>
#include "makerfabs_pin.h"

#define B_VALUE_UP 0
#define B_VALUE_DOWN 1
#define B_VALUE_LEFT 2
#define B_VALUE_RIGHT 3

#define B_VALUE_SELECT 4
#define B_VALUE_START 5

#define B_VALUE_A 6
#define B_VALUE_B 7
#define B_VALUE_L 8
#define B_VALUE_R 9

#define B_VALUE_MENU 10

void key_init();
int get_key_value();
int key_scanf();

#endif
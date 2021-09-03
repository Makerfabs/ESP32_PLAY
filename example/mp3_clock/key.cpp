#include "key.h"

extern PCF8574 pcf8574;

void key_init()
{
    for (int i = 0; i < 8; i++)
    {
        pcf8574.pinMode(i, INPUT);
    }
    pinMode(B_L, INPUT);
    pinMode(B_R, INPUT);
    pinMode(B_MENU, INPUT);

    if (pcf8574.begin())
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("KO");
    }
}

int get_key_value()
{
    int temp_a = key_scanf();
    if (temp_a == -1)
        return temp_a;
    delay(40);
    int temp_b = key_scanf();

    /*
    while (key_scanf() != -1)
    {
        delay(40);
    }
    */

    if (temp_a == temp_b)
    {
        Serial.printf("Key value is : %d\n", temp_a);
        delay(100);
        return temp_a;
    }
    else
        return -1;
}

int key_scanf()
{
    PCF8574::DigitalInput val = pcf8574.digitalReadAll();
    if (val.B_START == LOW)
        return B_VALUE_START;
    if (val.B_SELECT == LOW)
        return B_VALUE_SELECT;
    if (val.B_UP == LOW)
        return B_VALUE_UP;
    if (val.B_DOWN == LOW)
        return B_VALUE_DOWN;
    if (val.B_LEFT == LOW)
        return B_VALUE_LEFT;
    if (val.B_RIGHT == LOW)
        return B_VALUE_RIGHT;
    if (val.B_A == LOW)
        return B_VALUE_A;
    if (val.B_B == LOW)
        return B_VALUE_B;
    if (digitalRead(B_L) == LOW)
        return B_VALUE_L;
    if (digitalRead(B_R) == LOW)
        return B_VALUE_R;
    if (digitalRead(B_MENU) == LOW)
        return B_VALUE_MENU;

    return -1;
}
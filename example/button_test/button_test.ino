#include <Wire.h>
#include "PCF8574.h"
#include "makerfabs_pin.h"

PCF8574 pcf8574(PCF_ADD);

void setup()
{

    Serial.begin(115200);
    while (!Serial)
        ; // Leonardo: wait for serial monitor
    Serial.println("\nI2C Scanner");

    Wire.begin(ESP32_SDA, ESP32_SCL);

    scanf_i2c();

    Serial.print("Init pcf8574...");
    key_init();
    if (pcf8574.begin())
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("KO");
    }
}

void loop()
{
    key_scanf();
    delay(100);
}

void scanf_i2c()
{
    byte error, address;
    int nDevices;

    static int s = 0;
    Serial.print(s++);
    Serial.println(". Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++)
    {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println("  !");

            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
    {
        Serial.print(">>>> Found total ");
        ;
        Serial.print(nDevices);
        Serial.println(" devices\n");
    }
}

void key_init()
{
    for (int i = 0; i < 8; i++)
    {
        pcf8574.pinMode(i, INPUT);
    }
    pinMode(B_L, INPUT);
    pinMode(B_R, INPUT);
}

int key_scanf()
{
    PCF8574::DigitalInput val = pcf8574.digitalReadAll();
    if (val.B_START == LOW)
        Serial.println("B_START PRESSED");
    if (val.B_SELECT == LOW)
        Serial.println("B_SELECT PRESSED");
    if (val.B_UP == LOW)
        Serial.println("B_UP PRESSED");
    if (val.B_DOWN == LOW)
        Serial.println("B_DOWN PRESSED");
    if (val.B_LEFT == LOW)
        Serial.println("B_LEFT PRESSED");
    if (val.B_RIGHT == LOW)
        Serial.println("B_RIGHT PRESSED");
    if (val.B_A == LOW)
        Serial.println("B_A PRESSED");
    if (val.B_B == LOW)
        Serial.println("B_B PRESSED");
    if (digitalRead(B_L) == LOW)
        Serial.println("B_L PRESSED");
    if (digitalRead(B_R) == LOW)
        Serial.println("B_R PRESSED");

    return 0;
}
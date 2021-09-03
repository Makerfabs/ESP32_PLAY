# ESP32 PLAY (NOT READY)


```c++
/*
Version:		V1.0
Author:			Vincent
Create Date:	2021/8/31
Note:
	
*/
```



![](md_pic/ESPlay.jpg)



[toc]

# Makerfabs

[Makerfabs home page](https://www.makerfabs.com/)

[Makerfabs Wiki](https://makerfabs.com/wiki/index.php?title=Main_Page)



# ESP32 PLAY

## Intruduce

Product Link ：[ESPlay Micro - Open Source ESP32 Game Console](https://www.makerfabs.com/esplay-micro.html) 

Wiki Link :  []() 



ESPlay Micro is a Handheld game console based on ESP32, which is from Espressif with great functionality and really mature on the software development side. This project most inspired by Odroid Go and Pocketsprite, those devices are great and have well community support, but some of them had some issues that can be updated. Odroid Go had shared its SPI for both display and SD card, and they need patched esp-IDF version for it and SD card not work very well in SPI mode, Then Pocketsprite only has esp32 wroom module without psram and also without SD card support so limited portability for saving collection of games.

So that is what this ESPlay micro updated. With SD card support added, customers can collect many more games. High quality sound I2S DAC also added to make It possible a good Walkman.

Note that there only the PCBA, USB cable, and SD card, without case and battery, you will need a lipo battery, or USB charger, to start to play. Makerfabs is also designing some cases for this, but need some more time for us to make a reasonable solution.


## Feature

- ESP32 WROVER, Dual-core processor with Integrated 4MB Flash + 4MB PSRAM
- Integrated WIFI and Bluetooth 4 BLE
- 2,4" ILI9341 TFT Panel
- More button (expanded via PCF8574 I2C GPIO)
- Micro SD slot connected to SDMMC Host in 1 Line Mode for saving GPIO pin
- Integrated I2S DAC via UDA1334A
- Integrated USB to Serial for programming and debugging using CH340C
- Built-in TP4056 Li-Po charger
- I2C port for expandable function
- 3.5mm Headphone jack
- Smaller Size, PCB only 100 x 50 mm



### Front:

![front](md_pic/front~1.jpg)

### Back:

![back](md_pic/back~1.jpg)


# Example

# 例程



## Equipment list

## 器材清单

- ESP32 Touch Camera ILI9341
- Micro SD card


## Compiler Options

## 编译选项和库

**If you have any questions，such as how to install the development board, how to download the code, how to install the library. Please refer to :[Makerfabs_FAQ](https://github.com/Makerfabs/Makerfabs_FAQ)**

- Install board : ESP32 .
- Install library : Adafruit_GFX library.
- Install library : Adafruit_ILI9341 library.
- Install library : Adafruit_STMPE610 library.

- Insert a Micro SD card into ESP32 Touch Camera.
- Upload codes, select "ESP32 Wrover Module" and "Huge APP"


## Example List

## 例程列表

### Camera

This is a test demo. You can check that ESP32, screen, touch, SD card reader and the camera function properly.

- Copy logo.bmp to sd card.
- Insert SD card.
- Upload code to ESP32.
- Restart ESP32.
- Screen will show "TOUCH TO START TEST", please touch screen.
- Will show logo picture in SD card.
- And ESP32 will open camera, and show on screen.

### Camera_faster

Use LovyanGFX library. More faster than Adafruit_GFX. This example don't need SD card.

- Upload code to ESP32.
- Restart ESP32.
- Screen will show "TOUCH TO START TEST", please touch screen.
- And ESP32 will open camera, and show on screen.

### Graphic_Test

- Test ILI9341 screen with Adafruit_ILI9341 library.

### Touch_Draw

- Use your hand or stylus (resistance screen) to draw on the screen.

![Touch_Draw](E:/资料储备/git模板/md_pic/draw2~1.jpg)

### SD_Test

- Display pictures from SD card.

![SD2TFT](E:/资料储备/git模板/md_pic/logo~1.jpg)

### Wifi_Camera

- Use esp32 and ov2640 as a webcam.
- Provides a real-time display of the control web page.



### Screen Shot Receiver

Transmit the contents of the monitor to the TFT screen via Wifi.A host computer software is provided to box select the areas of the screen that need to be transferred.

Change from [MakePython ESP32 Color LCD で 動画をWiFi受信](https://homemadegarbage.com/makerfabs05)

- Upload codes.

- Open "ScreenShotSender.exe"

- Input IP on the screen.

![SD2TFT](E:/资料储备/git模板/md_pic/ss-1~1.jpg)

- Click "Connect"

![SD2TFT](E:/资料储备/git模板/md_pic/ss-2~1.jpg)

  `

### Album

Read JPG image from SD card and display it on screen.You can switch photos by clicking on the left or right side of the screen.For the sake of display, it is best not to be larger than 320 * 240 pixels.

- Copy the image from the JPG folder to the SD root directory.
- Insert SD card.
- Upload code to ESP32.
- Restart ESP32.
- Will show jpg picture in SD card.

![album](E:/资料储备/git模板/md_pic/album~1.jpg)



## Code Explain

## 必要代码说明

- Import Library

```c++
#include "SPI.h"
#include "SD.h"
#include "FS.h"
#include "Adafruit_STMPE610.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
```



- Define Pins

```c++
//SPI
#define SPI_MOSI 13
#define SPI_MISO 12
#define SPI_SCK 14

//SD Card
#define SD_CS 4

//Touch Screen
#define STMPE_CS 2

//TFT
#define TFT_CS 15
#define TFT_DC 33
#define TFT_LED -1 //1//-1
#define TFT_RST -1 //3//-1
```



- Set Cs function. Because the screen, touch and SD card all use SPI interface. To prevent SPI conflicts, the manual SPI switch is set. Low is enable.

```c++
//SPI control
#define SPI_ON_TFT digitalWrite(TFT_CS, LOW)
#define SPI_OFF_TFT digitalWrite(TFT_CS, HIGH)
#define SPI_ON_SD digitalWrite(SD_CS, LOW)
#define SPI_OFF_SD digitalWrite(SD_CS, HIGH)
#define STMPE_ON digitalWrite(STMPE_CS, LOW)
#define STMPE_OFF digitalWrite(STMPE_CS, HIGH)
```



- Init SPI

```c++
SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
```


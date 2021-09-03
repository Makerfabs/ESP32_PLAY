
#include <SPI.h>
#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_ILI9341.h>
#include <time.h>
#include <PCF8574.h>
#include "Audio.h"
#include "SD_MMC.h"

#include "wifi_save.h"
#include "key.h"
#include "my_audio.h"
#include "makerfabs_pin.h"

//Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_DC, LCD_MOSI, LCD_SCK, LCD_RST);
Adafruit_ILI9341 tft = Adafruit_ILI9341(LCD_CS, LCD_DC, LCD_RST);
PCF8574 pcf8574(PCF_ADD);
Audio audio;

//ntp var
const char *ntpServer = "120.25.108.11";
const long gmtOffset_sec = (-5) * 60 * 60; //China+8
const int daylightOffset_sec = 0;

//clock var
struct tm timeinfo;
const int page_num = 3;
int page = 0;
int page_line = 0;
int page_add = 0;
int16_t alarm_h = 8;
int16_t alarm_m = 0;
int16_t timezone = 0;
int alarm_flag = 0;
int alarm_enable = 1;
int last_h = -1;
int last_m = -1;
int button_flag = 0;

//mp3 var
int mp3_mod_flag = 0;
int mp3_runtime = 0;
int mp3_key_runtime = 0;
int mp3_vol = 21;
int music_over_flag = 1;

String file_list[20];
int file_num = 0;
int file_index = 0;

struct Music_info
{
    String name;
    int length;
    int runtime;
    int volume;
    int status;
    int mute_volume;
} music_info = {"", 0, 0, 0, 0, 0};

void setup(void)
{
    Serial.begin(115200);
    Serial.print(F("Hello! ES32_PLAY"));

    Wire.begin(ESP32_SDA, ESP32_SCL);
    SPI.begin(LCD_SCK, LCD_MISO, LCD_MOSI);

    key_init();

    tft_init();
    congfig_init();
    wifi_init();
    sd_init();
    audio_init();

    tft.setCursor(0, 120);
    tft.print("All init over");
    delay(2000);

    //tft.fillScreen(ILI9341_BLUE);
    tft.setRotation(3);
    print_img(SD_MMC, "/logo32.bmp");
    tft.setRotation(1);
}

void loop()
{
    //main_menu();
    if (mp3_mod_flag == 0)
    {
        Serial.println("clock_mod");
        while (1)
        {
            clock_mod();
            if (button_flag == 1)
                break;
        }
    }
    else
    {
        Serial.println("mp3_mod");
        if (file_num != 0)
            while (1)
            {
                mp3_mod();
                if (button_flag == 1)
                    break;
            }
        else
        {
            mp3_mod_flag = 0;
            button_flag = 1;
            tft.fillScreen(ILI9341_BLACK);
            tft.setTextColor(ILI9341_WHITE);
            tft.setTextSize(4);
            tft.setCursor(10, 100);
            tft.print("No music in SD");
        }
    }
}

//Init
void tft_init()
{
    Serial.println("Start TFT init");

    pinMode(LCD_CS, OUTPUT);
    pinMode(LCD_DC, OUTPUT);
    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH);

    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(ILI9341_BLACK);
    delay(100);

    Serial.println("TFT init done");
    delay(100);

    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(1);
    tft.print("TFT init over");
}

void wifi_init()
{
    Serial.println("Start WIFI config and ntp init");

    tft.setCursor(0, 40);
    tft.print("WIFI init start");

    tft.setCursor(0, 60);
    tft.print("If you want set WIFI. Hold down S1 button 3 second, then clock will go into Settings Mode. Use your phone connect to Makerfabs_ap, and visit \"192.168.4.1\".");

    if (wifi_set_main())
    {
        tft.fillRect(0, 60, 320, 220, ILI9341_BLACK);
        Serial.println("Connect WIFI SUCCESS");
        tft.setCursor(0, 60);
        tft.print("Connect WIFI SUCCESS");
    }
    else
    {
        tft.fillRect(0, 60, 320, 220, ILI9341_BLACK);
        Serial.println("Connect WIFI FAULT");
        tft.setCursor(0, 60);
        tft.print("Connect WIFI FAULT");
    }

    //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    configTime((const long)(timezone * 3600), daylightOffset_sec, ntpServer);
    Serial.println(F("Alread get npt time."));
    tft.setCursor(0, 80);
    tft.print("Alread get npt time");
}

void congfig_init()
{
    read_congfig(&alarm_h, &alarm_m, &timezone);
    tft.setCursor(0, 20);
    tft.print("Alread get congfig");
    delay(500);
}

void sd_init()
{
    //SD(SDIO)
    if (!SD_MMC.begin("/sdcard", true, false))
    {
        Serial.println("Card Mount Failed");
        return;
    }
    Serial.println(F("SD load over."));
    tft.setCursor(0, 100);
    tft.print("SD load over");
    //listDir(SD_MMC, "/", 0);
}

void audio_init()
{
    file_num = get_music_list(SD_MMC, "/", 0, file_list);
    Serial.print("Music file count:");
    Serial.println(file_num);
    Serial.println("All music:");
    for (int i = 0; i < file_num; i++)
    {
        Serial.println(file_list[i]);
    }

    if (file_num == 0)
        return;

    //Audio(I2S)
    //audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT); //改了库
    audio.setVolume(mp3_vol); // 0...21

    //audio.connecttoFS(SD_MMC, "/yueliang.wav"); //ChildhoodMemory.mp3  //MoonRiver.mp3 //320k_test.mp3
    open_new_song(file_list[file_index]);
}
//Menu

void clock_mod()
{
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
    }

    switch (page)
    {
    case 0:
        clock_page();
        break;
    case 1:
        alarm_page();
        break;
    case 2:
        timezone_page();
        break;

    default:
        break;
    }

    //Check alarm clock
    if (timeinfo.tm_hour == alarm_h && timeinfo.tm_min == alarm_m && alarm_enable == 1)
    {
        if (alarm_flag == 0)
            alarming();
        alarm_flag = 1;
    }
    else
    {
        alarm_flag = 0;
    }

    //Check Button Status in 10s
    int runtime = millis();
    while (millis() - runtime < 10000)
    {
        int key_value = -1;
        if ((key_value = get_key_value()) != -1)
        {
            if (key_value == B_VALUE_B)
            {
                button_flag = 1;
                page = ++page % page_num;
                page_line = 0;
                page_add = 0;

                break;
            }
            else if (key_value == B_VALUE_A)
            {
                button_flag = 1;
                page_line++;
                break;
            }
            else if (key_value == B_VALUE_MENU)
            {
                button_flag = 1;
                page_add++;
                break;
            }
            else if (key_value == B_VALUE_R)
            {
                button_flag = 1;
                mp3_mod_flag = 1;
                break;
            }
        }
        delay(100);
    }
}

void clock_page()
{
    //If minute wasn't change,don't flesh screen
    if (last_h == timeinfo.tm_hour && last_m == timeinfo.tm_min && button_flag == 0)
        return;
    else
    {
        button_flag = 0;
        last_h = timeinfo.tm_hour;
        last_m = timeinfo.tm_min;
    }
    tft.fillScreen(ILI9341_BLACK);

    String date_str = (String)(timeinfo.tm_year + 1900) + "/" + (String)(timeinfo.tm_mon + 1) + "/" + (String)(timeinfo.tm_mday);

    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.print(date_str);

    //Check weather alarm enable set
    if (page_add == 1)
    {
        page_add = 0;
        alarm_enable = (alarm_enable + 1) % 2;
    }

    //Check alarm enable display
    if (alarm_enable)
    {
        tft.fillRect(240, 220, 8, 8, ILI9341_YELLOW);
    }
    else
    {
        tft.fillRect(240, 220, 8, 8, ILI9341_BLACK);
    }

    //Config display
    tft.setCursor(260, 220);
    String alarm_str = "";
    alarm_h < 10 ? alarm_str += "0" : alarm_str += "";
    alarm_str += (String)alarm_h + ":";
    alarm_m < 10 ? alarm_str += "0" : alarm_str += "";
    alarm_str += (String)alarm_m;
    tft.print(alarm_str);

    //Clock display
    tft.setCursor(80, 90);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setTextSize(6);

    char time_temp[20];
    sprintf(time_temp, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    tft.print(time_temp);
}

void alarm_page()
{
    tft.fillScreen(ILI9341_BLACK);

    tft.setCursor(10, 10);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(4);
    tft.print("ALARM SET");

    //Check button action, set alarm time.
    if (page_line % 2 == 0)
    {
        tft.fillCircle(65, 70, 5, ILI9341_WHITE);
        if (page_add != 0)
        {
            page_add = 0;
            alarm_h = ++alarm_h % 24;
            record_alarm(alarm_h, alarm_m);
        }
    }
    else
    {
        tft.fillCircle(65, 130, 5, ILI9341_WHITE);
        if (page_add != 0)
        {
            page_add = 0;
            alarm_m = ++alarm_m % 60;
            record_alarm(alarm_h, alarm_m);
        }
    }

    tft.setCursor(80, 60);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setTextSize(6);
    if (alarm_h < 10)
        tft.print("0");
    tft.print(alarm_h);
    tft.setCursor(80, 120);
    if (alarm_m < 10)
        tft.print("0");
    tft.print(alarm_m);
}

void timezone_page()
{
    tft.fillScreen(ILI9341_BLACK);

    tft.setCursor(10, 10);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(4);
    tft.print("TIMEZONE");

    tft.setCursor(10, 60);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.print("Effective after rst");
    if (page_add != 0)
    {
        page_add = 0;
        timezone = ++timezone;
        if (timezone > 12)
            timezone = -11;
        record_timezone(timezone);
    }

    tft.setCursor(80, 90);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setTextSize(6);
    if (timezone >= 0)
        tft.print("+");
    tft.print(timezone);
}

void alarming()
{
    int flag = 0;
    int runtime = millis();
    open_new_song("/yueliang.wav");
    audio.setVolume(21);

    while (1)
    {

        if (millis() - runtime > 1000)
        {
            runtime = millis();
            if (flag % 2 == 0)
            {
                tft.fillRect(0, 0, 320, 40, ILI9341_RED);
                tft.fillRect(0, 200, 320, 40, ILI9341_YELLOW);
            }
            else
            {
                tft.fillRect(0, 0, 320, 40, ILI9341_YELLOW);
                tft.fillRect(0, 200, 320, 40, ILI9341_RED);
            }
            flag++;
        }
        if (get_key_value() != -1)
        {
            break;
        }
        //delay(10);
         audio.loop();

    }
    alarm_enable = 0;
    open_new_song(file_list[file_index]);
}

void record_alarm(int16_t hour, int16_t minute)
{

    // 初始化NVS，并检查初始化情况
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // 如果NVS分区被占用则对其进行擦除
        // 并再次初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open 打开NVS文件
    printf("\n");
    printf("Opening Non-Volatile Config (NVS) handle... ");
    nvs_handle my_handle;                                // 定义不透明句柄
    err = nvs_open("Config", NVS_READWRITE, &my_handle); // 打开文件
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");

        // Write
        printf("Updating alarm_h in NVS ... ");
        err = nvs_set_i16(my_handle, "alarm_h", hour);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        printf("Updating alarm_m in NVS ... ");
        err = nvs_set_i16(my_handle, "alarm_m", minute);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(my_handle);
    }

    printf("\n");
}

void record_timezone(int16_t timezone)
{

    // 初始化NVS，并检查初始化情况
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // 如果NVS分区被占用则对其进行擦除
        // 并再次初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open 打开NVS文件
    printf("\n");
    printf("Opening Non-Volatile Config (NVS) handle... ");
    nvs_handle my_handle;                                // 定义不透明句柄
    err = nvs_open("Config", NVS_READWRITE, &my_handle); // 打开文件
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");

        // Write
        printf("Updating alarm_h in NVS ... ");
        err = nvs_set_i16(my_handle, "timezone", timezone);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(my_handle);
    }

    printf("\n");
}

void read_congfig(int16_t *hour, int16_t *minute, int16_t *timezone)
{
    // 初始化NVS，并检查初始化情况
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // 如果NVS分区被占用则对其进行擦除
        // 并再次初始化
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open 打开NVS文件
    printf("\n");
    printf("Opening Non-Volatile Config (NVS) handle... ");
    nvs_handle my_handle;                                // 定义不透明句柄
    err = nvs_open("Config", NVS_READWRITE, &my_handle); // 打开文件
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");

        // Read
        printf("Reading Config from NVS ... \n");

        //Read alarm_h
        err = nvs_get_i16(my_handle, "alarm_h", hour);
        switch (err)
        {
        case ESP_OK:
            printf("Done\n");
            printf("alarm_h: %d\n", *hour);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default:
            printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        //Read alarm_m
        err = nvs_get_i16(my_handle, "alarm_m", minute);
        switch (err)
        {
        case ESP_OK:
            printf("Done\n");
            printf("alarm_m: %d\n", *minute);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default:
            printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        //Read timezone
        err = nvs_get_i16(my_handle, "timezone", timezone);
        switch (err)
        {
        case ESP_OK:
            printf("Done\n");
            printf("timezone: %d\n", *timezone);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default:
            printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        // Close
        nvs_close(my_handle);
    }

    printf("\n");
    return;
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels)
            {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

int print_img(fs::FS &fs, String filename)
{
    File f = fs.open(filename);
    if (!f)
    {
        Serial.println("Failed to open file for reading");
        return 0;
    }

    f.seek(54);
    int X = 320;
    int Y = 240;
    uint8_t RGB[3 * X];
    for (int row = 0; row < Y; row++)
    {
        f.seek(54 + 3 * X * row);
        f.read(RGB, 3 * X);
        for (int col = 0; col < X; col++)
        {
            tft.drawPixel(col, row, tft.color565(RGB[col * 3 + 2], RGB[col * 3 + 1], RGB[col * 3]));
        }
    }

    f.close();

    return 0;
}

//****************MP3 MOD
void mp3_mod()
{
    if (button_flag != 0 || music_over_flag == 1)
    {
        button_flag = 0;
        music_over_flag = 0;
        tft.fillScreen(ILI9341_BLACK);
        tft.setTextColor(ILI9341_WHITE);

        //Last song
        if (file_num > 1)
        {
            int temp_index = file_index;
            if (temp_index > 0)
                temp_index--;
            else
                temp_index = file_num - 1;

            tft.setTextSize(2);
            tft.setCursor(10, 80);
            tft.print(cut_music_name(file_list[temp_index]));

            //Next song
            temp_index = file_index;
            if (temp_index < file_num - 1)
                temp_index++;
            else
                temp_index = 0;

            tft.setTextSize(2);
            tft.setCursor(10, 160);
            tft.print(cut_music_name(file_list[temp_index]));
        }

        //Now song
        tft.setTextSize(4);
        tft.setCursor(10, 100);
        tft.print(cut_music_name(file_list[file_index]));

        tft.setTextSize(1);
        tft.setCursor(250, 220);
        tft.print((String) "Vol:" + mp3_vol);
    }

    //Song exist time
    if ((mp3_runtime - millis()) > 1000)
    {
        fresh_song_time();
        char temp[30];
        sprintf(temp, "%2d:%2d / %2d:%d", music_info.runtime / 60, music_info.runtime % 60, music_info.length / 60, music_info.length % 60);
        tft.fillRect(10, 140, 200, 20, ILI9341_BLACK);
        tft.setCursor(10, 140);
        tft.setTextSize(2);
        tft.print(temp);
        mp3_runtime = millis();
    }

    audio.loop();

    if (mp3_key_runtime - millis() > 500)
    {
        int key_value = -1;
        if ((key_value = get_key_value()) != -1)
        {
            if (key_value == B_VALUE_R)
            {
                button_flag = 1;
                mp3_mod_flag = 0;
                delay(100);
            }
            else if (key_value == B_VALUE_SELECT)
            {
                button_flag = 1;
                if (mp3_vol > 0)
                    mp3_vol--;

                audio.setVolume(mp3_vol);
            }
            else if (key_value == B_VALUE_START)
            {
                button_flag = 1;
                if (mp3_vol < 21)
                    mp3_vol++;
                audio.setVolume(mp3_vol);
            }
            else if (key_value == B_VALUE_UP)
            {
                button_flag = 1;
                if (file_index > 0)
                    file_index--;
                else
                    file_index = file_num - 1;
                open_new_song(file_list[file_index]);
            }
            else if (key_value == B_VALUE_DOWN)
            {
                button_flag = 1;
                if (file_index < file_num - 1)
                    file_index++;
                else
                    file_index = 0;
                open_new_song(file_list[file_index]);
            }
        }

        mp3_key_runtime = millis();
    }
}

int get_music_list(fs::FS &fs, const char *dirname, uint8_t levels, String wavlist[30])
{
    Serial.printf("Listing directory: %s\n", dirname);
    int i = 0;

    File root = fs.open(dirname);
    if (!root)
    {
        Serial.println("Failed to open directory");
        return i;
    }
    if (!root.isDirectory())
    {
        Serial.println("Not a directory");
        return i;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
        }
        else
        {
            String temp = file.name();
            if (temp.endsWith(".wav"))
            {
                wavlist[i] = temp;
                i++;
            }
            else if (temp.endsWith(".mp3"))
            {
                wavlist[i] = temp;
                i++;
            }
        }
        file = root.openNextFile();
    }
    return i;
}

//歌曲结束逻辑
void audio_eof_mp3(const char *info)
{ //end of file
    Serial.print("eof_mp3     ");
    Serial.println(info);
    file_index++;
    if (file_index >= file_num)
    {
        file_index = 0;
    }
    open_new_song(file_list[file_index]);
    music_over_flag = 1;
}

void open_new_song(String filename)
{
    //去掉文件名的根目录"/"和文件后缀".mp3",".wav"
    //music_info.name = filename.substring(1, filename.indexOf("."));
    music_info.name = cut_music_name(filename);
    audio.connecttoFS(SD_MMC, filename);
    music_info.runtime = audio.getAudioCurrentTime();
    music_info.length = audio.getAudioFileDuration();
    music_info.volume = audio.getVolume();
    music_info.status = 1;
    Serial.println("**********start a new sound************");
}

//刷新歌曲时间
void fresh_song_time()
{
    music_info.runtime = audio.getAudioCurrentTime();
    music_info.length = audio.getAudioFileDuration();
}

String cut_music_name(String filename)
{
    return filename.substring(1, filename.indexOf("."));
}
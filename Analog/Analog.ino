#include <wifiboy32.h>
#include <Arduino.h>
#include <DHTesp.h>
#include "wb32_sprite.h"

/** Initialize DHT sensor */
DHTesp dht;
/** Pin number for DHT11 data pin */
int dhtPin = 0;

void blit_num256(uint16_t num, uint16_t x, uint16_t y, uint8_t color_mode)
{
    uint16_t d[5];

    d[0] = num / 10000;
    d[1] = (num - d[0] * 10000) / 1000;
    d[2] = (num - d[0] * 10000 - d[1] * 1000) / 100;
    d[3] = (num - d[0] * 10000 - d[1] * 1000 - d[2] * 100) / 10;
    d[4] = num - d[0] * 10000 - d[1] * 1000 - d[2] * 100 - d[3] * 10;

    for (int i = 0; i < 5; i++)
    {
        wb32_blitBuf8(d[i] * 8 + 120, color_mode * 8, 240, x + i * 8, y, 8, 8, (uint8_t *)sprites); //將d[0]~d[4]逐個顯示並排列
    }
}
void setup()
{
    dht.setup(dhtPin, DHTesp::DHT11);
    wb32_init();
    wb32_initBuf8();
    for (int i = 0; i < 256; i++)
        wb32_setPal8(i, wb32_color565(sprite_pal[i][0], sprite_pal[i][1], sprite_pal[i][2]));

    pinMode(dhtPin,INPUT);
    pinMode(23,OUTPUT);
}

void loop()
{
    wb32_clearBuf8();
    digitalWrite(23,LOW);
    TempAndHumidity lastValues = dht.getTempAndHumidity();

    blit_num256((int) lastValues.temperature,100,100,1);
    blit_num256((int) lastValues.humidity,100,120,1);
    wb32_blit8();
}
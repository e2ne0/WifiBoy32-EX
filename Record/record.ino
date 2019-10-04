#include <Arduino.h>
#include <wifiboy32.h>
#include "wb32_sprite.h"

void setup()
{
    pinMode(22, OUTPUT);
    pinMode(23, OUTPUT);
    pinMode(35, INPUT);
    pinMode(34, INPUT);
    digitalWrite(22, HIGH);
    digitalWrite(23, HIGH);
    for (int i = 0; i < 256; i++)
        wb32_setPal8(i, wb32_color565(sprite_pal[i][0], sprite_pal[i][1], sprite_pal[i][2]));
    
    wb32_init();
    wb32_initBuf8();
}

void loop()
{
    wb32_clearBuf8();
    if (digitalRead(35) == 0)
    {
        digitalWrite(23, 0);
    }
    if (digitalRead(35) == 1)
    {
        digitalWrite(23, HIGH);
    }

    if (digitalRead(34) == 0)
    {
        digitalWrite(22, HIGH);
    }
    if (digitalRead(34) == 1)
    {
        digitalWrite(22, 0);
    }
    wb32_blitBuf8(0, 0, 240,0,0,240,320, (uint8_t *)spritesTest);
    wb32_blit8();
}
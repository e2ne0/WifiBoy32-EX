#include <wifiboy32.h>
#include <Arduino.h>
#include "wb32_sprite.h"

uint8_t starsX[100];
uint16_t starsY[100];
uint8_t starsSpeed[100];
uint16_t starsColor = 254;

void setup()
{
    wb32_init();
    wb32_initBuf8();
    for (int i = 0; i < 256; i++)
        wb32_setPal8(i, wb32_color565(sprite_pal[i][0], sprite_pal[i][1], sprite_pal[i][2]));

    wb32_fillScreen(wbBLACK);
    for (int i = 0; i < 100; i++)
    {
        starsX[i] = random(0, 240);
        starsY[i] = random(0, 320);
        starsSpeed[i] = random(2, 5);
    }
}

void loop()
{
    wb32_clearBuf8();

    for (int i = 0; i < 100; i++)
    {
        wb32_setBuf8(starsX[i] + starsY[i] * 240, starsColor);
        starsY[i] += starsSpeed[i];
        if (starsY[i] >= 320)
            starsY[i] -= 320;
    }

    wb32_blit8();
}
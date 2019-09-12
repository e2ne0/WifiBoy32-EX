#include <wifiboy32.h>
#include <Arduino.h>
#include "wb32_sprite.h"

void setup()
{
    wb32_init();
    wb32_initBuf8();
    for (int i = 0; i < 256; i++)
        wb32_setPal8(i, wb32_color565(sprite_pal[i][0], sprite_pal[i][1], sprite_pal[i][2]));
}

void loop()
{

}
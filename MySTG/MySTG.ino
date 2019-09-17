#include <wifiboy32.h>
#include <Arduino.h>
#include "wb32_sprite.h"

uint8_t starsX[100];            //宣告陣列星星位置X
uint16_t starsY[100];           //宣告陣列星星位置Y
uint8_t starsSpeed[100];        //宣告陣列星星垂直速度
uint16_t starsColor = 65535;    //宣告星星顏色並設為白色
const uint8_t buttonLeft = 17;  //宣告向左移動的按鍵腳位為常量
const uint8_t buttonRight = 32; //宣告向右移動的按鍵腳位為常量
int masterX = 112;              //宣告主角位置X

void setup()
{
    wb32_init();
    wb32_initBuf8();
    for (int i = 0; i < 256; i++)
        wb32_setPal8(i, wb32_color565(sprite_pal[i][0], sprite_pal[i][1], sprite_pal[i][2]));

    wb32_fillScreen(wbBLACK); //設定背景(黑色)

    for (int i = 0; i < 100; i++) //宣告區域變數i = 0;當i < 100時執行;執行完畢i+1
    {
        starsX[i] = random(0, 240); //於第i個星星位置X隨機一個整數(0~240)
        starsY[i] = random(0, 320);
        starsSpeed[i] = random(2, 5);
    }

    pinMode(buttonLeft, INPUT);
    pinMode(buttonRight, INPUT);
}

void loop()
{
    wb32_clearBuf8();

    for (int i = 0; i < 100; i++)
    {
        wb32_setBuf8(starsX[i] + starsY[i] * 240, starsColor); //(於buffer設定顯示的位置(0~76799),顏色)
        starsY[i] += starsSpeed[i];                            //第i個星星位置Y以第i個星星速度增加
        if (starsY[i] >= 320)                                  //當第i個星星位置Y超過320
            starsY[i] = 0;                                     //回到0
    }

    wb32_blitBuf8(4, 75, 240, masterX, 280, 16, 21, (uint8_t *)sprites); //將主角顯示出來

    if (digitalRead(buttonLeft) == 0) //按下IO17時
        masterX -= 3;                 //主角左移3像素
    if (masterX < 0)
        masterX = 0;

    if (digitalRead(buttonRight) == 0)
        masterX += 3; //主角右移3像素
    if (masterX > 224)
        masterX = 224;

    wb32_blit8();
}
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
uint8_t masterState = 0;        //宣告主角狀態
int enemyX[10];                 //宣告敵方位置X
int enemyY[10];                 //宣告敵方位置X
unsigned long currentTime;      //宣告現在時間的變數
unsigned long enemySpawnCD;     //宣告敵人生成時間的變數
bool enemyAlive[10];            //宣告控制敵機存活的陣列
unsigned int enemyNo = 0;       //宣告為敵機編號的變數

void setup()
{
    currentTime = millis(); //現在時間為經過時間

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
        if (i < 10) //加到10前執行以下
        {
            enemyX[i] = random(0, 229); //初始化敵機的X
            enemyY[i] = -13;            //初始化敵機的Y
        }
    }
    //pinMode(35, INPUT);
    pinMode(buttonLeft, INPUT);
    pinMode(buttonRight, INPUT);

    enemySpawnCD = currentTime + 1000; //讓下台敵機出生CD為1秒後
    enemyAlive[0] = true;              //設置第一個敵機為存活
}

void loop()
{
    wb32_clearBuf8();

    currentTime = millis(); //當前時間每禎以毫秒更新

    for (int i = 0; i < 100; i++)
    {
        wb32_setBuf8(starsX[i] + starsY[i] * 240, starsColor); //(於buffer設定顯示的位置(0~76799),顏色)
        starsY[i] += starsSpeed[i];                            //第i個星星位置Y以第i個星星速度增加
        if (starsY[i] >= 320)                                  //當第i個星星位置Y超過320
            starsY[i] = 0;                                     //回到0
    }

    MasterCtrl();

    if (currentTime >= enemySpawnCD) //當現在時間到達敵機出生的CD時
    {
        enemyAlive[enemyNo] = true;                      //敵機設為存活 
        if (enemyNo < 10)
            enemyNo += 1; //敵機編號+1
        else
            enemyNo = 0; //敵機編號回到0

        enemySpawnCD = currentTime + random(1000, 1500); //設置下一個敵機出生的時間
    }
    for (int i; i < 10; i++)
    {
        if (enemyAlive[i]) //當本敵機存活時
        {
            enemyY[i] += 3; //設定向下飛的速度
            wb32_blitBuf8(3, 150, 240, enemyX[i], enemyY[i], 11, 11, (uint8_t *)sprites);
            if (enemyY[i] > 320) //當飛出畫面底部時
            {
                enemyX[i] = random(0, 229); //重新設置X位置
                enemyY[i] = -13;            //回到螢幕上方
                enemyAlive[i] = false;      //設定本敵機為未存活
            }
        }
        else //當本敵機未存活時
        {
            continue; //繼續執行
        }
    }

    wb32_blit8();
}

void MasterCtrl()
{
    switch (masterState)
    {
    case 0: //待機
        PlayerMovement();
        break;

    case 1: //移動
        PlayerMovement();
        break;

    case 2: //死亡

        break;
    }
}

void PlayerMovement()
{
    wb32_blitBuf8(4, 75, 240, masterX, 280, 16, 21, (uint8_t *)sprites); //將主角顯示出來
    if (digitalRead(buttonLeft) == 0)                                    //按下IO17時
    {
        masterX -= 8;    //主角左移8像素
        masterState = 1; //主角進入移動狀態
    }
    if (masterX < 0)
        masterX = 0;

    if (digitalRead(buttonRight) == 0)
    {
        masterX += 8; //主角右移8像素
        masterState = 1;
    }
    if (masterX > 224)
        masterX = 224;

    if (digitalRead(buttonLeft) == 1 && digitalRead(buttonRight) == 1) //兩個按鍵都鬆開時
        masterState = 0;                                               //主角進入待機狀態
}

/*void blit_num256(uint8_t num, uint16_t x, uint16_t y, uint8_t color_mode)
{
    uint8_t d[2];

    d[0] = num / 10;
    d[1] = num - d[1] * 10; //算出num的個位放入d[4]

    for (int i = 0; i < 2; i++)
    {
        wb32_blitBuf8(d[i] * 8 + 120, color_mode * 8, 240, x + i * 8, y, 8, 8, (uint8_t *)sprites); //將d[0]~d[4]逐個顯示並排列
    }
}*/

/*void bullet()
{
    switch (bulletState)
    {
    case 0:

        wb32_blitBuf8(11, 70, 240, masterX, bulletY, 2, 5, (uint8_t *)sprites);
        bulletX = masterX;
        bulletState++;
        break;

    case 1:
        bulletY -= 100;
        wb32_blitBuf8(11, 70, 240, bulletX, bulletY, 2, 5, (uint8_t *)sprites);
        if (bulletY < 100)
        {
            bulletState++;
        }
        break;

    case 2:
        bulletY = 275;
        shoot = false;
        return;
    }
}*/
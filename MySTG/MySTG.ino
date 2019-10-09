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
uint8_t masterStatus = 0;       //宣告主角狀態
int enemyX[10];                 //宣告敵方位置X
int enemyY[10];                 //宣告敵方位置Y
unsigned long currentTime;      //宣告現在時間的變數
unsigned long enemySpawnCD;     //宣告敵人生成時間的變數
bool enemyAlive[10];            //宣告控制敵機存活的陣列
unsigned int enemyNo = 0;       //宣告為敵機編號的變數
int bulletX[100];               //宣告子彈位置X
int bulletY[100];               //宣告子彈位置Y
unsigned long bulletSpawnCD;    //宣告子彈生成時間的變數
int bulletStatus[100];          //宣告子彈狀態
bool bulletAlive[100];          //宣告控制子彈存活的陣列
unsigned int bulletNo = 0;      //宣告為子彈編號的變數
int score = 0;                  //宣告分數變數
uint8_t life = 3;               //宣告生命
unsigned long soundStop;        //宣告控制音樂停止的變數
uint8_t sceneStatus;            //宣告控制場景的變數

void blit_str256(const char *str, int x, int y)
{
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] >= '@' && str[i] <= ']')
            wb32_blitBuf8(8 * (str[i] - '@'), 0, 240, x + i * 8, y, 8, 8, (uint8_t *)sprites);
        if (str[i] >= '!' && str[i] <= '>')
            wb32_blitBuf8(8 * (str[i] - '!'), 8, 240, x + i * 8, y, 8, 8, (uint8_t *)sprites);
        if (str[i] == '?')
            wb32_blitBuf8(8 * 14, 16, 240, x + i * 8, y, 8, 8, (uint8_t *)sprites);
        if (str[i] == 'c')
            wb32_blitBuf8(8 * 13, 16, 240, x + i * 8, y, 8, 8, (uint8_t *)sprites);
        if (str[i] == 'w')
            wb32_blitBuf8(7, 16, 240, x + i * 8, y, 26, 8, (uint8_t *)sprites);
        if (str[i] == 'x')
            wb32_blitBuf8(42, 16, 240, x + i * 8, y, 61, 8, (uint8_t *)sprites);
    }
}

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

void SetupSound()
{
    pinMode(25, OUTPUT);  //設定蜂鳴器的腳位為輸出
    ledcSetup(1, 400, 8); //ledcSetup(設定頻道1的頻率為400解析度為8位元)
    ledcAttachPin(25, 1); //ledcAttachPin(將腳位25定義為頻道1)
}

void MakeSound(int volume)
{
    ledcWrite(1, volume); //寫入音量
}

void SoundFreq(int freq)
{
    ledcSetup(1, freq, 8); //寫入音高
}

void setup()
{
    currentTime = millis(); //現在時間為經過時間

    wb32_init();
    wb32_initBuf8();
    SetupSound();
    for (int i = 0; i < 256; i++)
        wb32_setPal8(i, wb32_color565(sprite_pal2[i][0], sprite_pal2[i][1], sprite_pal2[i][2]));

    wb32_fillScreen(wbBLACK); //設定背景(黑色)

    for (int i = 0; i < 100; i++) //宣告區域變數i = 0;當i < 100時執行;執行完畢i+1
    {
        starsX[i] = random(0, 240); //於第i個星星位置X隨機一個整數(0~240)
        starsY[i] = random(0, 320);
        starsSpeed[i] = random(2, 5);
        bulletY[i] = 294; //初始化子彈的Y
        if (i < 10)       //加到10前執行以下
        {
            enemyX[i] = random(0, 229); //初始化敵機的X
            enemyY[i] = -13;            //初始化敵機的Y
        }
    }

    pinMode(buttonLeft, INPUT);
    pinMode(buttonRight, INPUT);

    // enemySpawnCD = currentTime + 1000; //讓下台敵機出生CD為1秒後
    // enemyAlive[0] = true;              //設置第一個敵機為存活

    // bulletSpawnCD = currentTime; //讓第一顆子彈馬上到出生時間
}

void loop()
{
    wb32_clearBuf8();
    currentTime = millis(); //當前時間在每禎以毫秒更新

    // blit_str256("SCORE", 0, 0);
    // blit_num256(score, 40, 0, 1);
    // for (int i = life; i > 0; i--)
    //     wb32_blitBuf8(4, 75, 240, 239 - i * 16, 0, 16, 21, (uint8_t *)sprites); //wb32_blitBuf8(int x:我們把最右側的生命圖示顯示出來後就能自動往左增加剩餘生命)

    // for (int i = 0; i < 100; i++)
    // {
    //     wb32_setBuf8(starsX[i] + starsY[i] * 240, starsColor); //(於buffer設定顯示的位置(0~76799),顏色)
    //     starsY[i] += starsSpeed[i];                            //第i個星星位置Y以第i個星星速度增加
    //     if (starsY[i] >= 320)                                  //當第i個星星位置Y超過320
    //         starsY[i] = 0;                                     //回到0
    // }

    // // if (life > 0)
    // // {
    // //     MasterCtrl();
    // //     EnemyCtrl();
    // //     Collision();
    // // }
    // SceneCtrl();
    if (currentTime > soundStop)
        MakeSound(0); //於停止時間把音量降為0
    wb32_blitBuf8(0, 0, 240,0,0,240,320, (uint8_t *)sprites2);
    wb32_blit8();
}

void MasterCtrl()
{
    switch (masterStatus)
    {
    case 0: //待機
        PlayerMovement();
        BulletCtrl();
        break;

    case 1: //移動
        PlayerMovement();
        BulletCtrl();
        break;

    case 2: //死亡

        break;
    }
}

void PlayerMovement()
{
    wb32_blitBuf8(4, 75, 240, masterX, 299, 16, 21, (uint8_t *)sprites); //將主角顯示出來
    if (digitalRead(buttonLeft) == 0)                                    //按下IO17時
    {
        masterX -= 8;     //主角左移8像素
        masterStatus = 1; //主角進入移動狀態
    }
    if (masterX < 0)
        masterX = 0;

    if (digitalRead(buttonRight) == 0)
    {
        masterX += 8; //主角右移8像素
        masterStatus = 1;
    }
    if (masterX > 224)
        masterX = 224;

    if (digitalRead(buttonLeft) == 1 && digitalRead(buttonRight) == 1) //兩個按鍵都鬆開時
        masterStatus = 0;                                              //主角進入待機狀態
}

void EnemyCtrl()
{
    if (currentTime >= enemySpawnCD) //當現在時間到達敵機出生的CD時
    {
        enemyAlive[enemyNo] = true; //敵機設為存活
        if (enemyNo < 9)
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
        else          //當本敵機未存活時
            continue; //繼續執行
    }
}

void BulletCtrl()
{
    if (currentTime >= bulletSpawnCD)
    {
        bulletAlive[bulletNo] = true;
        bulletStatus[bulletNo] = 0; //將子彈狀態歸0
        if (bulletNo < 99)
            bulletNo += 1;
        else
            bulletNo = 0;
        bulletSpawnCD = currentTime + 200;
    }
    for (int i = 0; i < 100; i++)
    {
        if (bulletAlive[i])
        {
            switch (bulletStatus[i])
            {
            case 0:                       //出生定位
                bulletX[i] = masterX + 6; //子彈會從主角的中心發射
                bulletStatus[i]++;
                break;

            case 1: //顯示與移動
                bulletY[i] -= 10;
                wb32_blitBuf8(11, 70, 240, bulletX[i], bulletY[i], 2, 5, (uint8_t *)sprites);
                if (bulletY[i] < 0)
                {
                    bulletStatus[i]++;
                }
                break;

            case 2: //消失
                bulletY[i] = 294;
                bulletAlive[i] = false;
                break;
            }
        }
        else
            continue;
    }
}

void Collision()
{
    for (int i = 0; i < 100; i++)
    {
        if (bulletAlive[i])
        {
            for (int j = 0; j < 10; j++)
            {
                if (enemyAlive[j])
                {
                    if (enemyX[j] <= bulletX[i] + 1 && enemyX[j] + 10 >= bulletX[i] && enemyY[j] + 10 >= bulletY[i])
                    {
                        score++;
                        enemyAlive[j] = false;
                        enemyX[j] = random(0, 229);
                        enemyY[j] = -13;
                        bulletStatus[i] = 2;
                        SoundFreq(400);                //設定音高為400
                        MakeSound(30);                 //設定音量為30
                        soundStop = currentTime + 100; //設定停止時間為0.1秒後
                    }
                    if (enemyY[j] >= 288 && enemyX[j] + 10 >= masterX && enemyX[j] <= masterX + 15)
                    {
                        life--;
                        enemyAlive[j] = false;
                        enemyX[j] = random(0, 229);
                        enemyY[j] = -13;
                        SoundFreq(200);
                        MakeSound(30);
                        soundStop = currentTime + 100;
                    }
                }
                else
                    continue;
            }
        }
        else
            continue;
    }
}

void SceneCtrl()
{
    switch (sceneStatus)
    {
    case 0: //遊戲開始
        if (digitalRead(buttonLeft) == 0 || digitalRead(buttonRight) == 0)
        {
            enemySpawnCD = currentTime + 1000;
            enemyAlive[0] = true;
            bulletSpawnCD = currentTime;
            sceneStatus++;
        }
        blit_str256("PRESS L OR R", 71, 155);
        break;

    case 1: //遊戲中
        blit_str256("SCORE", 0, 0);
        blit_num256(score, 40, 0, 1);
        currentTime = millis();
        MasterCtrl();
        EnemyCtrl();
        Collision();
        if (life <= 0)
        { //當life到達0時先將所有敵機與子彈從場上清除再到下一個場景
            enemyNo = 0;
            bulletNo = 0;
            for (int i = 0; i < 100; i++)
            {
                if (i < 10)
                {
                    enemyAlive[i] = false;
                    enemyY[i] = -13;
                }
                bulletStatus[i] = 2;
            }
            sceneStatus++;
            delay(500); //給一個小延遲產生結束的感受
        }
        break;

    case 2: //遊戲結束
        if (digitalRead(buttonLeft) == 0 || digitalRead(buttonRight) == 0)
        { //當回到遊戲場景時要將數值初始化
            life = 3;
            enemySpawnCD = currentTime + 1000;
            enemyAlive[0] = true;
            bulletSpawnCD = currentTime;
            sceneStatus = 1;
            score = 0;
        }
        MakeSound(0);
        blit_str256("PRESS L OR R", 71, 170);
        blit_str256("GAMEOVER", 87, 162);
        blit_str256("YOUR SCORE", 79, 200);
        blit_num256(score, 99, 208, 1);
        break;
    }
}
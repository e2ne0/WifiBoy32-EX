#include <wifiboy32.h>
#include <Arduino.h>
#include "wb32_sprite.h"
#include "stage.h"

int ball_x = 100, ball_y = 100;
const int ballSpeed = 8; //因為速度不會改變所以可加上const
int ballState = 0;
int playerBar = 100;
const int moveDistance = 8; //因為移動距離不會改變所以可加上const
const uint8_t button_L = 17, button_R = 32;
bool blockAlive[5][6] = //[有幾組][每組多少個數]
{ //我們以每關5行舉，每行放6個磚塊例
    {0,0,0,0,0,0},
    {1,1,1,1,1,1},
    {0,0,0,0,0,0},
    {1,1,1,1,1,1},
    {0,0,0,0,0,0},
}; // 宣告一組bool二維陣列判斷關卡磚塊是否存活

void setup()
{
    wb32_init();
    wb32_initBuf8();
    for (int i = 0; i < 256; i++)
        wb32_setPal8(i, wb32_color565(sprite_pal[i][0], sprite_pal[i][1], sprite_pal[i][2]));

    pinMode(button_L, INPUT);
    pinMode(button_R, INPUT);
}

void loop()
{
    wb32_clearBuf8();

    BallMovement();
    PlayerMovement();
    BlockCtrl();
    wb32_blit8();
}

void BallMovement()
{
    switch (ballState)
    {
    case 0: //右下移動
        ball_x += ballSpeed;
        ball_y += ballSpeed;
        if (ball_x >= 230)
            ballState = 1;
        if (ball_y >= 310)
            ballState = 2;
        break;

    case 1: //左下移動
        ball_x -= ballSpeed;
        ball_y += ballSpeed;
        if (ball_x <= 0)
            ballState = 0;
        if (ball_y >= 310)
            ballState = 3;
        break;

    case 2: //右上移動
        ball_x += ballSpeed;
        ball_y -= ballSpeed;
        if (ball_x >= 230)
            ballState = 3;
        if (ball_y <= 0)
            ballState = 0;
        break;

    case 3: //左上移動
        ball_x -= ballSpeed;
        ball_y -= ballSpeed;
        if (ball_x <= 0)
            ballState = 2;
        if (ball_y <= 0)
            ballState = 1;
        break;
    }
    wb32_blitBuf8(192, 192, 240, ball_x, ball_y, 10, 10, (uint8_t *)sprites);
}

void PlayerMovement()
{
    if (digitalRead(button_L) == 0)
        playerBar -= moveDistance; //下方球拍左移
    if (playerBar < 0)
        playerBar = 0;

    if (digitalRead(button_R) == 0)
        playerBar += moveDistance; //下方球拍右移
    if (playerBar > 200)
        playerBar = 200;

    wb32_blitBuf8(68, 216, 240, playerBar, 310, 40, 8, (uint8_t *)sprites);
}

void BlockCtrl()
{
    for(int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            if(blockAlive[i][j])
                wb32_blitBuf8(192, 216, 240, j * 40, i * 6, 39, 5, (uint8_t *)sprites);
        }
    }
}

void StageReader(uint8_t stage)
{
    for (int i = (stage - 1) * 5; i < stage * 5; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            // if (stageData[i][j] != 0)
            //     wb32_blitBuf8(192, 216, 240, j * 40, 6 * (i - (stage - 1) * 5 ), 39, 5, (uint8_t *)sprites);
            // else
            // {
            //     continue;
            // }
            blockAlive[i - (stage - 1) * 5][j] = stageData [i][j];
        }
    }
}



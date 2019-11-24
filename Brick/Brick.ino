#include <wifiboy32.h>
#include <Arduino.h>
#include "wb32_sprite.h"

int ball_x, ball_y;
int ballSpeed;
int ballState;

void setup()
{
    wb32_init();
    wb32_initBuf8();
    for (int i = 0; i < 256; i++)
        wb32_setPal8(i, wb32_color565(sprite_pal[i][0], sprite_pal[i][1], sprite_pal[i][2]));
    ball_x = ball_y = 100;
    ballSpeed = 8;
}

void loop()
{
    wb32_clearBuf8();

    BallMovement();

    wb32_blitBuf8(192, 192, 240, ball_x, ball_y, 10, 10, (uint8_t *)sprites);
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
}

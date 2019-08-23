#include <wifiboy32.h> //全域資料用<>
#include <Arduino.h>
#include "wb32_sprite.h" //本地資料用""

int ball_x, ball_y;
int ballSpeed;
int ballState;
int upperBar = 100, lowerBar = 100;
int moveDistance = 8;
int score;

void blit_str256(const char *str, int x, int y)
{
    for (int i = 0; i < strlen(str); i++) //for(宣告一個int i, i < strlen偵測文字長度(str我們輸入的文字), 達成前一項條件執行下方程式結束後i+1)
    {
        if (str[i] >= '@' && str[i] <= ']')                                                    //根據ASCII以十進位來說 當文字的ASCII>=64並<=93時
            wb32_blitBuf8(8 * (str[i] - '@'), 0, 240, x + i * 8, y, 8, 8, (uint8_t *)sprites); //擷取這邊的圖像要*8是因為每個文字大小為8*8所以每隔一個文字就要+8以A為例他在sprites的x就是8*(65-64)=1 他在第一行所以y=0
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
    uint16_t d[5]; //宣告一個型別為uint16_t的變數陣列d

    d[0] = num / 10000;                                               //算出num的萬位放入d[0]
    d[1] = (num - d[0] * 10000) / 1000;                               //算出num的千位放入d[1]
    d[2] = (num - d[0] * 10000 - d[1] * 1000) / 100;                  //算出num的百位放入d[2]
    d[3] = (num - d[0] * 10000 - d[1] * 1000 - d[2] * 100) / 10;      //算出num的十位放入d[3]
    d[4] = num - d[0] * 10000 - d[1] * 1000 - d[2] * 100 - d[3] * 10; //算出num的個位放入d[4]

    for (int i = 0; i < 5; i++)
    {
        wb32_blitBuf8(d[i] * 8 + 120, color_mode * 8, 240, x + i * 8, y, 8, 8, (uint8_t *)sprites); //將d[0]~d[4]逐個顯示並排列
    }
}

void setup()
{
    wb32_init();                  // 載入WifiBoy32的引擎
    wb32_initBuf8();              // 準備 75KB 的buffer
    for (int i = 0; i < 256; i++) // 於wb32_sprite.h中的sprite_pal中設定8bits色彩(sprites會用到)
        wb32_setPal8(i, wb32_color565(sprite_pal[i][0], sprite_pal[i][1], sprite_pal[i][2]));

    //下方玩家
    pinMode(17, INPUT); //宣告IO17為輸入
    pinMode(32, INPUT);
    //上方玩家
    pinMode(34, INPUT);
    pinMode(35, INPUT);

    ball_x = ball_y = 100; //使ball_x與ball_y於一開始先為100
    ballSpeed = 8;
    ballState = 0; //先將球的狀態設為往右下移動
}

void loop()
{
    wb32_clearBuf8(); //由於我們不會把之前的畫面留在buffer中，所以我們每一幀都要清空buffer

    wb32_blitBuf8(192, 216, 240, upperBar, 2, 40, 8, (uint8_t *)sprites);
    wb32_blitBuf8(68, 216, 240, lowerBar, 310, 40, 8, (uint8_t *)sprites);

    blit_str256("SCORE", 100, 151);  //剛剛加入顯示文字的功能(要顯示的文字 sprites中只有大寫所以只會顯示大寫, 螢幕位置x, 螢幕位置y)
    blit_num256(score, 100, 161, 1); //5位數的計數器(要顯示的值, 螢幕位置x, 螢幕位置y, 要顯示的顏色根據在sprites中的行數顯示需要的顏色)

    BallMovement(); //這是一個獨立的功能要放到loop或setup才會執行，由於我們需要持續執行所以我們放在loop
    PlayerMovement();

    wb32_blitBuf8(192, 192, 240, ball_x, ball_y, 10, 10, (uint8_t *)sprites); //從sprites的x192 y192開始擷取 在x=ball_x y=ball_y的位置生成一顆10*10的球
    wb32_blit8();
}

void BallMovement() //增加一個名為BallMovement的功能
{
    switch (ballState) //switch偵測ballState的值來切換狀態
    {
    case 0: //當ballState為0時觸發本事件，依此推類，右下移動
        ball_x += ballSpeed;
        ball_y += ballSpeed;
        if (ball_x >= 230) //當球到達右邊界需要向左下移動時
            ballState = 1; //將球的狀態變更為1
        if (ball_y >= 300) //球抵達球拍高度時
        {
            if ((ball_x + 10 >= lowerBar) && (ball_x <= lowerBar + 40)) //球如果在球排的範圍
            {
                ballState = 2; //本來往右下碰到後改為右上
                score++;       //增加分數
            }
            if (ball_y >= 320) //沒碰到球拍掉下去
            {
                score = 0;             //分數歸0
                ball_x = ball_y = 100; //球回到起始位置
                ballState = 0;
                delay(500); //延後0.5秒執行
            }
        }
        break; //結束本事件

    case 1: //左下移動
        ball_x -= ballSpeed;
        ball_y += ballSpeed;
        if (ball_x <= 0) //當球到達左邊界需要向右下移動時
            ballState = 0;
        if (ball_y >= 300)
        {
            if ((ball_x + 10 >= lowerBar) && (ball_x <= lowerBar + 40))
            {
                ballState = 3; //本來往左下碰到後改為左上
                score++;
            }
            if (ball_y >= 320)
            {
                score = 0;
                ball_x = ball_y = 100;
                ballState = 0;
                delay(500);
            }
        }
        break;

    case 2: //右上移動
        ball_x += ballSpeed;
        ball_y -= ballSpeed;
        if (ball_x >= 230) //當球到達右邊界需要向左上移動時
            ballState = 3;
        if (ball_y <= 10)
        {
            if ((ball_x + 10 >= upperBar) && (ball_x <= upperBar + 40))
            {
                ballState = 0; //本來往右上碰到後改為右下
                score++;
            }
            if (ball_y < 0)
            {
                score = 0;
                ball_x = ball_y = 100;
                ballState = 0;
                delay(500);
            }
        }
        break;

    case 3: //左上移動
        ball_x -= ballSpeed;
        ball_y -= ballSpeed;
        if (ball_x <= 0) //當球到達左邊界需要向右上移動時
            ballState = 2;
        if (ball_y <= 10)
        {
            if ((ball_x + 10 >= upperBar) && (ball_x <= upperBar + 40))
            {
                ballState = 1; //本來往左上碰到後改為左下
                score++;
            }
            if (ball_y < 0)
            {
                score = 0;
                ball_x = ball_y = 100;
                ballState = 0;
                delay(500);
            }
        }
        break;
    }
}

void PlayerMovement()
{
    if (digitalRead(17) == 0)     //按下IO17時
        lowerBar -= moveDistance; //下方球拍左移
    if (lowerBar < 0)
        lowerBar = 0;

    if (digitalRead(32) == 0)
        lowerBar += moveDistance; //下方球拍右移
    if (lowerBar > 200)
        lowerBar = 200;

    if (digitalRead(34) == 0)
        upperBar -= moveDistance; //上方球拍左移
    if (upperBar < 0)
        upperBar = 0;

    if (digitalRead(35) == 0)
        upperBar += moveDistance; //上方球拍右移
    if (upperBar > 200)
        upperBar = 200;
}
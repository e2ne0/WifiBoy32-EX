#include <Arduino.h>
#include <wifiboy32.h>

void setup()
{
    pinMode(22, OUTPUT);
    pinMode(23, OUTPUT);
    pinMode(35, INPUT);
    pinMode(34, INPUT);
    digitalWrite(22, HIGH);
    digitalWrite(23, HIGH);
}

void loop()
{
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
}
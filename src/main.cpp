#include <Arduino.h>
#include <M5GFX.h>

static M5GFX screen;

void setup()
{
    screen.init();
    screen.setRotation(3);
    screen.setBrightness(128);

    screen.fillScreen(TFT_BLACK);
    screen.setTextColor(TFT_WHITE, TFT_BLACK);
    screen.setTextSize(3);
    screen.setCursor(20, 40);
    screen.print("Hello!");
}

void loop()
{
    return;
}

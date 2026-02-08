#include <Arduino.h>
#include <M5GFX.h>
#include <Button.h>
#include <ScrollList.h>

static M5GFX screen;

static constexpr uint8_t kButtonUpPin = 35;     // Up
static constexpr uint8_t kButtonDownPin = 39;   // Down
// static constexpr uint8_t kButtonSelectPin = 37; // Select

static Button buttonUp(kButtonUpPin);
static Button buttonDown(kButtonDownPin);
// static Button buttonSelect(kButtonSelectPin);

static constexpr const char* kItems[] = {
    "First item",
    "Second item",
    "Third item",
    "Fourth item",
    "Fifth item",
    "Sixth item",
    "Seventh item",
    "Eighth item",
    "Ninth item",
    "Tenth item",
};

static constexpr size_t kItemCount = sizeof(kItems) / sizeof(kItems[0]);
static ScrollList list(screen, kItems, kItemCount);

void setup()
{
    screen.init();
    screen.setRotation(3);
    screen.setBrightness(128);

    buttonUp.begin();
    buttonDown.begin();
    // buttonSelect.begin();

    list.draw();
}

void loop()
{
    bool updated = false;

    if (buttonUp.pressed())
    {
        updated = list.moveUp(true);
    }

    if (buttonDown.pressed())
    {
        updated = list.moveDown(true) || updated;
    }

    if (updated)
    {
        list.draw();
    }

    delay(10);
}

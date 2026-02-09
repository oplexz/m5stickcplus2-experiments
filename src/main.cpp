#include <Arduino.h>
#include <M5GFX.h>
#include <Button.h>
#include <ScrollList.h>
#include <ValueEditor.h>

static M5GFX screen;

static constexpr uint8_t kButtonUpPin = 35;     // Up
static constexpr uint8_t kButtonDownPin = 39;   // Down
static constexpr uint8_t kButtonSelectPin = 37; // Select

static Button buttonUp(kButtonUpPin);
static Button buttonDown(kButtonDownPin);
static Button buttonSelect(kButtonSelectPin);

static constexpr const char* kItems[] = {
    "Brightness",
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
static ValueEditor valueEditor(screen);

static constexpr int kBrightnessItemIndex = 0;

enum class ScreenMode
{
    List,
    Editor,
};

static ScreenMode screenMode = ScreenMode::List;

static uint8_t percentToBrightness(int percent)
{
    if (percent < 0)
    {
        percent = 0;
    }
    if (percent > 100)
    {
        percent = 100;
    }
    return static_cast<uint8_t>((percent * 255) / 100);
}

void setup()
{
    screen.init();
    screen.setRotation(3);
    screen.setBrightness(128);

    buttonUp.begin();
    buttonDown.begin();
    buttonSelect.begin();

    valueEditor.setLabel("Brightness");
    valueEditor.setSuffix("%");
    valueEditor.setRange(0, 100);
    valueEditor.setStep(1);
    valueEditor.setValue(50);

    list.draw();
}

void loop()
{
    bool updated = false;

    if (screenMode == ScreenMode::List)
    {
        if (buttonUp.pressed())
        {
            updated = list.moveUp(true);
        }

        if (buttonDown.pressed())
        {
            updated = list.moveDown(true) || updated;
        }

        if (buttonSelect.pressed())
        {
            if (list.selectedIndex() == kBrightnessItemIndex)
            {
                screenMode = ScreenMode::Editor;
                valueEditor.draw();
            }
        }

        if (updated)
        {
            list.draw();
        }
    }
    else
    {
        if (buttonUp.pressed())
        {
            if (valueEditor.decrease())
            {
                screen.setBrightness(percentToBrightness(valueEditor.value()));
                valueEditor.draw();
            }
        }

        if (buttonDown.pressed())
        {
            if (valueEditor.increase())
            {
                screen.setBrightness(percentToBrightness(valueEditor.value()));
                valueEditor.draw();
            }
        }

        if (buttonSelect.pressed())
        {
            screenMode = ScreenMode::List;
            list.draw();
        }
    }

    delay(10);
}

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
static constexpr uint32_t kRepeatDelayMs = 500;
static constexpr uint32_t kRepeatIntervalMs = 33;

enum class ScreenMode
{
    List,
    Editor,
};

static ScreenMode screenMode = ScreenMode::List;
static uint32_t upPressStartMs = 0;
static uint32_t upLastRepeatMs = 0;
static uint32_t downPressStartMs = 0;
static uint32_t downLastRepeatMs = 0;

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
    valueEditor.setStep(5);
    valueEditor.setValue(50);

    list.draw();
}

void loop()
{
    bool updated = false;

    if (screenMode == ScreenMode::List)
    {
        buttonUp.read();
        buttonDown.read();
        buttonSelect.read();

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
        uint32_t now = millis();
        bool upState = (buttonUp.read() == Button::PRESSED);
        bool upChanged = buttonUp.has_changed();

        if (!upState)
        {
            upPressStartMs = 0;
            upLastRepeatMs = 0;
        }
        else if (upChanged)
        {
            if (valueEditor.decrease())
            {
                screen.setBrightness(percentToBrightness(valueEditor.value()));
                valueEditor.draw();
            }
            upPressStartMs = now;
            upLastRepeatMs = now;
        }
        else if (upPressStartMs != 0 &&
                 now - upPressStartMs >= kRepeatDelayMs &&
                 now - upLastRepeatMs >= kRepeatIntervalMs)
        {
            if (valueEditor.decrease())
            {
                screen.setBrightness(percentToBrightness(valueEditor.value()));
                valueEditor.draw();
            }
            upLastRepeatMs = now;
        }

        bool downState = (buttonDown.read() == Button::PRESSED);
        bool downChanged = buttonDown.has_changed();

        if (!downState)
        {
            downPressStartMs = 0;
            downLastRepeatMs = 0;
        }
        else if (downChanged)
        {
            if (valueEditor.increase())
            {
                screen.setBrightness(percentToBrightness(valueEditor.value()));
                valueEditor.draw();
            }
            downPressStartMs = now;
            downLastRepeatMs = now;
        }
        else if (downPressStartMs != 0 &&
                 now - downPressStartMs >= kRepeatDelayMs &&
                 now - downLastRepeatMs >= kRepeatIntervalMs)
        {
            if (valueEditor.increase())
            {
                screen.setBrightness(percentToBrightness(valueEditor.value()));
                valueEditor.draw();
            }
            downLastRepeatMs = now;
        }

        if (buttonSelect.pressed())
        {
            screenMode = ScreenMode::List;
            list.draw();
        }
    }

    delay(10);
}

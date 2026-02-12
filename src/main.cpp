#include <Arduino.h>
#include <M5GFX.h>
#include <Button.h>
#include <ScrollList.h>
#include <ValueEditor.h>
#include <IrBruteforce.h>
#include <IrCodeSender.h>
#include <IrRemote.h>
#include <IrRepeatSender.h>

static M5GFX screen;

static constexpr uint8_t kButtonUpPin = 35;     // Up
static constexpr uint8_t kButtonDownPin = 39;   // Down
static constexpr uint8_t kButtonSelectPin = 37; // Select

static Button buttonUp(kButtonUpPin);
static Button buttonDown(kButtonDownPin);
static Button buttonSelect(kButtonSelectPin);

static constexpr uint8_t kIrPin = 19; // M5StickC Plus2 IR LED

static constexpr IrCommand kLampCommands[] = {
    {"White/Yellow",  0x00, 0x18},
    {"Yellow>White",  0x00, 0x30},
    {"Colorful 1",    0x00, 0x38},
    {"Colorful 2",    0x00, 0x4A},
    {"Off",           0x00, 0x62},
};
static constexpr size_t kLampCommandCount = sizeof(kLampCommands) / sizeof(kLampCommands[0]);
static IrRemote lampRemote(screen, kIrPin, kLampCommands, kLampCommandCount);

static constexpr const char* kItems[] = {
    "Brightness",
    "Lamp Remote",
    "IR Bruteforce",
    "IR Send",
    "IR Repeat",
};

static constexpr size_t kItemCount = sizeof(kItems) / sizeof(kItems[0]);
static ScrollList list(screen, kItems, kItemCount);
static ValueEditor valueEditor(screen);

static constexpr int kBrightnessItemIndex = 0;
static constexpr int kLampRemoteItemIndex = 1;
static constexpr int kIrBruteforceItemIndex = 2;
static constexpr int kIrSendItemIndex = 3;
static constexpr int kIrRepeatItemIndex = 4;

static IrBruteforce irBruteforce(screen, kIrPin);
static IrCodeSender irCodeSender(screen, kIrPin);
static IrRepeatSender irRepeatSender(screen, kIrPin);
static constexpr uint32_t kRepeatDelayMs = 500;
static constexpr uint32_t kRepeatIntervalMs = 33;

static constexpr uint32_t kLongPressMs = 3000;

enum class ScreenMode
{
    List,
    Editor,
    IrBruteforce,
    IrSend,
    IrRepeat,
    LampRemote,
};

static ScreenMode screenMode = ScreenMode::List;
static uint32_t upPressStartMs = 0;
static uint32_t upLastRepeatMs = 0;
static uint32_t downPressStartMs = 0;
static uint32_t downLastRepeatMs = 0;
static uint32_t selectPressStartMs = 0;

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

    irCodeSender.begin();
    irRepeatSender.begin();
    lampRemote.begin();

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
            else if (list.selectedIndex() == kLampRemoteItemIndex)
            {
                screenMode = ScreenMode::LampRemote;
                selectPressStartMs = 0;
                lampRemote.draw();
            }
            else if (list.selectedIndex() == kIrBruteforceItemIndex)
            {
                screenMode = ScreenMode::IrBruteforce;
                irBruteforce.start();
            }
            else if (list.selectedIndex() == kIrSendItemIndex)
            {
                screenMode = ScreenMode::IrSend;
                selectPressStartMs = 0;
                irCodeSender.draw();
            }
            else if (list.selectedIndex() == kIrRepeatItemIndex)
            {
                screenMode = ScreenMode::IrRepeat;
                selectPressStartMs = 0;
                irRepeatSender.draw();
            }
        }

        if (updated)
        {
            list.draw();
        }
    }
    else if (screenMode == ScreenMode::Editor)
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
    else if (screenMode == ScreenMode::IrBruteforce)
    {
        buttonUp.read();
        buttonDown.read();

        irBruteforce.tick();

        if (buttonSelect.pressed())
        {
            irBruteforce.stop();
            screenMode = ScreenMode::List;
            list.draw();
        }
    }
    else if (screenMode == ScreenMode::IrSend)
    {
        uint32_t now = millis();

        // Up/down with hold-to-repeat
        bool upState = (buttonUp.read() == Button::PRESSED);
        bool upChanged = buttonUp.has_changed();

        if (!upState)
        {
            upPressStartMs = 0;
            upLastRepeatMs = 0;
        }
        else if (upChanged)
        {
            irCodeSender.prev();
            upPressStartMs = now;
            upLastRepeatMs = now;
        }
        else if (upPressStartMs != 0 &&
                 now - upPressStartMs >= kRepeatDelayMs &&
                 now - upLastRepeatMs >= kRepeatIntervalMs)
        {
            irCodeSender.prev();
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
            irCodeSender.next();
            downPressStartMs = now;
            downLastRepeatMs = now;
        }
        else if (downPressStartMs != 0 &&
                 now - downPressStartMs >= kRepeatDelayMs &&
                 now - downLastRepeatMs >= kRepeatIntervalMs)
        {
            irCodeSender.next();
            downLastRepeatMs = now;
        }

        // Select: short press = send, hold 3s = back to menu
        bool selectState = (buttonSelect.read() == Button::PRESSED);
        bool selectChanged = buttonSelect.has_changed();

        if (selectState && selectChanged)
        {
            // Just pressed down
            selectPressStartMs = now;
        }
        else if (!selectState && selectChanged && selectPressStartMs != 0)
        {
            // Released before long-press threshold -- send the code
            if (now - selectPressStartMs < kLongPressMs)
            {
                irCodeSender.send();
            }
            selectPressStartMs = 0;
        }
        else if (selectState && selectPressStartMs != 0 &&
                 now - selectPressStartMs >= kLongPressMs)
        {
            // Held long enough -- go back
            selectPressStartMs = 0;
            screenMode = ScreenMode::List;
            list.draw();
        }
    }

    else if (screenMode == ScreenMode::IrRepeat)
    {
        uint32_t now = millis();

        // Up/down with hold-to-repeat for navigation
        bool upState = (buttonUp.read() == Button::PRESSED);
        bool upChanged = buttonUp.has_changed();

        if (!upState)
        {
            upPressStartMs = 0;
            upLastRepeatMs = 0;
        }
        else if (upChanged)
        {
            irRepeatSender.prev();
            upPressStartMs = now;
            upLastRepeatMs = now;
        }
        else if (upPressStartMs != 0 &&
                 now - upPressStartMs >= kRepeatDelayMs &&
                 now - upLastRepeatMs >= kRepeatIntervalMs)
        {
            irRepeatSender.prev();
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
            irRepeatSender.next();
            downPressStartMs = now;
            downLastRepeatMs = now;
        }
        else if (downPressStartMs != 0 &&
                 now - downPressStartMs >= kRepeatDelayMs &&
                 now - downLastRepeatMs >= kRepeatIntervalMs)
        {
            irRepeatSender.next();
            downLastRepeatMs = now;
        }

        // Auto-send tick
        irRepeatSender.tick();

        // Select: short press = toggle sending, long hold = back
        bool selectState = (buttonSelect.read() == Button::PRESSED);
        bool selectChanged = buttonSelect.has_changed();

        if (selectState && selectChanged)
        {
            selectPressStartMs = now;
        }
        else if (!selectState && selectChanged && selectPressStartMs != 0)
        {
            if (now - selectPressStartMs < kLongPressMs)
            {
                // Toggle sending on/off
                if (irRepeatSender.isSending())
                {
                    irRepeatSender.stopSending();
                }
                else
                {
                    irRepeatSender.startSending();
                }
            }
            selectPressStartMs = 0;
        }
        else if (selectState && selectPressStartMs != 0 &&
                 now - selectPressStartMs >= kLongPressMs)
        {
            irRepeatSender.stopSending();
            selectPressStartMs = 0;
            screenMode = ScreenMode::List;
            list.draw();
        }
    }
    else if (screenMode == ScreenMode::LampRemote)
    {
        uint32_t now = millis();

        buttonUp.read();
        bool upChanged = buttonUp.has_changed();
        if (buttonUp.read() == Button::PRESSED && upChanged)
        {
            if (lampRemote.moveUp(true))
            {
                lampRemote.draw();
            }
        }

        buttonDown.read();
        bool downChanged = buttonDown.has_changed();
        if (buttonDown.read() == Button::PRESSED && downChanged)
        {
            if (lampRemote.moveDown(true))
            {
                lampRemote.draw();
            }
        }

        // Select: short press = send, hold 3s = back to menu
        bool selectState = (buttonSelect.read() == Button::PRESSED);
        bool selectChanged = buttonSelect.has_changed();

        if (selectState && selectChanged)
        {
            selectPressStartMs = now;
        }
        else if (!selectState && selectChanged && selectPressStartMs != 0)
        {
            if (now - selectPressStartMs < kLongPressMs)
            {
                lampRemote.sendSelected();
            }
            selectPressStartMs = 0;
        }
        else if (selectState && selectPressStartMs != 0 &&
                 now - selectPressStartMs >= kLongPressMs)
        {
            selectPressStartMs = 0;
            screenMode = ScreenMode::List;
            list.draw();
        }
    }

    delay(10);
}

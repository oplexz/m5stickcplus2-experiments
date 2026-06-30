#include <Arduino.h>
#include <M5GFX.h>
#include <Button.h>
#include <ScrollList.h>
#include <ValueEditor.h>
#include <IrBruteforce.h>
#include <IrCodeSender.h>
#include <IrRemote.h>
#include <IrRepeatSender.h>
#include <BleKeyboard.h>

#include "screens/ScreenManager.h"
#include "screens/ListScreen.h"
#include "screens/IrMenuScreen.h"
#include "screens/BrightnessScreen.h"
#include "screens/IrBruteforceScreen.h"
#include "screens/IrSendScreen.h"
#include "screens/IrRepeatScreen.h"
#include "screens/LampRemoteScreen.h"
#include "screens/BleKeyboardScreen.h"

static M5GFX screen;

static constexpr uint8_t kButtonUpPin     = 35;
static constexpr uint8_t kButtonDownPin   = 39;
static constexpr uint8_t kButtonSelectPin = 37;
static constexpr uint8_t kIrPin           = 19;

static constexpr uint32_t kRepeatDelayMs    = 500;
static constexpr uint32_t kRepeatIntervalMs = 33;
static constexpr uint32_t kLongPressMs      = 600;

static Button buttonUp(kButtonUpPin);
static Button buttonDown(kButtonDownPin);
static Button buttonSelect(kButtonSelectPin);

static constexpr IrCommand kLampCommands[] = {
    {"White/Yellow", 0x00, 0x18},
    {"Yellow>White", 0x00, 0x30},
    {"Colorful 1",   0x00, 0x38},
    {"Colorful 2",   0x00, 0x4A},
    {"Off",          0x00, 0x62},
};
static constexpr size_t kLampCommandCount =
    sizeof(kLampCommands) / sizeof(kLampCommands[0]);

static IrRemote lampRemote(screen, kIrPin, kLampCommands, kLampCommandCount);

static constexpr const char* kMainItems[] = {"Brightness", "IR Tools", "BLE Keyboard"};
static ScrollList mainList(screen, kMainItems, 3);

static constexpr const char* kIrItems[] = {
    "Lamp Remote", "IR Bruteforce", "IR Send", "IR Repeat"
};
static ScrollList irList(screen, kIrItems, 4);

static ValueEditor    valueEditor(screen);
static IrBruteforce   irBruteforce(screen, kIrPin);
static IrCodeSender   irCodeSender(screen, kIrPin);
static IrRepeatSender irRepeatSender(screen, kIrPin);
static BleKeyboard    bleKeyboard(screen, "M5 Macro", 4321);

static ListScreen         listScreen(mainList);
static IrMenuScreen       irMenuScreen(irList, buttonSelect, kLongPressMs);
static BrightnessScreen   brightnessScreen(screen, valueEditor,
                                           buttonUp, buttonDown,
                                           kRepeatDelayMs, kRepeatIntervalMs);
static IrBruteforceScreen irBruteforceScreen(irBruteforce);
static IrSendScreen       irSendScreen(irCodeSender,
                                       buttonUp, buttonDown, buttonSelect,
                                       kRepeatDelayMs, kRepeatIntervalMs, kLongPressMs);
static IrRepeatScreen     irRepeatScreen(irRepeatSender,
                                         buttonUp, buttonDown, buttonSelect,
                                         kRepeatDelayMs, kRepeatIntervalMs, kLongPressMs);
static LampRemoteScreen   lampRemoteScreen(lampRemote, buttonSelect, kLongPressMs);
static BleKeyboardScreen  bleKeyboardScreen(bleKeyboard,
                                           buttonUp, buttonDown, buttonSelect,
                                           kRepeatDelayMs, kRepeatIntervalMs, kLongPressMs);

static ScreenManager manager;

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

    manager.registerScreen(ScreenId::List,         &listScreen);
    manager.registerScreen(ScreenId::IrMenu,       &irMenuScreen);
    manager.registerScreen(ScreenId::Brightness,   &brightnessScreen);
    manager.registerScreen(ScreenId::IrBruteforce, &irBruteforceScreen);
    manager.registerScreen(ScreenId::IrSend,       &irSendScreen);
    manager.registerScreen(ScreenId::IrRepeat,     &irRepeatScreen);
    manager.registerScreen(ScreenId::LampRemote,   &lampRemoteScreen);
    manager.registerScreen(ScreenId::BleKeyboard,  &bleKeyboardScreen);

    manager.begin(ScreenId::List);
}

void loop()
{
    uint32_t now = millis();
    Inputs inputs{buttonUp, buttonDown, buttonSelect, now};
    manager.tick(inputs);
    delay(10);
}

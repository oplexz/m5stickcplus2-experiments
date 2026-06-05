#include "BleKeyboard.h"

#include <cstdio>

// ── server callbacks ──────────────────────────────────────────────────────────

class BleKeyboardServerCallbacks : public NimBLEServerCallbacks
{
public:
    explicit BleKeyboardServerCallbacks(bool* connected, bool* authenticated)
    : connected_(connected), authenticated_(authenticated)
    {}

    void onConnect(NimBLEServer*, NimBLEConnInfo&) override
    {
        *connected_     = true;
        *authenticated_ = false;
    }

    void onDisconnect(NimBLEServer*, NimBLEConnInfo&, int) override
    {
        *connected_     = false;
        *authenticated_ = false;
    }

    void onAuthenticationComplete(NimBLEConnInfo& info) override
    {
        *authenticated_ = info.isEncrypted() || info.isBonded();
    }

private:
    bool* connected_;
    bool* authenticated_;
};

// ── constants ─────────────────────────────────────────────────────────────────

namespace
{
constexpr uint8_t kKeyboardReportId = 1;

// ── key codes ─────────────────────────────────────────────────────────────────
constexpr uint8_t kEnter  = 0x28;
constexpr uint8_t kEsc    = 0x29;
constexpr uint8_t kTab    = 0x2B;
constexpr uint8_t kSpace  = 0x2C;
constexpr uint8_t kBacksp = 0x2A;
constexpr uint8_t kUp     = 0x52;
constexpr uint8_t kDown   = 0x51;
constexpr uint8_t kLeft   = 0x50;
constexpr uint8_t kRight  = 0x4F;
// letters: a=0x04 b=0x05 ... z=0x1D
constexpr uint8_t kA = 0x04, kB = 0x05, kC = 0x06, kD = 0x07, kE = 0x08;
constexpr uint8_t kF = 0x09, kG = 0x0A, kH = 0x0B, kI = 0x0C, kJ = 0x0D;
constexpr uint8_t kK = 0x0E, kL = 0x0F, kM = 0x10, kN = 0x11, kO = 0x12;
constexpr uint8_t kP = 0x13, kQ = 0x14, kR = 0x15, kS = 0x16, kT = 0x17;
constexpr uint8_t kU = 0x18, kV = 0x19, kW = 0x1A, kX = 0x1B, kY = 0x1C;
constexpr uint8_t kZ = 0x1D;
// function keys
constexpr uint8_t kF1 = 0x3A, kF2 = 0x3B, kF3 = 0x3C, kF4 = 0x3D;
constexpr uint8_t kF5 = 0x3E, kF6 = 0x3F, kF7 = 0x40, kF8 = 0x41;
constexpr uint8_t kF9 = 0x42, kF10 = 0x43, kF11 = 0x44, kF12 = 0x45;
// modifiers (for use with combo())
constexpr uint8_t kModCtrl  = 0x01;
constexpr uint8_t kModShift = 0x02;
constexpr uint8_t kModAlt   = 0x04;
constexpr uint8_t kModGui   = 0x08; // Win / Cmd
constexpr uint32_t kSentFlashDurationMs  = 600;
constexpr uint32_t kDrawIntervalMs       = 100;

static uint8_t kReportMap[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x06,  // Usage (Keyboard)
    0xA1, 0x01,  // Collection (Application)
    0x85, 0x01,  //   Report ID (1)
    // Modifier keys (8 bits)
    0x05, 0x07,  //   Usage Page (Key Codes)
    0x19, 0xE0,  //   Usage Minimum (Left Ctrl)
    0x29, 0xE7,  //   Usage Maximum (Right GUI)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x01,  //   Logical Maximum (1)
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x08,  //   Report Count (8)
    0x81, 0x02,  //   Input (Data, Variable, Absolute)
    // Reserved byte
    0x95, 0x01,  //   Report Count (1)
    0x75, 0x08,  //   Report Size (8)
    0x81, 0x01,  //   Input (Constant)
    // Key array (6 simultaneous keys)
    0x95, 0x06,  //   Report Count (6)
    0x75, 0x08,  //   Report Size (8)
    0x15, 0x00,  //   Logical Minimum (0)
    0x25, 0x65,  //   Logical Maximum (101)
    0x05, 0x07,  //   Usage Page (Key Codes)
    0x19, 0x00,  //   Usage Minimum (0)
    0x29, 0x65,  //   Usage Maximum (101)
    0x81, 0x00,  //   Input (Data, Array, Absolute)
    0xC0,        // End Collection
};
}

// ── BleKeyboard ───────────────────────────────────────────────────────────────

BleKeyboard::BleKeyboard(M5GFX& screen, const char* deviceName, uint32_t passkey)
: screen_(screen)
, canvas_(&screen)
, canvasReady_(false)
, name_(deviceName)
, passkey_(passkey)
, active_(false)
, bleReady_(false)
, bleConnected_(false)
, bleAuthenticated_(false)
, sentFlashMs_(0)
, lastDrawMs_(0)
, server_(nullptr)
, hid_(nullptr)
, inputKeyboard_(nullptr)
, serverCallbacks_(nullptr)
{
}

void BleKeyboard::start()
{
    active_        = true;
    sentFlashMs_   = 0;
    lastDrawMs_    = 0;

    if (!canvasReady_)
    {
        canvas_.setColorDepth(16);
        canvasReady_ = (canvas_.createSprite(screen_.width(), screen_.height()) != nullptr);
    }

    if (!bleReady_)
    {
        initBle();
    }
    else if (!NimBLEDevice::getAdvertising()->isAdvertising())
    {
        NimBLEDevice::getAdvertising()->start();
    }
}

void BleKeyboard::stop()
{
    active_ = false;
    if (bleReady_)
    {
        NimBLEDevice::getAdvertising()->stop();
    }
    if (bleAuthenticated_ && inputKeyboard_ != nullptr)
    {
        uint8_t release[8] = {};
        inputKeyboard_->setValue(release, sizeof(release));
        inputKeyboard_->notify();
    }
}

void BleKeyboard::tick()
{
    if (!active_)
    {
        return;
    }

    uint32_t now = millis();
    if (now - lastDrawMs_ >= kDrawIntervalMs)
    {
        lastDrawMs_ = now;
        drawScreen(now);
    }
}

void BleKeyboard::typeString(const char* str)
{
    if (!bleAuthenticated_ || inputKeyboard_ == nullptr)
    {
        return;
    }

    for (const char* p = str; *p; ++p)
    {
        uint8_t modifier = 0;
        uint8_t keycode  = charToKeycode(*p, modifier);
        if (keycode != 0)
        {
            sendKey(modifier, keycode);
            delay(8);
        }
    }

    sentFlashMs_ = millis();
}

// ── macro sequence ────────────────────────────────────────────────────────────
// comment out any line to skip that step

void BleKeyboard::runMacro()
{
    if (!bleAuthenticated_) return;

    auto key   = [&](uint8_t k)              { sendKey(0x00,      k); };
    auto shift = [&](uint8_t k)              { sendKey(kModShift, k); };
    auto ctrl  = [&](uint8_t k)              { sendKey(kModCtrl,  k); };
    auto combo = [&](uint8_t mod, uint8_t k) { sendKey(mod,       k); };
    // sleep(ms) — fixed pause
    // sleep(minMs, maxMs) — random pause in range
    auto sleep = [&](uint32_t minMs, uint32_t maxMs = 0) {
        delay(maxMs > minMs ? static_cast<uint32_t>(random(minMs, maxMs + 1)) : minMs);
    };
    (void)shift; (void)ctrl; (void)combo; // suppress unused warnings

    key(kEnter);
    sleep(300, 400);
    key(kEnter);
    sleep(900, 1000);
    key(kY);
    sleep(205, 305);
    key(kDown);
    sleep(50, 150);
    key(kEnter);
    sleep(200, 300);
    key(kEnter);
    sleep(200, 300);
    key(kEsc);
    sleep(75, 150);
    key(kEsc);
    sleep(75, 150);
    key(kEsc);
    sleep(75, 150);
    key(kEsc);
    sleep(75, 150);
    key(kEsc);
    sleep(700, 800);

    sentFlashMs_ = millis();
}

bool BleKeyboard::isConnected() const
{
    return bleAuthenticated_;
}

uint8_t BleKeyboard::charToKeycode(char c, uint8_t& modifier)
{
    modifier = 0;
    if (c >= 'a' && c <= 'z') return static_cast<uint8_t>(0x04 + (c - 'a'));
    if (c >= 'A' && c <= 'Z') { modifier = 0x02; return static_cast<uint8_t>(0x04 + (c - 'A')); }
    if (c == ' ')              return 0x2C;
    if (c == '\n' || c == '\r') return 0x28;
    if (c == '\t')             return 0x2B;
    return 0;
}

void BleKeyboard::initBle()
{
    if (!NimBLEDevice::isInitialized())
    {
        NimBLEDevice::init(name_);
    }
    else
    {
        NimBLEDevice::setDeviceName(name_);
    }

    if (passkey_ > 0)
    {
        NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM);
        NimBLEDevice::setSecurityPasskey(passkey_);
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
    }
    else
    {
        NimBLEDevice::setSecurityAuth(BLE_SM_PAIR_AUTHREQ_BOND);
        NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
    }

    server_ = NimBLEDevice::createServer();
    server_->advertiseOnDisconnect(true);

    serverCallbacks_ = new BleKeyboardServerCallbacks(&bleConnected_, &bleAuthenticated_);
    server_->setCallbacks(serverCallbacks_);

    hid_           = new NimBLEHIDDevice(server_);
    inputKeyboard_ = hid_->getInputReport(kKeyboardReportId);

    hid_->setReportMap(kReportMap, sizeof(kReportMap));
    hid_->setManufacturer("M5Stack");
    hid_->setPnp(0x02, 0x1234, 0x5678, 0x0110);
    hid_->setHidInfo(0x00, 0x01);
    hid_->startServices();
    hid_->setBatteryLevel(80);

    NimBLEAdvertising* adv = NimBLEDevice::getAdvertising();
    adv->setName(name_);
    adv->setAppearance(HID_KEYBOARD);
    adv->addServiceUUID(hid_->getHidService()->getUUID());
    adv->start();

    bleReady_ = true;
}

void BleKeyboard::sendKey(uint8_t modifier, uint8_t keycode)
{
    if (!bleAuthenticated_ || inputKeyboard_ == nullptr)
    {
        return;
    }

    uint8_t press[8] = {modifier, 0x00, keycode, 0x00, 0x00, 0x00, 0x00, 0x00};
    inputKeyboard_->setValue(press, sizeof(press));
    inputKeyboard_->notify();

    delay(8);

    uint8_t release[8] = {};
    inputKeyboard_->setValue(release, sizeof(release));
    inputKeyboard_->notify();
}

void BleKeyboard::drawScreen(uint32_t nowMs)
{
    if (canvasReady_)
    {
        drawContent(canvas_, nowMs);
        canvas_.pushSprite(0, 0);
    }
    else
    {
        drawContent(screen_, nowMs);
    }
}

template <typename T>
void BleKeyboard::drawContent(T& d, uint32_t nowMs)
{
    const int16_t w    = screen_.width();
    const int16_t h    = screen_.height();
    const int16_t midY = h / 2 - 8;

    d.fillScreen(TFT_BLACK);

    // ── top bar ───────────────────────────────────────────────────────────────
    d.setTextSize(1);
    d.setTextColor(TFT_WHITE, TFT_BLACK);
    d.setCursor(3, 4);
    d.print(name_);

    d.setCursor(w - 30, 4);
    if (bleAuthenticated_)      { d.setTextColor(TFT_GREEN,  TFT_BLACK); d.print("BLE+"); }
    else if (bleConnected_)     { d.setTextColor(TFT_YELLOW, TFT_BLACK); d.print("BLE~"); }
    else                        { d.setTextColor(0x8410,     TFT_BLACK); d.print("BLE-"); }
    d.setTextColor(TFT_WHITE, TFT_BLACK);

    d.drawFastHLine(0, 14, w, 0x4208);

    // ── middle content ────────────────────────────────────────────────────────
    bool showSent = (sentFlashMs_ != 0 && nowMs - sentFlashMs_ < kSentFlashDurationMs);

    if (showSent)
    {
        d.setTextSize(2);
        d.setTextColor(TFT_YELLOW, TFT_BLACK);
        d.setCursor(w / 2 - 24, midY);
        d.print("Sent!");
        d.setTextSize(1);
        d.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    else if (bleAuthenticated_)
    {
        d.setCursor(3, midY);
        d.print("Press btn");
        d.setTextColor(TFT_CYAN, TFT_BLACK);
        d.setCursor(3, midY + 12);
        d.print("  -> macro");
        d.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    else if (bleConnected_)
    {
        d.setTextColor(TFT_YELLOW, TFT_BLACK);
        d.setCursor(3, midY - 4);
        d.print("Authenticating...");
        if (passkey_ > 0)
        {
            char pinStr[12];
            snprintf(pinStr, sizeof(pinStr), "%lu", static_cast<unsigned long>(passkey_));
            int16_t pinX = (w - static_cast<int16_t>(strlen(pinStr)) * 12) / 2;
            d.setTextSize(2);
            d.setTextColor(TFT_CYAN, TFT_BLACK);
            d.setCursor(pinX, midY + 10);
            d.print(pinStr);
            d.setTextSize(1);
        }
        d.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    else
    {
        if (passkey_ > 0)
        {
            char pinStr[12];
            snprintf(pinStr, sizeof(pinStr), "%lu", static_cast<unsigned long>(passkey_));
            int16_t pinX = (w - static_cast<int16_t>(strlen(pinStr)) * 12) / 2;
            d.setTextColor(0x8410, TFT_BLACK);
            d.setCursor(3, midY - 8);
            d.print("Advertising... PIN:");
            d.setTextSize(2);
            d.setTextColor(TFT_CYAN, TFT_BLACK);
            d.setCursor(pinX, midY + 6);
            d.print(pinStr);
            d.setTextSize(1);
            d.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        else
        {
            d.setCursor(3, midY);
            d.print("Advertising...");
        }
    }

    // ── bottom hint ───────────────────────────────────────────────────────────
    d.drawFastHLine(0, h - 14, w, 0x4208);
    d.setTextColor(0x8410, TFT_BLACK);
    d.setCursor(3, h - 10);
    d.print("Short=run  Hold=back");
    d.setTextColor(TFT_WHITE, TFT_BLACK);
}

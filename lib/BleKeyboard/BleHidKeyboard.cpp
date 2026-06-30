#include "BleHidKeyboard.h"

// ── Server callbacks ──────────────────────────────────────────────────────────

class BleHidKeyboardCallbacks : public NimBLEServerCallbacks
{
public:
    BleHidKeyboardCallbacks(bool* connected, bool* authenticated)
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

// ── HID report descriptor ─────────────────────────────────────────────────────

uint8_t BleHidKeyboard::kReportMap_[] = {
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

// ── BleHidKeyboard ────────────────────────────────────────────────────────────

BleHidKeyboard::BleHidKeyboard(const char* name, uint32_t passkey)
: name_(name)
, passkey_(passkey)
, bleReady_(false)
, connected_(false)
, authenticated_(false)
, server_(nullptr)
, hid_(nullptr)
, inputKeyboard_(nullptr)
, serverCallbacks_(nullptr)
{}

void BleHidKeyboard::init()
{
    if (!bleReady_) initBle();
}

void BleHidKeyboard::startAdvertising()
{
    if (bleReady_ && !NimBLEDevice::getAdvertising()->isAdvertising())
    {
        NimBLEDevice::getAdvertising()->start();
    }
}

void BleHidKeyboard::stop()
{
    if (bleReady_)
    {
        NimBLEDevice::getAdvertising()->stop();
    }
    if (authenticated_ && inputKeyboard_ != nullptr)
    {
        uint8_t release[8] = {};
        inputKeyboard_->setValue(release, sizeof(release));
        inputKeyboard_->notify();
    }
}

bool BleHidKeyboard::isReady()         const { return bleReady_; }
bool BleHidKeyboard::isAdvertising()   const
{
    return bleReady_ && NimBLEDevice::getAdvertising()->isAdvertising();
}
bool BleHidKeyboard::isConnected()     const { return connected_; }
bool BleHidKeyboard::isAuthenticated() const { return authenticated_; }

void BleHidKeyboard::sendKey(uint8_t modifier, uint8_t keycode)
{
    if (!authenticated_ || inputKeyboard_ == nullptr) return;

    uint8_t press[8] = {modifier, 0x00, keycode, 0x00, 0x00, 0x00, 0x00, 0x00};
    inputKeyboard_->setValue(press, sizeof(press));
    inputKeyboard_->notify();

    delay(random(50, 71)); // human-like press duration

    uint8_t release[8] = {};
    inputKeyboard_->setValue(release, sizeof(release));
    inputKeyboard_->notify();
}

void BleHidKeyboard::keyDown(uint8_t modifier, uint8_t keycode)
{
    if (!authenticated_ || inputKeyboard_ == nullptr) return;
    uint8_t press[8] = {modifier, 0x00, keycode, 0x00, 0x00, 0x00, 0x00, 0x00};
    inputKeyboard_->setValue(press, sizeof(press));
    inputKeyboard_->notify();
}

void BleHidKeyboard::keyUp()
{
    if (!authenticated_ || inputKeyboard_ == nullptr) return;
    uint8_t release[8] = {};
    inputKeyboard_->setValue(release, sizeof(release));
    inputKeyboard_->notify();
}

void BleHidKeyboard::typeString(const char* str)
{
    if (!authenticated_ || inputKeyboard_ == nullptr) return;

    for (const char* p = str; *p; ++p)
    {
        uint8_t modifier = 0;
        uint8_t keycode  = HidKeycodes::charToKeycode(*p, modifier);
        if (keycode != 0)
        {
            sendKey(modifier, keycode);
            delay(8);
        }
    }
}

void BleHidKeyboard::initBle()
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

    serverCallbacks_ = new BleHidKeyboardCallbacks(&connected_, &authenticated_);
    server_->setCallbacks(serverCallbacks_);

    hid_           = new NimBLEHIDDevice(server_);
    inputKeyboard_ = hid_->getInputReport(1); // report ID 1

    hid_->setReportMap(kReportMap_, sizeof(kReportMap_));
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

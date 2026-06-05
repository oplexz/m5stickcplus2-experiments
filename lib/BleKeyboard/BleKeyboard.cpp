#include "BleKeyboard.h"
#include "BleKeyboardView.h"
#include "HidKeycodes.h"

BleKeyboard::BleKeyboard(M5GFX& screen, const char* deviceName, uint32_t passkey)
: screen_(screen)
, canvas_(&screen)
, canvasReady_(false)
, transport_(deviceName, passkey)
, name_(deviceName)
, passkey_(passkey)
, active_(false)
, sentFlashMs_(0)
, lastDrawMs_(0)
{}

void BleKeyboard::start()
{
    active_      = true;
    sentFlashMs_ = 0;
    lastDrawMs_  = 0;

    if (!canvasReady_)
    {
        canvas_.setColorDepth(16);
        canvasReady_ = (canvas_.createSprite(screen_.width(), screen_.height()) != nullptr);
    }

    if (!transport_.isReady())
    {
        transport_.init();
    }
    else if (!transport_.isAdvertising())
    {
        transport_.startAdvertising();
    }
}

void BleKeyboard::stop()
{
    active_ = false;
    transport_.stop();
}

void BleKeyboard::tick()
{
    if (!active_) return;

    uint32_t now = millis();
    if (now - lastDrawMs_ >= kDrawIntervalMs)
    {
        lastDrawMs_ = now;
        drawScreen(now);
    }
}

void BleKeyboard::typeString(const char* str)
{
    transport_.typeString(str);
    if (transport_.isAuthenticated())
    {
        sentFlashMs_ = millis();
    }
}

void BleKeyboard::runMacro()
{
    if (!transport_.isAuthenticated()) return;

    using namespace HidKeycodes;

    auto key   = [&](uint8_t k)              { transport_.sendKey(0x00,      k); };
    auto shift = [&](uint8_t k)              { transport_.sendKey(kModShift, k); };
    auto ctrl  = [&](uint8_t k)              { transport_.sendKey(kModCtrl,  k); };
    auto combo = [&](uint8_t mod, uint8_t k) { transport_.sendKey(mod,       k); };
    auto sleep = [&](uint32_t minMs, uint32_t maxMs = 0) {
        delay(maxMs > minMs ? static_cast<uint32_t>(random(minMs, maxMs + 1)) : minMs);
    };
    (void)shift; (void)ctrl; (void)combo;

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
    return transport_.isAuthenticated();
}

void BleKeyboard::drawScreen(uint32_t nowMs)
{
    BleKeyboardView::State state{
        transport_.isConnected(),
        transport_.isAuthenticated(),
        name_,
        passkey_,
        sentFlashMs_
    };

    if (canvasReady_)
    {
        BleKeyboardView::drawContent(canvas_, state, nowMs, screen_.width(), screen_.height());
        canvas_.pushSprite(0, 0);
    }
    else
    {
        BleKeyboardView::drawContent(screen_, state, nowMs, screen_.width(), screen_.height());
    }
}

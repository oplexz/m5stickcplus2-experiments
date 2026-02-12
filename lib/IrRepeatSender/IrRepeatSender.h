#ifndef IR_REPEAT_SENDER_H
#define IR_REPEAT_SENDER_H

#include <Arduino.h>
#include <M5GFX.h>
#include <IRsend.h>

class IrRepeatSender
{
  public:
    IrRepeatSender(M5GFX& screen, uint16_t irPin);

    void begin();

    // Set how often the code is re-sent while active (default 110ms).
    void setRepeatIntervalMs(uint32_t intervalMs);

    void draw();

    // Navigate the code space. Wraps around.
    bool next();
    bool prev();

    // Call every loop(). Sends the code if enough time has elapsed.
    void tick();

    // Start/stop auto-sending.
    void startSending();
    void stopSending();
    bool isSending() const;

    uint8_t address() const;
    uint8_t command() const;
    uint32_t codeIndex() const;

  private:
    void sendCode();
    void drawScreen();
    void drawStatus();

    M5GFX& screen_;
    IRsend irSender_;

    uint32_t codeIndex_; // 0 .. 65535
    uint32_t repeatIntervalMs_;
    uint32_t lastSendMs_;
    bool sending_;
    uint32_t sendCount_;

    static constexpr uint32_t kTotalCodes = 256UL * 256UL;
};

#endif

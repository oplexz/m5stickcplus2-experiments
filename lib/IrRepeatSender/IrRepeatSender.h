#ifndef IR_REPEAT_SENDER_H
#define IR_REPEAT_SENDER_H

#include <Arduino.h>
#include <M5GFX.h>
#include <NecSender.h>
#include <CodeIndex.h>

class IrRepeatSender
{
public:
    IrRepeatSender(M5GFX& screen, uint16_t irPin);

    void begin();

    // Set how often the code is re-sent while active (default 110 ms).
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

    uint8_t  address()   const;
    uint8_t  command()   const;
    uint32_t codeIndex() const;

private:
    M5GFX&     screen_;
    NecSender  sender_;
    CodeIndex  codeIndex_;

    uint32_t repeatIntervalMs_;
    uint32_t lastSendMs_;
    bool     sending_;
    uint32_t sendCount_;
};

#endif

#ifndef IR_CODE_SENDER_H
#define IR_CODE_SENDER_H

#include <Arduino.h>
#include <M5GFX.h>
#include <IRsend.h>

class IrCodeSender
{
  public:
    IrCodeSender(M5GFX& screen, uint16_t irPin);

    void begin();
    void draw();

    // Navigate through the linear code space (address * 256 + command).
    // Returns true if the code index changed.
    bool next();
    bool prev();

    // Send the currently selected NEC code.
    void send();

    // Getters
    uint8_t address() const;
    uint8_t command() const;
    uint32_t codeIndex() const;

  private:
    void drawCode();

    M5GFX& screen_;
    IRsend irSender_;

    uint32_t codeIndex_; // 0 .. 65535

    static constexpr uint32_t kTotalCodes = 256UL * 256UL;
};

#endif

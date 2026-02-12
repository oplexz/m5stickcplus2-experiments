#ifndef IR_BRUTEFORCE_H
#define IR_BRUTEFORCE_H

#include <Arduino.h>
#include <M5GFX.h>
#include <IRsend.h>

class IrBruteforce
{
  public:
    IrBruteforce(M5GFX& screen, uint16_t irPin);

    void setDelayMs(uint32_t delayMs);

    void start();
    void stop();
    bool isRunning() const;

    bool tick();

    uint16_t currentAddress() const;
    uint16_t currentCommand() const;
    uint32_t totalCodes() const;
    uint32_t codesSent() const;

  private:
    void drawProgress();
    void sendCurrentCode();

    M5GFX& screen_;
    IRsend irSender_;

    uint32_t delayMs_;
    uint32_t lastSendMs_;

    bool running_;
    uint16_t address_;
    uint16_t command_;
    uint32_t codesSent_;

    static constexpr uint32_t kTotalCodes = 256UL * 256UL; // 65536
};

#endif

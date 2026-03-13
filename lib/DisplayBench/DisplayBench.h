#ifndef DISPLAY_BENCH_H
#define DISPLAY_BENCH_H

#include <Arduino.h>
#include <M5GFX.h>

class DisplayBench
{
  public:
    DisplayBench(M5GFX& screen);

    void start();
    void tick();

  private:
    M5GFX& screen_;

    uint32_t windowStartMs_;
    uint32_t frameCount_;
    uint64_t accumUs_;
    uint32_t minUs_;
    uint32_t maxUs_;

    float lastFps_;
    float lastAvgMs_;
    uint32_t lastMinUs_;
    uint32_t lastMaxUs_;
    bool whiteFrame_;
};

#endif

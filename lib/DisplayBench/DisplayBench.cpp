#include "DisplayBench.h"

DisplayBench::DisplayBench(M5GFX& screen)
: screen_(screen)
, windowStartMs_(0)
, frameCount_(0)
, accumUs_(0)
, minUs_(0)
, maxUs_(0)
, lastFps_(0.0f)
, lastAvgMs_(0.0f)
, lastMinUs_(0)
, lastMaxUs_(0)
, whiteFrame_(false)
{
}

void DisplayBench::start()
{
    windowStartMs_ = millis();
    frameCount_ = 0;
    accumUs_ = 0;
    minUs_ = 0;
    maxUs_ = 0;
    lastFps_ = 0.0f;
    lastAvgMs_ = 0.0f;
    lastMinUs_ = 0;
    lastMaxUs_ = 0;
    whiteFrame_ = false;
}

void DisplayBench::tick()
{
    uint16_t bgColor = whiteFrame_ ? TFT_WHITE : TFT_BLACK;
    uint16_t fgColor = whiteFrame_ ? TFT_BLACK : TFT_WHITE;

    uint32_t frameStartUs = micros();
    screen_.fillScreen(bgColor);
    uint32_t frameUs = micros() - frameStartUs;

    whiteFrame_ = !whiteFrame_;
    ++frameCount_;
    accumUs_ += frameUs;

    if (minUs_ == 0 || frameUs < minUs_)
    {
        minUs_ = frameUs;
    }
    if (frameUs > maxUs_)
    {
        maxUs_ = frameUs;
    }

    uint32_t now = millis();
    uint32_t elapsedMs = now - windowStartMs_;
    if (elapsedMs >= 1000 && frameCount_ > 0)
    {
        lastFps_ = (frameCount_ * 1000.0f) / elapsedMs;
        lastAvgMs_ = (accumUs_ / static_cast<float>(frameCount_)) / 1000.0f;
        lastMinUs_ = minUs_;
        lastMaxUs_ = maxUs_;

        windowStartMs_ = now;
        frameCount_ = 0;
        accumUs_ = 0;
        minUs_ = 0;
        maxUs_ = 0;
    }

    screen_.setTextColor(fgColor, bgColor);
    screen_.setTextSize(1);
    screen_.setCursor(6, 6);
    screen_.print("Display Bench");
    screen_.setCursor(6, 20);
    screen_.print("Full-screen fill");
    screen_.setCursor(6, 38);
    screen_.printf("FPS: %.1f", lastFps_);
    screen_.setCursor(6, 52);
    screen_.printf("Avg frame: %.2f ms", lastAvgMs_);
    screen_.setCursor(6, 66);
    screen_.printf("Min/Max: %lu/%lu us",
                   static_cast<unsigned long>(lastMinUs_),
                   static_cast<unsigned long>(lastMaxUs_));
    screen_.setCursor(6, screen_.height() - 12);
    screen_.print("Hold Select = back");
}

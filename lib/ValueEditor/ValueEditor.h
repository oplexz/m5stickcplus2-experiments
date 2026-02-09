#ifndef VALUE_EDITOR_H
#define VALUE_EDITOR_H

#include <Arduino.h>
#include <M5GFX.h>

class ValueEditor
{
  public:
    ValueEditor(M5GFX& screen);

    void setLabel(const char* label);
    void setRange(int minValue, int maxValue);
    void setStep(int step);
    void setSuffix(const char* suffix);
    void setValue(int value);

    int value() const;

    bool increase();
    bool decrease();
    void draw();

  private:
    int clamp(int value) const;

    M5GFX& screen_;
    const char* label_;
    const char* suffix_;
    int minValue_;
    int maxValue_;
    int step_;
    int value_;
};

#endif

#include "ValueEditor.h"

ValueEditor::ValueEditor(M5GFX& screen)
: screen_(screen)
, label_("Value")
, suffix_("")
, minValue_(0)
, maxValue_(100)
, step_(1)
, value_(0)
{
}

void ValueEditor::setLabel(const char* label)
{
    label_ = label;
}

void ValueEditor::setRange(int minValue, int maxValue)
{
    minValue_ = minValue;
    maxValue_ = maxValue;
    value_ = clamp(value_);
}

void ValueEditor::setStep(int step)
{
    step_ = step;
}

void ValueEditor::setSuffix(const char* suffix)
{
    suffix_ = suffix;
}

void ValueEditor::setValue(int value)
{
    value_ = clamp(value);
}

int ValueEditor::value() const
{
    return value_;
}

bool ValueEditor::increase()
{
    int next = clamp(value_ + step_);
    if (next == value_)
    {
        return false;
    }
    value_ = next;
    return true;
}

bool ValueEditor::decrease()
{
    int next = clamp(value_ - step_);
    if (next == value_)
    {
        return false;
    }
    value_ = next;
    return true;
}

void ValueEditor::draw()
{
    screen_.fillScreen(TFT_BLACK);
    screen_.setTextColor(TFT_WHITE, TFT_BLACK);

    screen_.setTextSize(2);
    screen_.setCursor(8, 8);
    screen_.print(label_);

    screen_.setTextSize(3);
    screen_.setCursor(8, 48);
    screen_.printf("%d%s", value_, suffix_);
}

int ValueEditor::clamp(int value) const
{
    if (value < minValue_)
    {
        return minValue_;
    }
    if (value > maxValue_)
    {
        return maxValue_;
    }
    return value;
}

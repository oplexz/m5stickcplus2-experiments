#ifndef CODE_INDEX_H
#define CODE_INDEX_H

#include <Arduino.h>

// Wraps the flat 0..65535 NEC code space (256 addresses × 256 commands).
// Provides wrapping navigation and address/command accessors, eliminating the
// duplicated index logic in IrCodeSender and IrRepeatSender.
class CodeIndex
{
public:
    static constexpr uint32_t kTotal = 256UL * 256UL;

    CodeIndex() : index_(0) {}

    void next()  { index_ = (index_ >= kTotal - 1) ? 0          : index_ + 1; }
    void prev()  { index_ = (index_ == 0)           ? kTotal - 1 : index_ - 1; }
    void reset() { index_ = 0; }

    uint8_t  address() const { return static_cast<uint8_t>(index_ >> 8); }
    uint8_t  command() const { return static_cast<uint8_t>(index_ & 0xFF); }
    uint32_t value()   const { return index_; }

private:
    uint32_t index_;
};

#endif

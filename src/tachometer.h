#pragma once

#include <stdint.h>

class Tachometer {
public:
    void begin();
    uint32_t update(uint32_t nowMs);
    uint32_t rpm() const { return rpm_; }

private:
    uint32_t last_sample_ms_ = 0;
    uint32_t rpm_ = 0;
};

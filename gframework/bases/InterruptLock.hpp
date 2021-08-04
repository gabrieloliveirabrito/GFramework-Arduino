#pragma once

class InterruptLock
{
public:
    InterruptLock()
    {
#if !defined(ARDUINO_ARCH_NRF52)
        noInterrupts();
#endif
    }
    ~InterruptLock()
    {
#if !defined(ARDUINO_ARCH_NRF52)
        interrupts();
#endif
    }
};
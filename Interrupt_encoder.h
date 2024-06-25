#ifndef __INCLUDE_INTERRUPT_H__
#define __INCLUDE_INTERRUPT_H__

#include "mbed.h"

class Interrupt_encoder{
    public: 
    Interrupt_encoder(PinName a, PinName b, int resolution = 200, int times = 4);
    int get_count();
    void reset();
    private:
    void pulse_a();
    void pulse_b();
    InterruptIn _slit_a, _slit_b;
    Callback<void(void)> callback_pulse_a;
    Callback<void(void)> callback_pulse_b;
    int _resolution, _times, _count = 0, _angle = 0;
    float _angle_per_pulse;
};

#endif // __INCLUDE_INTERRUPT_H__

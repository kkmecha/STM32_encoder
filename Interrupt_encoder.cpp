#include "Interrupt_encoder.h"

// 割り込み処理でのプログラム
Interrupt_encoder::Interrupt_encoder(PinName a, PinName b, int resolution, int times)
                  : _slit_a(a), _slit_b(b), _resolution(resolution), _times(times)
{  
    callback_pulse_a = callback(this, &Interrupt_encoder::pulse_a);
    callback_pulse_b = callback(this, &Interrupt_encoder::pulse_a);
    _angle_per_pulse = 360.0 / _resolution * _times;
}

int Interrupt_encoder::get_count(){
    _slit_a.rise(callback_pulse_a);
    _slit_a.fall(callback_pulse_a);
    _slit_b.rise(callback_pulse_b);
    _slit_b.fall(callback_pulse_b);

    _angle = (int)(_count / _angle_per_pulse); 
    return _angle;
}

void Interrupt_encoder::reset(){
    _angle = 0;
}

void Interrupt_encoder::pulse_a(){
    if(_slit_a != _slit_b){
        _count++;
    }else{
        _count--;
    }
}

void Interrupt_encoder::pulse_b(){
    if(_slit_a == _slit_b){
        _count++;
    }else{
        _count--;
    }
}

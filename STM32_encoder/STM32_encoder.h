#ifndef __STM32_ENCODER_H__
#define __STM32_ENCODER_H__

#include "mbed.h"
#include <stdint.h>

#define TIM3_TIM4_PERIOD 0xffff
#define MAX_TIMER_VALUE_HALF 32768

class STM32_encoder{
    public:
        STM32_encoder(PinName a, PinName b);
        void start();
        void reset();
        int32_t get_count();

    private:
        enum class _timer_select{
            tim_3,
            tim_4
        };
        TIM_HandleTypeDef _timer;
        TIM_Encoder_InitTypeDef _encoder;
        GPIO_InitTypeDef _GPIO_InitStruct;
        _timer_select _tim;
        void _HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim, PinName a, PinName b);
        void _timer_setting();
        void _encoder_setting();

        static int32_t _encoder_high_bits;
        int32_t _hbits = 0, _count = 0;
};

#endif

#ifndef __STM32_ENCODER_H__
#define __STM32_ENCODER_H__

// #include "mbed.h"
// #include "stm32f7xx.h"
// #include "stm32f767xx.h"
// #include "stm32f7xx_hal_tim_ex.h"
// #include "system_stm32f7xx.h"

#define MAX_TIMER_VALUE_HALF 32768 //16bitの最高値の半分だから32768

class STM32_encoder
{
    public: 
    STM32_encoder(PinName a, PinName b);
    void start();
    void reset();
    int32_t get_count();

    private:
    TIM_HandleTypeDef _htim;
    TIM_Encoder_InitTypeDef encoder;

    void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim, PinName a, PinName b);
    void _timer_instance_setting();
    void _encoder_instance_setting();
    enum class _timer_select{
        tim_3,
        tim_4
    };
    _timer_select _tim;
};

#endif

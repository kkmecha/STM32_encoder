// TIM3 (TIM_3...0) : (PA_6, PA_7), (PB_0, PB_1), (PB_4, PB_5), (PC_6, PC_7) 
// TIM4 (TIM_4...1) : (PB_6, PB_7), (PD_12, PD_13)

#include "mbed.h"
#include "STM32_encoder.h"

STM32_encoder::STM32_encoder(PinName a, PinName b)
{
    HAL_TIM_Encoder_MspInit(&_htim, a, b);
    _timer_instance_setting();
    _encoder_instance_setting();
}

void STM32_encoder::HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim, PinName a, PinName b) // とりあえず形にはできた
{
    GPIO_InitTypeDef _GPIO_InitStruct;

    _GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    _GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    _GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    if(a == PA_6){
        if(b == PA_7){
            __TIM3_CLK_ENABLE();
            __GPIOA_CLK_ENABLE(); 
            _GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
            _GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
            HAL_GPIO_Init(GPIOA, &_GPIO_InitStruct);
            _tim = _timer_select::tim_3;
        }
    }else if(a == PB_0){
        if(b == PB_1){
            __TIM3_CLK_ENABLE();
            __GPIOB_CLK_ENABLE(); 
            _GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
            _GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
            HAL_GPIO_Init(GPIOB, &_GPIO_InitStruct);
            _tim = _timer_select::tim_3;
        }
    }else if(a == PB_4){
        if(b == PB_5){
            __TIM3_CLK_ENABLE();
            __GPIOB_CLK_ENABLE(); 
            _GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
            _GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
            HAL_GPIO_Init(GPIOB, &_GPIO_InitStruct);
            _tim = _timer_select::tim_3;
        }
    }else if(a == PC_6){
        if(b == PC_7){
            __TIM3_CLK_ENABLE();
            __GPIOC_CLK_ENABLE(); 
            _GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
            _GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
            HAL_GPIO_Init(GPIOC, &_GPIO_InitStruct);
            _tim = _timer_select::tim_3;
        }
    } // ここまでがTIM3
    
    else if(a == PB_6){
        if(b == PB_7){
            __TIM4_CLK_ENABLE();
            __GPIOB_CLK_ENABLE(); 
            _GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
            _GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
            HAL_GPIO_Init(GPIOB, &_GPIO_InitStruct);
            _tim = _timer_select::tim_4;
        }
    }else if(a == PD_12){
        if(b == PD_13){
            __TIM4_CLK_ENABLE();
            __GPIOD_CLK_ENABLE(); 
            _GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
            _GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
            HAL_GPIO_Init(GPIOD, &_GPIO_InitStruct);
            _tim = _timer_select::tim_4;
    
        } // ここまでがTIM4
    }
}

void STM32_encoder::_timer_instance_setting()
{
    if(_tim == _timer_select::tim_3){
        _htim.Instance = TIM3; /* 使うタイマー */  // １つ目のエンコーダー
    }else if(_tim == _timer_select::tim_4){
        _htim.Instance = TIM4;
    }
        _htim.Init.Period = 0xffff;
        _htim.Init.Prescaler = 0;
        _htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        _htim.Init.CounterMode = TIM_COUNTERMODE_UP;
}

void STM32_encoder::_encoder_instance_setting()
{
       /*4逓倍で読みたい*/
    encoder.EncoderMode = TIM_ENCODERMODE_TI12; /* CH1、CH2両方読む*/
    encoder.IC1Filter = 0x0f;
    encoder.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING /*立ち上がりを読む*/; 
    encoder.IC1Prescaler = TIM_ICPSC_DIV4;
    encoder.IC1Selection = TIM_ICSELECTION_DIRECTTI;
 
    encoder.IC2Filter = 0x0f;
    encoder.IC2Polarity = TIM_INPUTCHANNELPOLARITY_FALLING /*立ち下がりを読む*/;    
    encoder.IC2Prescaler = TIM_ICPSC_DIV4;
    encoder.IC2Selection = TIM_ICSELECTION_DIRECTTI;
}

void STM32_encoder::start()
{
    HAL_TIM_Encoder_Init(&_htim, &encoder);
    HAL_TIM_Encoder_Start(&_htim,TIM_CHANNEL_ALL);
}

void STM32_encoder::reset()
{
    core_util_critical_section_enter();
    __HAL_TIM_CLEAR_FLAG(&_htim, TIM_IT_UPDATE);
    core_util_critical_section_exit();
}

int32_t STM32_encoder::get_count()
{
    int32_t _count, _hbits, _angle;
    static int _encoder_high_bits = 0;
    if(_tim == _timer_select::tim_3){
        core_util_critical_section_enter();
        _count = TIM3->CNT;
        if ((TIM3->SR & (TIM_FLAG_UPDATE)) == (TIM_FLAG_UPDATE)){
            TIM3->SR = ~(TIM_IT_UPDATE);
            if (TIM3->CNT < MAX_TIMER_VALUE_HALF)
                _encoder_high_bits += 1;
            else
                _encoder_high_bits -= 1;
            _count = TIM3->CNT;
        }
        _hbits = _encoder_high_bits;
        core_util_critical_section_exit();
    }else if(_tim == _timer_select::tim_4){
        core_util_critical_section_enter();
        _count = TIM4->CNT;
        if ((TIM4->SR & (TIM_FLAG_UPDATE)) == (TIM_FLAG_UPDATE)){
            TIM4->SR = ~(TIM_IT_UPDATE);
            if (TIM4->CNT < MAX_TIMER_VALUE_HALF)
                _encoder_high_bits += 1;
            else
                _encoder_high_bits -= 1;
            _count = TIM4->CNT;
        }
        _hbits = _encoder_high_bits;
        core_util_critical_section_exit();
    }
  return  (_hbits << 16) | _count;
}

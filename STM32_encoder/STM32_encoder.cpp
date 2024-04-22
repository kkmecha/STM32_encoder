// TIM3 (TIM_3...0) : (PA_6, PA_7), (PB_0, PB_1), (PB_4, PB_5), (PC_6, PC_7) 
// TIM4 (TIM_4...1) : (PB_6, PB_7), (PD_12, PD_13)

#include "mbed.h"
#include <stdint.h>
#include "STM32_encoder.h"

STM32_encoder::STM32_encoder(PinName a, PinName b){
    _HAL_TIM_Encoder_MspInit(&_timer, a, b);
    _timer_setting();
    _encoder_setting();
}

void STM32_encoder::_HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim, PinName a, PinName b){
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

void STM32_encoder::_timer_setting(){
    if(_tim == _timer_select::tim_3){
        _timer.Instance = TIM3; /* 使うタイマー */ 
    }else if(_tim == _timer_select::tim_4){
        _timer.Instance = TIM4; /* 使うタイマー */ 
    }

    _timer.Init.Period = TIM3_TIM4_PERIOD;
    _timer.Init.Prescaler = 0;
    _timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    _timer.Init.CounterMode = TIM_COUNTERMODE_UP;
}

void STM32_encoder::_encoder_setting(){
/*4逓倍で読みたい*/
    _encoder.EncoderMode = TIM_ENCODERMODE_TI12; /* CH1、CH2両方読む*/
    _encoder.IC1Filter = 0x0f;

    _encoder.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING; /*立ち上がりを読む*/; 
    _encoder.IC1Prescaler = TIM_ICPSC_DIV4;
    _encoder.IC1Selection = TIM_ICSELECTION_DIRECTTI;

    _encoder.IC2Filter = 0x0f;
    _encoder.IC2Polarity = TIM_INPUTCHANNELPOLARITY_FALLING; /*立ち下がりを読む*/;    
    _encoder.IC2Prescaler = TIM_ICPSC_DIV4;
    _encoder.IC2Selection = TIM_ICSELECTION_DIRECTTI;

}

void STM32_encoder::start(){
   HAL_TIM_Encoder_Init(&_timer, &_encoder);
   HAL_TIM_Encoder_Start(&_timer,TIM_CHANNEL_ALL);
}

int32_t STM32_encoder::get_count(){ // 結局ここの処理いらなくね？　（_hbits(_encoder_high_bits)が１の時は正転　０の時は逆転　TIMx->CNTはunsign型なので、これを利用して負数の表示もしたい）
    if (_tim == STM32_encoder::_timer_select::tim_3){
        core_util_critical_section_enter();
        _count = TIM3->CNT;
        if ((TIM3->SR & (TIM_FLAG_UPDATE)) == (TIM_FLAG_UPDATE))
        {
            TIM3->SR = ~(TIM_IT_UPDATE);
            if (TIM3->CNT < MAX_TIMER_VALUE_HALF)
                _encoder_high_bits += 1;
            else
                _encoder_high_bits -= 1;
            _count = TIM3->CNT;
        }
        _hbits = _encoder_high_bits;
        // printf("TIM3->CNT:%d, _encoder_high_bits:%d, _count:%d, _hbits:%d, (_hbits << 16) | _count:%d\r\n", TIM3->CNT, _encoder_high_bits, _count, _hbits, (_hbits << 16) | _count);
        core_util_critical_section_exit();
    }else if (_tim == STM32_encoder::_timer_select::tim_4){
        core_util_critical_section_enter();
        _count = TIM4->CNT;
        if ((TIM4->SR & (TIM_FLAG_UPDATE)) == (TIM_FLAG_UPDATE))
        {
            TIM4->SR = ~(TIM_IT_UPDATE);
            if (TIM4->CNT < MAX_TIMER_VALUE_HALF)
                _encoder_high_bits += 1;
            else
                _encoder_high_bits -= 1;
            _count = TIM4->CNT;
        }
        _hbits = _encoder_high_bits;
        core_util_critical_section_exit();
        // printf("TIM4->CNT:%u, TIM$->SR:%u_encoder_high_bits:%d, _count:%d, _hbits:%d, (_hbits << 16) | _count:%d\r\n", TIM4->CNT, TIM4->SR, _encoder_high_bits, _count, _hbits, (_hbits << 16) | _count);
    }
    return  ((_hbits << 16) | (_count)) - 65536;
}

void STM32_encoder::reset(){
    STM32_encoder::_encoder_high_bits = 0;
    if(_tim == _timer_select::tim_3){
        TIM3->CNT = 0;
    }else if(_tim == _timer_select::tim_4){
        TIM4->CNT = 0;
    }
    core_util_critical_section_enter();
    __HAL_TIM_CLEAR_FLAG(&_timer, TIM_IT_UPDATE);
    core_util_critical_section_exit();
}

int32_t STM32_encoder::_encoder_high_bits = 0;

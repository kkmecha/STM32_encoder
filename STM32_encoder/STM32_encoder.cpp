#include "mbed.h"
#include "STM32_encoder.h"

// ライブラリが正常に動くようになり次第増やしていく
const TIM_Pin_Map tim_mappings[] = { // 独自のpinmap
    #ifdef TARGET_STM32F072RB
    #elif defined (TARGET_STM32F303K8)
    #elif defined (TARGET_STM32F401RE)
    #elif defined (TARGET_STM32F746ZG)
    #elif defined (TARGET_STM32F767ZI)
    // TIM_TypeDef*, GPIO_TypeDef*, (PinName, uint16_t)x2, uint8_t
    {TIM3, GPIOA, PA_6,  GPIO_PIN_6,  PA_7,  GPIO_PIN_7,  GPIO_AF2_TIM3},
    {TIM3, GPIOB, PB_0,  GPIO_PIN_0,  PB_1,  GPIO_PIN_1,  GPIO_AF2_TIM3},
    {TIM3, GPIOB, PB_4,  GPIO_PIN_4,  PB_5,  GPIO_PIN_5,  GPIO_AF2_TIM3},
    {TIM3, GPIOC, PC_6,  GPIO_PIN_6,  PC_7,  GPIO_PIN_7,  GPIO_AF2_TIM3},
    {TIM4, GPIOB, PB_6,  GPIO_PIN_6,  PB_7,  GPIO_PIN_7,  GPIO_AF2_TIM4},
    {TIM4, GPIOD, PD_12, GPIO_PIN_12, PD_13, GPIO_PIN_13, GPIO_AF2_TIM4},
    #elif
    #error "STM32_encoder's pinmap not found\r\n"
    #endif
};

STM32_encoder::STM32_encoder(PinName slit_a, PinName slit_b) : a(slit_a), b(slit_b)
{
    HAL_TIM_Encoder_MspInit(&_htim, slit_a, slit_b);


}

void STM32_encoder::HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim, PinName slit_a, PinName slit_b)
{
    GPIO_InitTypeDef _GPIO_InitStruct;

    _GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    _GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    _GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

    for(const TIM_Pin_Map& mapping : tim_mappings){ // 配列の全要素の走査
        if(mapping.Pin_name.pin_a == slit_a && mapping.Pin_name.pin_b == slit_b){ // 引数で指定されたピンがTIM_Pin_Mapとあっているか確認
            if(mapping.tim_instance == TIM3){
                __TIM3_CLK_ENABLE();
            }else if(mapping.tim_instance == TIM4){
                __TIM4_CLK_ENABLE();
            }
            _GPIO_InitStruct.Pin = mapping.Pin_name.pin_a_selected | mapping.Pin_name.pin_b_selected;
            _GPIO_InitStruct.Alternate = mapping.pin_alternate;
            HAL_GPIO_Init(mapping.gpio_port, &_GPIO_InitStruct);
            return;
        }
    }
    printf("GPIO pin map not found\r\n"); // STM32_encoderのpinmapの中で例外が起きた時の処理
}

void STM32_encoder::start(){
    // timer
    for(const TIM_Pin_Map& mapping : tim_mappings){
        if(mapping.Pin_name.pin_a == a && mapping.Pin_name.pin_b == b){
            _htim.Instance = mapping.tim_instance;
        }
    }
    _htim.Init.Period = 0xffff;
    _htim.Init.Prescaler = 0;
    _htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    _htim.Init.CounterMode = TIM_COUNTERMODE_UP;

    // encoder
    _encoder.EncoderMode = TIM_ENCODERMODE_TI12;
    _encoder.IC1Filter = 0x0f;
    _encoder.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING; 
    _encoder.IC1Prescaler = TIM_ICPSC_DIV4;
    _encoder.IC1Selection = TIM_ICSELECTION_DIRECTTI;
 
    _encoder.IC2Filter = 0x0f;
    _encoder.IC2Polarity = TIM_INPUTCHANNELPOLARITY_FALLING;    
    _encoder.IC2Prescaler = TIM_ICPSC_DIV4;
    _encoder.IC2Selection = TIM_ICSELECTION_DIRECTTI;

    HAL_TIM_Encoder_Init(&_htim, &_encoder);
    HAL_TIM_Encoder_Start(&_htim,TIM_CHANNEL_ALL);
}

void STM32_encoder::reset(){
    core_util_critical_section_enter();
    __HAL_TIM_CLEAR_FLAG(&_htim, TIM_IT_UPDATE);
    core_util_critical_section_exit();
}

int32_t  STM32_encoder::get_count(){
    int32_t _count, _hbits;
    static int _encoder_high_bits = 0;
    core_util_critical_section_enter();
    _count = _htim.Instance->CNT;
    if ((_htim.Instance->SR & TIM_FLAG_UPDATE) == TIM_FLAG_UPDATE) {
        _htim.Instance->SR = ~(TIM_IT_UPDATE);
        if (_htim.Instance->CNT < 32768) {
            _encoder_high_bits += 1;
        } else {
            _encoder_high_bits -= 1;
        }
        _count = _htim.Instance->CNT;
    }
    _hbits = _encoder_high_bits;
    core_util_critical_section_exit();
    return (_hbits << 16) | _count;
}

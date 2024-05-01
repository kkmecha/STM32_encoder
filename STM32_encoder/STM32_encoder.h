#ifndef __STM32_ENCODER__
#define __STM32_ENCODER__

#include "mbed.h"

typedef struct{
    PinName pin_a;
    uint16_t pin_a_selected;
    PinName pin_b;
    uint16_t pin_b_selected;
}Pin_Pointer;

typedef struct{
    TIM_TypeDef* tim_instance;
    GPIO_TypeDef* gpio_port;
    Pin_Pointer Pin_name;
    uint8_t pin_alternate;
}TIM_Pin_Map;

extern const TIM_Pin_Map tim_mappings[];

class STM32_encoder{
    public:
    STM32_encoder(PinName slit_a, PinName slit_b, int resolution, int times);
    void encoder_start();
    void encoder_reset();
    int32_t encoder_get_count();

    private:
    PinName _a, _b;
    int _resolution = 200, _times = 4; // 分解能, 何逓倍か
    void _GPIO_InitPeriph(PinName slit_a, PinName slit_b)
    TIM_HandleTypeDef _htim;
    GPIO_InitTypeDef _GPIO_InitStruct;
    TIM_Encoder_InitTypeDef _encoder;
};

#endif

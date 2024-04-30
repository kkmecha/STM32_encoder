#ifndef __STM32_ENCODER__
#define __STM32_ENCODER__

#include "mbed.h"
#include <stdlib.h>

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
    STM32_encoder(PinName slit_a, PinName slit_b);
    void GPIO_InitPeriph(PinName slit_a, PinName slit_b);
    void start();
    void reset();
    int32_t get_count();

    private:
    PinName a, b;
    TIM_HandleTypeDef _htim;
    GPIO_InitTypeDef _GPIO_InitStruct;
    TIM_Encoder_InitTypeDef _encoder;
};

#endif

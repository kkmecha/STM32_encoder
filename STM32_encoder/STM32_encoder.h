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
    STM32_encoder(MicroControler_Type name, PinName slit_a, PinName slit_b);
    void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim, MicroControler_Type name, PinName slit_a, PinName slit_b);
    void start();
    void reset();
    int32_t get_count();
    private:
    TIM_HandleTypeDef _htim;
    TIM_Encoder_InitTypeDef _encoder;
};

#endif

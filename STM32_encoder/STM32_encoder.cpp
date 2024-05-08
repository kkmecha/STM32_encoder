//                                  2^32 - 1
//         _bhitsの動き         ...4294967295 -> 0 ->    1   ...
//   (_hbits << 16) | _count    ...-65536~-1  -> 0 -> 1~65536...
#include "mbed.h"
#include "STM32_encoder.h"

// これはAMT102(4逓倍のエンコーダー)のライブラリです
// ライブラリが正常に動くようになり次第増やしていく

// F7xx系だと使える
// F072RBでOSエラー吐かれた -> 解消
// F303K8は使えるやつが余ってなくて検証できなかった
// F401REは生きてる実機がなくて検証できなかった

// 追加したTIMに対応したプログラムの実装
// OSエラーの原因の解明と改善(F072RBのみ)
// もっとエンコーダーを使えるような方法がないか調べる

const TIM_Pin_Map tim_mappings[] = { // 独自のpinmap
    // TIM_TypeDef*, GPIO_TypeDef*, (PinName, uint16_t)x2, uint8_t
    #ifdef TARGET_NUCLEO_F072RB // OSエラー吐いた -> 解消 正常に動作 ok
    // TIM2...32bit TIM3...16bit
    // Max Interface clock..48, Man timer clock...48

    // TIM3は正常
    // TIM2は正常に動いたものがなかった

    {TIM3,  0xffff,  GPIOA,  {PA_6,  GPIO_PIN_6,  PA_7,  GPIO_PIN_7},  GPIO_AF1_TIM3},
    {TIM3,  0xffff,  GPIOB,  {PB_4,  GPIO_PIN_4,  PB_5,  GPIO_PIN_5},  GPIO_AF1_TIM3},
    {TIM3,  0xffff,  GPIOC,  {PC_6,  GPIO_PIN_6,  PC_7,  GPIO_PIN_7},  GPIO_AF0_TIM3},
    //{TIM3, 0xffff,  GPIOE,  {PE_3,  GPIO_PIN_3,  PE_4,  GPIO_PIN_4},  GPIO_AF0_TIM3},

    // pinしんでる、、？(そんなわけない) そのくらい反応しない
    // PA_1は元気そう
    {TIM2,  0xffffffff,  GPIOA,  {PA_0,  GPIO_PIN_0,  PA_1,  GPIO_PIN_1},  GPIO_AF2_TIM2},
    {TIM2,  0xffffffff,  GPIOA,  {PA_5,  GPIO_PIN_5,  PA_1,  GPIO_PIN_1},  GPIO_AF2_TIM2},
    {TIM2,  0xffffffff,  GPIOA,  {PA_15, GPIO_PIN_15, PA_1,  GPIO_PIN_1},  GPIO_AF2_TIM2},

    #elif defined (TARGET_NUCLEO_F303K8) // 確認できなかった
    // TIM2...32bit TIM3...16bit
    // Max Interface clock..72, Man timer clock...72

    {TIM3,  0xffff,  GPIOA,  {PA_6,  GPIO_PIN_6,  PA_4,  GPIO_PIN_4},  GPIO_AF2_TIM3},
    {TIM3,  0xffff,  GPIOA,  {PA_6,  GPIO_PIN_6,  PA_7,  GPIO_PIN_7},  GPIO_AF2_TIM3},
    {TIM3,  0xffff,  GPIOB,  {PB_4,  GPIO_PIN_4,  PB_5,  GPIO_PIN_5},  GPIO_AF2_TIM3},
    //{TIM3,  0xffff,  GPIOC,  {PC_6,  GPIO_PIN_6,  PC_7,  GPIO_PIN_7},  GPIO_AF2_TIM3},

    {TIM2,  0xffffffff,  GPIOA,  {PA_0,  GPIO_PIN_0,  PA_1,  GPIO_PIN_1},  GPIO_AF1_TIM2},
    {TIM2,  0xffffffff,  GPIOA,  {PA_5,  GPIO_PIN_5,  PA_1,  GPIO_PIN_1},  GPIO_AF1_TIM2},
    {TIM2,  0xffffffff,  GPIOA,  {PA_15, GPIO_PIN_15, PA_1,  GPIO_PIN_1},  GPIO_AF1_TIM2},

    #elif defined (TARGET_NUCLEO_F401RE) // 確認できなかった
    // TIM2, TIM5...32bit TIM3, TIM4...16bit
    // Max Interface clock..42, Man timer clock...84
    // 16bit
    {TIM3,  0xffff,  GPIOA,  {PA_6,  GPIO_PIN_6,  PA_7,  GPIO_PIN_7},  GPIO_AF2_TIM3},
    {TIM3,  0xffff,  GPIOB,  {PB_4,  GPIO_PIN_4,  PB_5,  GPIO_PIN_5},  GPIO_AF2_TIM3},
    {TIM3,  0xffff,  GPIOC,  {PC_6,  GPIO_PIN_6,  PC_7,  GPIO_PIN_7},  GPIO_AF2_TIM3},

    {TIM4,  0xffff,  GPIOB,  {PB_6,  GPIO_PIN_6,  PB_7,  GPIO_PIN_7},  GPIO_AF2_TIM4},
    {TIM4,  0xffff,  GPIOD,  {PD_12, GPIO_PIN_12, PD_13, GPIO_PIN_13}, GPIO_AF2_TIM4},

    // 32bit
    {TIM2,  0xffffffff,  GPIOA,  {PA_0,  GPIO_PIN_0,  PA_1,  GPIO_PIN_1},  GPIO_AF1_TIM2},
    {TIM2,  0xffffffff,  GPIOA,  {PA_5,  GPIO_PIN_5,  PA_1,  GPIO_PIN_1},  GPIO_AF1_TIM2},
    {TIM2,  0xffffffff,  GPIOA,  {PA_15, GPIO_PIN_15, PA_1,  GPIO_PIN_1},  GPIO_AF1_TIM2},

    {TIM5,  0xffffffff,  GPIOA,  {PA_0,  GPIO_PIN_0,  PA_1,  GPIO_PIN_1},  GPIO_AF2_TIM5},
    // {TIM5,  0xffffffff,  GPIOH, PH_10, GPIO_PIN_10, PH_11, GPIO_PIN_11, GPIO_AF2_TIM5}, データシート上にはあるがPH_10などはない


    #elif defined (TARGET_NUCLEO_F746ZG) // ok
    // TIM2, TIM5...32bit TIM3, TIM4...16bit
    // Max Interface clock..54, Man timer clock...216
    // 16bit
    {TIM3,  0xffff,  GPIOA,  {PA_6,  GPIO_PIN_6,  PA_7,  GPIO_PIN_7},  GPIO_AF2_TIM3},
    {TIM3,  0xffff,  GPIOB,  {PB_4,  GPIO_PIN_4,  PB_5,  GPIO_PIN_5},  GPIO_AF2_TIM3},
    {TIM3,  0xffff,  GPIOC,  {PC_6,  GPIO_PIN_6,  PC_7,  GPIO_PIN_7},  GPIO_AF2_TIM3},

    {TIM4,  0xffff,  GPIOB,  {PB_6,  GPIO_PIN_6,  PB_7,  GPIO_PIN_7},  GPIO_AF2_TIM4},
    {TIM4,  0xffff,  GPIOD,  {PD_12, GPIO_PIN_12, PD_13, GPIO_PIN_13}, GPIO_AF2_TIM4}, 

    // 32bit
    {TIM2,  0xffffffff,  GPIOA,  {PA_0,  GPIO_PIN_0,  PA_1,  GPIO_PIN_1},  GPIO_AF1_TIM2},
    {TIM2,  0xffffffff,  GPIOA,  {PA_5,  GPIO_PIN_5,  PA_1,  GPIO_PIN_1},  GPIO_AF1_TIM2},
    {TIM2,  0xffffffff,  GPIOA,  {PA_15, GPIO_PIN_15, PA_1,  GPIO_PIN_1},  GPIO_AF1_TIM2},

    {TIM5,  0xffffffff,  GPIOA,  {PA_0,  GPIO_PIN_0,  PA_1,  GPIO_PIN_1},  GPIO_AF2_TIM5},
    // {TIM5,  0xffffffff,  GPIOH, PH_10, GPIO_PIN_10, PH_11, GPIO_PIN_11, GPIO_AF2_TIM5}, データシート上にはあるがPH_10などはない
    #elif defined (TARGET_NUCLEO_F767ZI) // ok
    // TIM2, TIM5...32bit TIM1, TIM3, TIM4, TIM8...16bit 
    // Max Interface clock..54, Man timer clock...216 (TIM1とTIM8は108, 216)
    // 16bit
    {TIM1,  0xffff,  GPIOA,  {PA_8,  GPIO_PIN_8,  PA_9,  GPIO_PIN_9},  GPIO_AF1_TIM1},
    {TIM1,  0xffff,  GPIOA,  {PA_15, GPIO_PIN_15, PA_9,  GPIO_PIN_9},  GPIO_AF1_TIM1},
    {TIM1,  0xffff,  GPIOE,  {PE_9,  GPIO_PIN_9,  PE_11, GPIO_PIN_11}, GPIO_AF1_TIM1},

    {TIM3,  0xffff,  GPIOA,  {PA_6,  GPIO_PIN_6,  PA_7,  GPIO_PIN_7},  GPIO_AF2_TIM3},
    {TIM3,  0xffff,  GPIOB,  {PB_0,  GPIO_PIN_0,  PB_1,  GPIO_PIN_1},  GPIO_AF2_TIM3},
    {TIM3,  0xffff,  GPIOB,  {PB_4,  GPIO_PIN_4,  PB_5,  GPIO_PIN_5},  GPIO_AF2_TIM3},
    {TIM3,  0xffff,  GPIOC,  {PC_6,  GPIO_PIN_6,  PC_7,  GPIO_PIN_7},  GPIO_AF2_TIM3},

    {TIM4,  0xffff,  GPIOB,  {PB_6,  GPIO_PIN_6,  PB_7,  GPIO_PIN_7},  GPIO_AF2_TIM4},
    {TIM4,  0xffff,  GPIOD,  {PD_12, GPIO_PIN_12, PD_13, GPIO_PIN_13}, GPIO_AF2_TIM4},

    {TIM8,  0xffff,  GPIOC,  {PC_6,  GPIO_PIN_6,  PC_7,  GPIO_PIN_7},  GPIO_AF3_TIM8},
    // {TIM8,  0xffff,  GPIOC, PI_5,  GPIO_PIN_5,  PI_6,  GPIO_PIN_6,  GPIO_AF3_TIM8}, データシート上にはあるがPI_5などはない

    // 32bit
    {TIM2,  0xffffffff,  GPIOA,  {PA_0,  GPIO_PIN_0,  PA_1,  GPIO_PIN_1},  GPIO_AF1_TIM2},    //
    {TIM2,  0xffffffff,  GPIOA,  {PA_5,  GPIO_PIN_5,  PA_1,  GPIO_PIN_1},  GPIO_AF1_TIM2},    // PA_1が見当たらない
    {TIM2,  0xffffffff,  GPIOA,  {PA_15, GPIO_PIN_15, PA_1,  GPIO_PIN_1},  GPIO_AF1_TIM2},  //

    {TIM5,  0xffffffff,  GPIOA, PA_0, GPIO_PIN_0, PA_1, GPIO_PIN_1, GPIO_AF2_TIM5}, 
    // {TIM5,  0xffffffff,  GPIOH, PH_10, GPIO_PIN_10, PH_11, GPIO_PIN_11, GPIO_AF2_TIM5}, データシート上にはあるがPH_10などはない
    #endif
};

STM32_encoder::STM32_encoder(PinName slit_a, PinName slit_b, int resolution = 200, int times = 4) : _a(slit_a), _b(slit_b), _resolution(resolution), _times(times)
{
    GPIO_InitPeriph(slit_a, slit_b);
}
 
void STM32_encoder::GPIO_InitPeriph(PinName slit_a, PinName slit_b)
{
    for(const TIM_Pin_Map& mapping : tim_mappings){ // 配列の全要素の走査
        if(mapping.Pin_name.pin_a == slit_a && mapping.Pin_name.pin_b == slit_b){ // 引数で指定されたピンがTIM_Pin_Mapとあっているか確認
            if(mapping.tim_instance == TIM2){
                __TIM2_CLK_ENABLE();
            }else if(mapping.tim_instance == TIM3){
                __TIM3_CLK_ENABLE();
            }/*else if(mapping.tim_instance == TIM4){ // f072rb f303k8 x
                __TIM4_CLK_ENABLE();
            }else if(mapping.tim_instance == TIM5){ // f072rb f303k8 x
                __TIM5_CLK_ENABLE();
            }*/

            if(mapping.gpio_port == GPIOA){
                __GPIOA_CLK_ENABLE();
            }else if(mapping.gpio_port == GPIOB){
                __GPIOB_CLK_ENABLE();
            }else if(mapping.gpio_port == GPIOC){
                __GPIOC_CLK_ENABLE();
            }else if(mapping.gpio_port == GPIOD){
                __GPIOD_CLK_ENABLE();
            }/*else if(mapping.gpio_port == GPIOE){ // f303k8 x
                __GPIOE_CLK_ENABLE();
            }else if(mapping.gpio_port == GPIOH){ // f072rb f303k8 x
                __GPIOH_CLK_ENABLE();
            }*/

            _GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            _GPIO_InitStruct.Pull = GPIO_PULLDOWN; // デフォルト...プルダウン
            _GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
            _GPIO_InitStruct.Pin = mapping.Pin_name.pin_a_selected | mapping.Pin_name.pin_b_selected;
            _GPIO_InitStruct.Alternate = mapping.pin_alternate;
            HAL_GPIO_Init(mapping.gpio_port, &_GPIO_InitStruct);
        }
    }
}

void STM32_encoder::start(){
    // timer
    for(const TIM_Pin_Map& mapping : tim_mappings){
        if(mapping.Pin_name.pin_a == _a && mapping.Pin_name.pin_b == _b){
            _htim.Instance = mapping.tim_instance;
        }
    }
    _htim.Init.Period = tim_mappings->tim_max; // TIMx->CNT(パルスカウンタ)の最大値の設定
    _htim.Init.Prescaler = 0;
    _htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // クロックの何分の一でカウントの計算をするか
    _htim.Init.CounterMode = TIM_COUNTERMODE_UP;

    // encoder
    // CH1の設定
    _encoder.EncoderMode = TIM_ENCODERMODE_TI12; // CH1とCH2の両方を読み取る
    _encoder.IC1Filter = 0x0f;
    _encoder.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING; // 立ち上がりエッジのとき
    _encoder.IC1Prescaler = TIM_ICPSC_DIV4; // 4逓倍を表す
    _encoder.IC1Selection = TIM_ICSELECTION_DIRECTTI;
 
    // CH2の設定
    _encoder.IC2Filter = 0x0f;
    _encoder.IC2Polarity = TIM_INPUTCHANNELPOLARITY_FALLING; // 立ち下がりエッジのとき
    _encoder.IC2Prescaler = TIM_ICPSC_DIV4; // 4逓倍を表す
    _encoder.IC2Selection = TIM_ICSELECTION_DIRECTTI;

    HAL_TIM_Encoder_Init(&_htim, &_encoder);
    HAL_TIM_Encoder_Start(&_htim,TIM_CHANNEL_ALL);
}

void STM32_encoder::reset(){
    core_util_critical_section_enter();
    __HAL_TIM_CLEAR_FLAG(&_htim, TIM_IT_UPDATE);
    core_util_critical_section_exit();
}

int32_t  STM32_encoder::get_count(){ // 16bitの時の処理のみしか書いてない(たぶん改善できた) クロック数の違いにも対応したい
    int32_t _count, _angle;
    core_util_critical_section_enter();
    _count = _htim.Instance->CNT;
    if((_htim.Instance->SR & (TIM_FLAG_UPDATE)) == TIM_FLAG_UPDATE){
        _htim.Instance->SR = ~(TIM_IT_UPDATE);
        if(_htim.Instance->CNT < (_htim.Init.Period + 1) / 2)
            _hbits += 1;
        else
            _hbits -= 1;
        _count = _htim.Instance->CNT;
    }
    _angle = int32_t(((_hbits << 16) | _count) / float(_resolution * _times / 360.0)); // 発生したパルス数 / 1°あたりのパルス数  (発生したパルス数を度数法に変換)
    core_util_critical_section_exit();
    //printf("count:%lu, hbits:%lu, (_hbits << 16) | _count:%ld\r\n", _count, _hbits, (_hbits << 16) | _count);
    //ThisThread::sleep_for(5ms);
    return _angle;
}

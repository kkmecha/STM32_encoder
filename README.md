# STM32_encoder  
STM32_encoderのtest版(2つ目)  
**とりあえず今はNucleo-F767ZIにのみ対応**  

*このbranchではTIM3(AF2(PA_6, PA_7), AF2(PB_0, PB_1), AF2(PB_4, PB_5), AF2(PC_6, PC_7))と  
TIM4(AF2(PB_6, PB_7), AF2(PD_12, PD_13)のみ使えます  

～関数一覧～  
    STM32_encoder()   コンストラクタ  
新しく追加    start()           計測スタート  
名称の変更    get_count()        エンコーダーのパルスを読み取り、int32_t型で返却  
名称の変更    encoder_reset()     相対角をリセット  

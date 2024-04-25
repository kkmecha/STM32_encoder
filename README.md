# STM32_encoder  
STM32_encoderのtest版(2つ目)  
**Nucleo-F767ZIに対応**  
  
使えるピン番号  
Nucleo-F767ZI : (PA_6, PA_7), (PB_0, PB_1), (PB_4, PB_5), (PC_6, PC_7), (PB_6, PB_7), (PD_12, PD_13)  

STM32_encoder(PinName a, PinName b);        コンストラクタ  
start();                                    計測スタート  
reset();                                    データリセット  
get_count();                                角度取得関数  

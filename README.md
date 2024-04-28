# STM32_encoder  
STM32_encoderのtest版(2つ目)  
**Nucleo-F767ZIに対応**  
これから使えるマイコンを増やしていきます  

なぜか片方が-65536, 0, 65535, ...  をまたぐ角度を回したときに変な挙動が、、(ビットが反転してそうな動き)  
  
使えるピン番号  
Nucleo-F767ZI :  
エンコーダー1つ目　(PA_6, PA_7), (PB_0, PB_1), (PB_4, PB_5), (PC_6, PC_7)   
エンコーダー2つ目　(PB_6, PB_7), (PD_12, PD_13)  

STM32_encoder(PinName a, PinName b);        コンストラクタ  
start();                                    計測スタート  
reset();                                    データリセット  
get_count();                                角度取得関数  

#include "mbed.h"
#include "STM32_encoder.h"

// 第3、第4引数(分解能と逓倍の指定)は省略できます
STM32_encoder encoder1(PC_6, PC_7, 800, 4); // 分解能800
STM32_encoder encoder2(PD_12, PD_13, 200, 4); // 分解能200

int32_t a, b;

int main(){
    encoder1.start();
    encoder2.start();
    encoder1.reset();
    encoder2.reset();
    while(true){
        printf("angle1:%ld, angle2:%ld\r\n", encoder1.get_count(), encoder2.get_count());
        ThisThread::sleep_for(5ms);
    }
}

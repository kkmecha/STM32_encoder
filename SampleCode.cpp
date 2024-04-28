#include "mbed.h"
#include "STM32_encoder.h"

STM32_encoder encoder1(PC_6, PC_7);
STM32_encoder encoder2(PD_12, PD_13);

int main()
{
    encoder1.start();
    encoder2.start();
    encoder1.reset();
    encoder2.reset();
    while(true){
        printf("encoder1:%ldencoder2:%ld\r\n", encoder1.get_count(), encoder2.get_count());
        ThisThread::sleep_for(5ms);
    }
}

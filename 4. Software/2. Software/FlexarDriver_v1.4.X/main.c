/*
 * Flexar Driver
 * Hardware Version: v1.4
 * Software Version: v1.0
 * www.flexar.io
 */

#include "mcc_generated_files/mcc.h"
#include "flexar_driver.h"

extern bool timer_flag;

void main(void){
    SYSTEM_Initialize(); /*Initialize the device*/
        
    __delay_ms(100);

    INTERRUPT_GlobalInterruptEnable(); /*Enable the Global Interrupts*/
    INTERRUPT_PeripheralInterruptEnable(); /*Enable the Peripheral Interrupts*/
                 
    driver_init();
    
    while (1){
        if(timer_flag == 1){  
            driver_run();         
            timer_flag = 0; /*Clear 160us timer flag*/
            CLRWDT(); /*Clear Watchdog*/
            loop_handler();        
        }
        else{
            /*Wait*/
        }
    }
}

/**
 End of File
*/
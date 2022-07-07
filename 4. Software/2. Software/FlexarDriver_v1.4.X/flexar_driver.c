#include "mcc_generated_files/mcc.h"
#include "flexar_driver.h"

bool signal_toggle = 0;
uint8_t index_tmr = 0U, adc_avgcounter = 0U, button_tmr = 0U, eepdriver_mode = 1U, driver_mode = 1U, LED_timer = 0u, LED_mode = 0U;
uint16_t index_lastval = 0xFFFFU, period_val = 0U, period_halfval  = 0U, period_tmr = 0U;
uint16_t adc_avgval[8U] = {0U,0U,0U,0U,0U,0U,0U,0U} ;
uint32_t eeptimer = 0U;

const uint16_t period_buffer[64U] = {6312U,	6312U,	6312U,	6312U,	6312U,	3145U,	2098U,	1574U,	1260U,	1051U,	
                                    901U,	789U,	701U,	631U,	574U,	526U,	485U,	451U,	421U,	394U,	
                                    371U,	351U,	332U,	315U,	300U,	287U,	274U,	263U,	252U,	243U,	
                                    234U,	225U,	218U,	210U,	204U,	197U,	191U,	186U,	181U,	176U,	
                                    171U,	166U,	162U,	158U,	154U,	150U,	147U,	144U,	140U,	137U,	
                                    134U,	132U,	129U,	126U,	124U,	121U,	119U,	117U,	115U,	113U,	
                                    111U,	109U,	107U,	105U};

void loop_handler(void){
    if(PORTAbits.RA0 == 0){ /*Button was pressed*/
        if(button_tmr < 100U){
            /*Button Debounce timer++*/
            button_tmr++; 
        }
        else{
            /*Button Press Detected*/
            while(PORTAbits.RA0 == 0){                 
                __delay_us(100); /*Wait for button release*/
            }
            driver_mode++;
            eeptimer = 0U;
            index_lastval = 0xFFFFU;
            driver_handler(driver_mode);       
        }
    }
    else{ /*Button was not pressed*/
        button_tmr = 0U; /*Button Debounce clear*/  
        trimmer_handler();
    }
    LATC =  LED_mode; /*Turn on mode LED*/
    flash_handler(); /*Save Mode in Flash*/
    LATC =  0x00; /*Turn off LEDs - brightness reduction*/   
}

void driver_handler(uint8_t mode_select){
    PWM6CONbits.PWM6EN = 0;/*Disable PWM5*/
    PWM5CONbits.PWM5EN = 0;/*Disable PWM6*/ 
    switch(mode_select){
        case 1: 
        case 7:           
            /*Enter Squarewave Mode*/
            LED_mode = LED_SQR_MODE; /*Turn On Squarewave Mode LED*/
            drive_pwm_config(); /*Set PWM frequency to 16kHz*/
            driver_mode = 1U;/*Reset counter*/  
            break;
        case 2:
            /*Enter PWM Mode*/
            LED_mode = LED_PWM_MODE; /*Turn On PWM Mode LED*/     
            drive_pwm_config(); /*Set PWM frequency to 16kHz*/
            break;
        case 3:            
        case 4:
            /*Enter Position Mode 1/2*/
            LED_mode = LED_POS_MODE; /*Turn On Position Mode LED*/      
            drive_pwm_config(); /*Set PWM frequency to 16kHz*/
            break;
        case 5:
            /*Enter Buzzer Mode*/
            LED_mode = LED_BUZ_MODE; /*Turn On Buzzer Mode LED*/  
            buzzer_pwm_config();
            break;
        case 6:
            /*Enter External Mode*/
           LED_mode = LED_EXT_MODE; /*Turn On External Mode LED*/
           T2CONbits.TMR2ON = 0; /*Disable Timer*/
           PWM6CONbits.PWM6EN = 0;/*Disable PWM5*/
           PWM5CONbits.PWM5EN = 0;/*Disable PWM6*/   
           TRISCbits.TRISC0 = 1U; /*Set pin to input*/
           TRISAbits.TRISA2 = 1U; /*Set pin to input*/              
           break;  
        default:
            break;
    }    
}
   
void flash_handler(void){ 
    if(eepdriver_mode != driver_mode){
        /*5min timer*/
        if(eeptimer < 1875000U){
            eeptimer++; /*160us timer*/         
        }         
        else{
            eeptimer = 0U;
            DATAEE_WriteByte(0x7000U,driver_mode);
            eepdriver_mode = driver_mode;         
        }
    }
    else{
        eeptimer = 0U;
    }
}

void trimmer_handler(void){
    uint16_t adc_newval = ADC_GetConversion(channel_ANA4);
    uint16_t index_val = 0U;
    uint16_t adc_avgsum = 0U;
    
    /*Eliminate R2 drops & get index*/
    if(adc_newval > 1008U){        
        adc_newval = 1008U;
    }
    else{
        /*Skip*/
    }
    
    /*Average Filtering*/
    if(adc_avgcounter >= 7U){
        adc_avgcounter = 0U;
    }
    else{
        adc_avgcounter++;        
    }
    adc_avgval[adc_avgcounter] = adc_newval;
    for (uint8_t i = 0; i <= 7U; i++){
        adc_avgsum += adc_avgval[adc_avgcounter];
    }
    adc_avgsum = adc_avgsum >> 3U;/*Divide by 8*/
     
    /*Get index*/
    index_val = adc_avgsum >> 4U; /*Divide by 16*/

    if(index_val != index_lastval){
        /*Check if new index is in deadband*/
        uint16_t array_val = index_val << 4U; /*Multiply by 16*/
        if(index_tmr < 250U){
            index_tmr++;/*Jitter Filter*/
        }
        else{
            index_lastval = index_val;
                if(driver_mode <= 2U){
                    /*Squarewave/PWM Mode*/
                    period_val = period_buffer[index_val];/*Get period value*/               
                    period_halfval = period_val >> 1U; /*Save the Divide by 2 value*/ 
                }
                else if(driver_mode <= 4U){
                    /*Position Mode 1/2 - 1.6% Resolution*/
                    if(array_val>800U){
                        array_val = 800U;
                    }
                    else{
                        /*skip*/
                    }
                    if(driver_mode == 4U){
                        /*Vary Duty cycle at Position 1*/
                        PWM5_LoadDutyValue(array_val);
                        PWM6_LoadDutyValue(800U);
                    }
                    else{
                        /*Vary Duty cycle at Position 2*/
                        PWM6_LoadDutyValue(array_val);
                        PWM5_LoadDutyValue(0U);
                    }  
                }
                else if(driver_mode == 5U){
                    /*Buzzer Mode*/
                    array_val = array_val >> 3U; /*Divide by 8*/
                    PR2 = (14U + array_val);
                    array_val = 2*(PR2+1);
                    PWM5_LoadDutyValue(array_val);
                    PWM6_LoadDutyValue(array_val); 
                }
                else{
                    /*Skip*/
                }    
            
        }         
    }
    else{
        index_tmr = 0U;
    }  
}

void driver_run(void){    
    uint32_t duty_val = 0U;
    period_tmr++;      
    
    if(driver_mode <= 2U){
        if(driver_mode == 2U){
            /*PWM Mode*/   
            if(signal_toggle){
                if(period_tmr <= period_halfval){ 
                    /*Period is smaller than 75%*/
                    duty_val = ((((uint32_t)period_tmr) * 400U)/((uint32_t)period_halfval)) + 400U;
                }
                else{
                    /*Period is greater than 75%*/
                    duty_val = 1200U - ((((uint32_t)period_tmr) * 800U)/((uint32_t)period_val));
                }
            }
            else{
                if(period_tmr <= period_halfval){ 
                    /*Period is smaller than 25%*/
                    duty_val = (400U - ((((uint32_t)period_tmr) * 400U)/((uint32_t)period_halfval)));
                }
                else{
                    duty_val = (((((uint32_t)period_tmr) * 800U)/((uint32_t)period_val)) - 400U);
                }
            }  
            if(duty_val >= 800U){
                duty_val = 800U;
            }
            else{
                /*Skip*/
            }
            PWM5_LoadDutyValue((uint16_t)duty_val);
            PWM6_LoadDutyValue((uint16_t)duty_val);             
        }
        else{
            /*Skip*/
        } 
        if(period_tmr >= period_val){ /*Match time period*/
            period_tmr = 0U; /*Clear timer*/
            signal_toggle = !signal_toggle;
            if(driver_mode == 1U){
                /*Squarewave Mode*/
                if(signal_toggle){
                    PWM5_LoadDutyValue(800U);
                    PWM6_LoadDutyValue(800U);            
                }
                else{
                    PWM5_LoadDutyValue(0U);
                    PWM6_LoadDutyValue(0U);            
                }            
            }
            else{
                /*Skip*/
            }
        }  
        else{
            /*Skip*/
        }           
    }
    else{
        /*Skip*/
    }  
}

void driver_init(void){  
     __delay_ms(500); /*500ms delay to attract magnet*/
             
    driver_mode = DATAEE_ReadByte(0x7000U); /*Read Mode from FLash addres 0x7000*/
    if((driver_mode == 0U)||(driver_mode >= 7U)){
        driver_mode = 1U; /*If empty/corrupted set to SQR-Mode*/
    }
    eepdriver_mode = driver_mode;
    buzzer_pwm_config();/*Play Init Buzzer Tone*/
    
    uint16_t adc_initval = ADC_GetConversion(channel_ANA4);
    for (uint8_t i = 0; i <= 7U; i++){
        adc_avgval[adc_avgcounter] = adc_initval;
    }
        
    /*Test LEDs*/
    LATC =  LED_SQR_MODE;
    __delay_ms(30); 
    LATC =  LED_PWM_MODE;
     __delay_ms(30);
    LATC =  LED_POS_MODE;
     __delay_ms(30);
    LATC =  LED_BUZ_MODE;
     __delay_ms(30);
    LATC =  LED_EXT_MODE;
     __delay_ms(30);
     
    PWM5_LoadDutyValue(125U); /*Set tp 25%*/
    PWM6_LoadDutyValue(125U); /*Set tp 25%*/
              
    /*Set up initialization lighting sequence according to new mode*/
    switch(driver_mode){
        case 1: 
        case 7:           
            LATC =  LED_EXT_MODE;
             __delay_ms(30);
            LATC =  LED_BUZ_MODE;
             __delay_ms(30);
            LATC =  LED_POS_MODE;
             __delay_ms(30);
            LATC =  LED_PWM_MODE;
             __delay_ms(30);
            LATC =  LED_SQR_MODE;
            break;
        case 2:
            LATC =  LED_EXT_MODE;
             __delay_ms(30);
            LATC =  LED_BUZ_MODE;
             __delay_ms(30);
            LATC =  LED_POS_MODE;
             __delay_ms(30);
            LATC =  LED_PWM_MODE;
            break;
        case 3:            
        case 4:
            LATC =  LED_EXT_MODE;
             __delay_ms(30);
            LATC =  LED_BUZ_MODE;
             __delay_ms(30);
            LATC =  LED_POS_MODE;
            break;
        case 5:
            /*Enter Buzzer Mode*/
            LATC =  LED_EXT_MODE;
             __delay_ms(30);
            LATC =  LED_BUZ_MODE;
            break;
        case 6:
            LATC =  LED_EXT_MODE;               
           break;  
        default:
            break;
    }                
    driver_handler(driver_mode); /*Set up new Mode*/       
}

void buzzer_pwm_config(void){    
    T2CONbits.TMR2ON = 0; /*Disable Timer*/
    PWM6CONbits.PWM6EN = 0;/*Disable PWM5*/
    PWM5CONbits.PWM5EN = 0;/*Disable PWM6*/   
    
    T2CON = 0x07; /*Update Prescalar*/
    PR2 = 128U; /*Set to inital value*/
    PWM5_LoadDutyValue(258U); /*Set tp 50%*/
    PWM6_LoadDutyValue(258U); /*Set tp 50%*/
    
    T2CONbits.TMR2ON = 1; /*Enable Timer*/
    PWM6CONbits.PWM6EN = 1;/*Enable PWM5*/
    PWM5CONbits.PWM5EN = 1;/*Enable PWM6*/   
}

void drive_pwm_config(void){    
    TRISCbits.TRISC0 = 0U; /*Set pin to output*/
    TRISAbits.TRISA2 = 0U; /*Set pin to output*/
                   
    T2CONbits.TMR2ON = 0; /*Disable Timer*/
    PWM6CONbits.PWM6EN = 0;/*Disable PWM5*/
    PWM5CONbits.PWM5EN = 0;/*Disable PWM6*/   
    
    
    T2CON = 0x05;/*Update prescalar & set frequency to 20kHz*/
    PR2 = 0xC7;
    PWM5DCH = 0xC7; /*100% Duty Cycle*/
    PWM5DCL = 0xC0; /*100% Duty Cycle*/ 
    PWM6DCH = 0xC7; /*100% Duty Cycle*/
    PWM6DCL = 0xC0; /*100% Duty Cycle*/ 
    
    T2CONbits.TMR2ON = 1; /*Enable Timer*/
    PWM6CONbits.PWM6EN = 1;/*Enable PWM5*/
    PWM5CONbits.PWM5EN = 1;/*Enable PWM6*/   
}


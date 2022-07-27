 /**
   PWM6 Generated Driver File
 
   @Company
     Microchip Technology Inc.
 
   @File Name
     pwm6.c
 
   @Summary
     This is the generated driver implementation file for the PWM6 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs
 
   @Description
     This source file provides implementations for driver APIs for PWM6.
     Generation Information :
         Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.7
         Device            :  PIC16LF18323
         Driver Version    :  2.01
     The generated drivers are tested against the following:
         Compiler          :  XC8 2.31 and above or later
         MPLAB             :  MPLAB X 5.45
 */ 

 /*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/
 
 /**
   Section: Included Files
 */

 #include <xc.h>
 #include "pwm6.h"

 /**
   Section: PWM Module APIs
 */

 void PWM6_Initialize(void)
 {
    // Set the PWM to the options selected in the PIC10 / PIC12 / PIC16 / PIC18 MCUs.
    // PWM6POL active_hi; PWM6EN disabled; 
    PWM6CON = 0x00;   

    // PWM6DCH 199; 
    PWM6DCH = 0xC7;   

    // PWM6DCL 3; 
    PWM6DCL = 0xC0;   

 }

 void PWM6_LoadDutyValue(uint16_t dutyValue)
 {
     // Writing to 8 MSBs of PWM duty cycle in PWMDCH register
     PWM6DCH = (dutyValue & 0x03FC)>>2;
     
     // Writing to 2 LSBs of PWM duty cycle in PWMDCL register
     PWM6DCL = (dutyValue & 0x0003)<<6;
 }
 /**
  End of File
 */

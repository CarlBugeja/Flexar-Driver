/* 
 * File:   flexar_driver.h
 * Author: carlb
 *
 * Created on August 26, 2021, 1:31 PM
 */

#ifndef FLEXAR_DRIVER_H
#define	FLEXAR_DRIVER_H

#ifdef	__cplusplus
extern "C" {
#endif

void flash_handler(void);
void loop_handler(void);
void driver_handler(uint8_t mode_select);
void trimmer_handler(void);
void driver_init(void);
void driver_run(void);
void drive_pwm_config(void);
void buzzer_pwm_config(void);

#define LED_OFF_MODE 0x0U
#define LED_SQR_MODE 0x2U
#define LED_PWM_MODE 0x4U
#define LED_POS_MODE 0x8U
#define LED_BUZ_MODE 0x10U
#define LED_EXT_MODE 0x20U

#ifdef	__cplusplus
}
#endif

#endif	/* FLEXAR_DRIVER_H */


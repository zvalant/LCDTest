/*
 * ili9341_hal.h
 *
 *  Created on: Jul 21, 2025
 *      Author: zac23
 */

#ifndef INC_ILI9341_HAL_H_
#define INC_ILI9341_HAL_H_

#include "stm32h7xx_hal.h"
// ILI9341 HAL Delays

#define ILI9341_HW_RESET_DELAY_MS 10
#define ILI0341_HW_STARTUP_DELAY_MS 120

//ILI9341 Pin Mappings
#define ILI9341_DC_Pin GPIO_PIN_1
#define ILI9341_DC_GPIO_Port GPIOG
#define ILI9341_RST_Pin GPIO_PIN_12
#define ILI9341_RST_GPIO_Port GPIOG
#define ILI9341_CS_Pin GPIO_PIN_15
#define ILI9341_CS_GPIO_Port GPIOG
#define LD2_Pin GPIO_PIN_1
#define LD2_GPIO_Port GPIOE

// ILI9341 HAL functions
void ILI9341_CS_Low(void);
void ILI9341_CS_High(void);
void ILI9341_DC_Low(void);
void ILI9341_DC_High(void);
void ILI9341_Reset(void);
#endif /* INC_ILI9341_HAL_H_ */

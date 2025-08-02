/*
 * ili9341_hal.h
 *
 *  Created on: Jul 21, 2025
 *      Author: zac23
 */

#ifndef INC_ILI9341_HAL_H_
#define INC_ILI9341_HAL_H_

#include "stm32h7xx_hal.h"

void ILI9341_CS_Low(void);
void ILI9341_CS_High(void);
void ILI9341_DC_Low(void);
void ILI9341_DC_High(void);
void ILI9341_Reset(void);
#endif /* INC_ILI9341_HAL_H_ */

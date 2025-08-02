/*
 * ili9341_hal.c
 *
 *  Created on: Jul 21, 2025
 *      Author: zac23
 */
#include "ili9341_config.h"
#include "ili9341_hal.h"
#include "stm32h7xx_hal.h"

void ILI9341_CS_Low(void) {
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_RESET);
}

void ILI9341_CS_High(void) {
    HAL_GPIO_WritePin(ILI9341_CS_GPIO_Port, ILI9341_CS_Pin, GPIO_PIN_SET);
}

void ILI9341_DC_Low(void) {
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_RESET);
}

void ILI9341_DC_High(void) {
    HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_SET);
}

void ILI9341_Reset(void) {
    HAL_GPIO_WritePin(ILI9341_RST_GPIO_Port, ILI9341_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(ILI9341_RST_GPIO_Port, ILI9341_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(120);
}

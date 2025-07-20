//had to modify please reload from origional ytube video


#ifndef ILI9341_STM32_DRIVER_H
#define ILI9341_STM32_DRIVER_H

#include "stm32h7xx_hal.h"

extern SPI_HandleTypeDef hspi1;

#define ILI9341_SCREEN_HEIGHT 	240
#define ILI9341_SCREEN_WIDTH 	320

/* PIN Configuration */
#define HSPI_INSTANCE			&hspi1
#define LCD_CS_PORT				GPIOG
#define LCD_CS_PIN				GPIO_PIN_15
#define LCD_DC_PORT				GPIOG
#define LCD_DC_PIN				GPIO_PIN_1
#define	LCD_RST_PORT			GPIOG
#define	LCD_RST_PIN				GPIO_PIN_12

#define BURST_MAX_SIZE 			500
#define BLACK      				0x0000
#define NAVY        			0x000F
#define DARKGREEN   			0x03E0
#define DARKCYAN    			0x03EF
#define MAROON      			0x7800
#define PURPLE      			0x780F
#define OLIVE       			0x7BE0
#define LIGHTGREY   			0xC618
#define DARKGREY    			0x7BEF
#define BLUE        			0x001F
#define GREEN       			0x07E0
#define CYAN        			0x07FF
#define RED         			0xF800
#define MAGENTA     			0xF81F
#define YELLOW      			0xFFE0
#define WHITE       			0xFFFF
#define ORANGE      			0xFD20
#define GREENYELLOW 			0xAFE5
#define PINK        			0xF81F

#define SCREEN_VERTICAL_1		0
#define SCREEN_HORIZONTAL_1		1
#define SCREEN_VERTICAL_2		2
#define SCREEN_HORIZONTAL_2		3

#endif

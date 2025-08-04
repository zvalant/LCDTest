/*
 * gfx.h
 *
 *  Created on: Jul 27, 2025
 *      Author: zac23
 */

#ifndef INC_ILI9341_GFX_H_
#define INC_ILI9341_GFX_H_

#include <stdint.h>


#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_WHITE   0xFFFF
#define COLOR_BLACK   0x0000


void ILI9341_FillScreen(uint16_t color, uint8_t rowCounter);
void ILI9341_RGB565FillScreen(uint16_t color);





#endif /* INC_ILI9341_GFX_H_ */

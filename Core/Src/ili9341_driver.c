/*
 * ili9341_driver.c
 *
 *  Created on: Jul 21, 2025
 *      Author: zac23
 */
#include "stm32h7xx_hal.h"
#include "ili9341_driver.h"
#include "ili9341_hal.h"
#include <stdbool.h>
#include <math.h>

extern SPI_HandleTypeDef ILI9341_SPI_HANDLE;

struct ili9341_driver {
	volatile uint8_t activeDMATransmissions;

};

static struct ili9341_driver this = { 0 };
ILI9341_Driver_Result_t ILI9341_Write_Command(uint8_t cmd) {
	ILI9341_CS_Low();
	ILI9341_DC_Low();  // Command mode
	HAL_StatusTypeDef result = HAL_SPI_Transmit(&hspi1, &cmd, 1,
	HAL_MAX_DELAY);
	ILI9341_CS_High();
	return (result == HAL_OK) ? ILI9341_DRIVER_OK : ILI9341_DRIVER_COMMAND_ERROR;
}
;

ILI9341_Driver_Result_t ILI9341_Write_Data8(uint8_t data) {
	ILI9341_CS_Low();
	ILI9341_DC_High(); // Data mode
	HAL_StatusTypeDef result = HAL_SPI_Transmit(&hspi1, &data, 1,
	HAL_MAX_DELAY);
	ILI9341_CS_High();
	return (result == HAL_OK) ? ILI9341_DRIVER_OK : ILI9341_DRIVER_DATA8_ERROR;

}

ILI9341_Driver_Result_t ILI9341_Write_Data16(uint16_t data) {
	uint8_t buffer[2];
	buffer[0] = data >> 8;    // High byte
	buffer[1] = data & 0xFF;  // Low byte
	ILI9341_CS_Low();
	ILI9341_DC_High();
	HAL_StatusTypeDef result = HAL_SPI_Transmit(&hspi1, buffer, 2,
	HAL_MAX_DELAY);
	ILI9341_CS_High();
	return (result == HAL_OK) ? ILI9341_DRIVER_OK : ILI9341_DATA16_ERROR;
}

ILI9341_Driver_Result_t ILI9341_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1,
		uint16_t y1) {
	// Column address

	if (x0
			>= x1|| y0>=y1 || x1 >ILI9341_WIDTH_PIXELS || y1 > ILI9341_HEIGHT_PIXELS) {
		return ILI9341_DRIVER_WINDOW_ERROR;
	}
	ILI9341_Driver_Result_t result;

	// Set column address
	if ((result = ILI9341_Write_Command(ILI9341_CASET)) != ILI9341_DRIVER_OK)
		return result;
	if ((result = ILI9341_Write_Data16(x0)) != ILI9341_DRIVER_OK)
		return result;
	if ((result = ILI9341_Write_Data16(x1)) != ILI9341_DRIVER_OK)
		return result;

	// Set page address
	if ((result = ILI9341_Write_Command(ILI9341_PASET)) != ILI9341_DRIVER_OK)
		return result;
	if ((result = ILI9341_Write_Data16(y0)) != ILI9341_DRIVER_OK)
		return result;
	if ((result = ILI9341_Write_Data16(y1)) != ILI9341_DRIVER_OK)
		return result;

	// Memory write command
	return ILI9341_Write_Command(ILI9341_RAMWR);
}

ILI9341_Driver_Result_t ILI9341_Driver_Init(void) {
	ILI9341_Reset();

	// Software reset
	ILI9341_Write_Command(ILI9341_SWRESET);
	HAL_Delay(ILI9341_RESET_STARTUP_DURATION_MS);

	// Sleep out
	ILI9341_Write_Command(ILI9341_SLPOUT);
	HAL_Delay(ILI9341_SLEEP_STARTUP_DURATION_MS);
	ILI9341_Write_Command(ILI9341_PXLFRMT);
	ILI9341_Write_Data8(ILI9341_PIXEL_FORMAT_VALUE);
	ILI9341_Write_Command(ILI9341_RGBORDER);
	ILI9341_Write_Data8(ILI9341_RGB_ORDER_VALUE);

	// Display on
	ILI9341_Write_Command(ILI9341_DISPON);
	HAL_Delay(ILI9341_DISPLAY_DELAY_MS);
	return ILI9341_DRIVER_OK;

}

ILI9341_Driver_Result_t ILI9341_Transmit_Frame(const uint8_t *frameBuffer) {

	if (frameBuffer == NULL) {
		return ILI9341_DRIVER_ERROR;
	}
	if (ILI9341_SetWindow(0, 0, ILI9341_WIDTH_PIXELS - 1,
	ILI9341_HEIGHT_PIXELS - 1) != ILI9341_DRIVER_OK) {
		return ILI9341_DRIVER_ERROR;
	};
	while (this.activeDMATransmissions > 0)
		;
	ILI9341_CS_Low();
	ILI9341_DC_High();
	uint32_t remainingBytes = ILI9341_FRAME_BUFFER_SIZE_BYTES;
	uint32_t offset = 0;

	while (remainingBytes > 0) {

		__disable_irq();
		this.activeDMATransmissions++;
		uint32_t transmitLength = fmin(ILI9341_DMA_CHUNK_SIZE_BYTES,
				remainingBytes);
		HAL_StatusTypeDef result = HAL_SPI_Transmit_DMA(&ILI9341_SPI_HANDLE,
				(uint8_t*) &frameBuffer[offset], transmitLength);
		if (result != HAL_OK) {
			this.activeDMATransmissions--;
			__enable_irq();
			ILI9341_CS_High();
			return ILI9341_DRIVER_ERROR;
		}
		__enable_irq();
		while (this.activeDMATransmissions > 0)
			;  //SPI DMA can only handle 1 transmission at a time
		offset = offset + transmitLength;
		remainingBytes = remainingBytes - transmitLength;
	}
	ILI9341_CS_High();
	//slight delay for static frame to stabilize
	HAL_Delay(2);

	return (ILI9341_DRIVER_OK);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == SPI1) {
		__disable_irq();
		if (this.activeDMATransmissions > 0) {
			this.activeDMATransmissions--;
		}
		__enable_irq();
	}
}

void Test_SPIData(void) {
	uint8_t test_data = 0xFF;

	for (int i = 0; i < SPI_TEST_COUNT; i++) {
		ILI9341_CS_Low();
		ILI9341_DC_High();
		HAL_SPI_Transmit(&hspi1, &test_data, 1, HAL_MAX_DELAY);
		ILI9341_CS_High();
		HAL_Delay(TEST_DELAY);
	}
}

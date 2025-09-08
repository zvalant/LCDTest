/*
 * ili9341_driver.c
 *
 *  Created on: Jul 21, 2025
 *      Author: zac23
 */
#include "stm32h7xx_hal.h"
#include "ili9341_driver.h"
#include "ili9341_config.h"
#include "ili9341_hal.h"
#include <stdbool.h>

extern SPI_HandleTypeDef ILI9341_SPI_HANDLE;
static volatile bool DMA_Transfer_Complete = false;
static volatile bool s_dma_transfer_complete = true;

static ILI9341_Result_t ILI9341_WaitDMAComplete(uint32_t timeout_ms);
static void ILI9341_InitSequence(void);

ILI9341_Result_t ILI9341_WriteCommand(uint8_t cmd) {
    ILI9341_CS_Low();
    ILI9341_DC_Low();  // Command mode
    HAL_Delay(1);
    HAL_StatusTypeDef response = HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    ILI9341_CS_High();
    return (response ==HAL_OK)? ILI9341_OK: ILI9341_ERROR;
};


ILI9341_Result_t ILI9341_WriteData(uint8_t data) {
    ILI9341_CS_Low();
    ILI9341_DC_High(); // Data mode
    HAL_Delay(1);
    HAL_StatusTypeDef response = HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    ILI9341_CS_High();
    return (response == HAL_OK)? ILI9341_OK: ILI9341_ERROR;

}

ILI9341_Result_t ILI9341_WriteData16(uint16_t data) {
    uint8_t buffer[2];
    buffer[0] = data >> 8;    // High byte
    buffer[1] = data & 0xFF;  // Low byte
    ILI9341_CS_Low();
    ILI9341_DC_High();
    HAL_StatusTypeDef response = HAL_SPI_Transmit(&hspi1, buffer, 2, HAL_MAX_DELAY);
    ILI9341_CS_High();
    return (response == HAL_OK)? ILI9341_OK: ILI9341_ERROR;
}

ILI9341_Result_t ILI9341_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Column address

	if(x0>=x1 || y0>=y1||x1 >ILI9341_WIDTH|| y1 > ILI9341_HEIGHT){
		return ILI9341_ERROR;
	}
	ILI9341_Result_t setWindowResponse = ILI9341_OK;
    ILI9341_WriteCommand(ILI9341_CASET);
    ILI9341_WriteData16(x0);
    ILI9341_WriteData16(x1);

    // Page address
    ILI9341_WriteCommand(ILI9341_PASET);
    ILI9341_WriteData16(y0);
    ILI9341_WriteData16(y1);

    // Memory write 2 frameBuffer
    ILI9341_WriteCommand(ILI9341_RAMWR);
    return setWindowResponse;
}

ILI9341_Result_t ILI9341_Init(void) {
    ILI9341_Reset();

    // Software reset
    ILI9341_WriteCommand(ILI9341_SWRESET);
    HAL_Delay(150);

    // Sleep out
    ILI9341_WriteCommand(ILI9341_SLPOUT);
    HAL_Delay(120);
    ILI9341_WriteCommand(ILI9341_PXLFRMT);
    ILI9341_WriteData(0x55);
    ILI9341_WriteCommand(ILI9341_RGBORDER);
    ILI9341_WriteData(0x08);
    ILI9341_WriteCommand(0x35);

    // Display on
    ILI9341_WriteCommand(ILI9341_DISPON);
    HAL_Delay(120);
    return ILI9341_OK;

}



ILI9341_Result_t TransmitFrame(const uint8_t* frameBuffer){

	ILI9341_Result_t transmitFrameResponse = ILI9341_OK;
	if(frameBuffer==NULL){
		return ILI9341_ERROR;
	}
	transmitFrameResponse = ILI9341_SetWindow(0, 0,ILI9341_WIDTH-1, ILI9341_HEIGHT-1);
	ILI9341_CS_Low();
	ILI9341_DC_High();
	HAL_StatusTypeDef HAL_Response = HAL_OK;
    for (int chunk = 0; chunk <ILI9341_DMA_CHUNK_COUNT; chunk++){
    	uint32_t offset = chunk* ILI9341_DMA_CHUNK_SIZE;

    	HAL_Response = HAL_SPI_Transmit_DMA(&ILI9341_SPI_HANDLE,(uint8_t*)&frameBuffer[offset], ILI9341_DMA_CHUNK_SIZE);
        while(!DMA_Transfer_Complete);
        DMA_Transfer_Complete = false;
    }
    transmitFrameResponse = TransmitFrameRemainder(frameBuffer);

	ILI9341_CS_High();
	//slight delay for static frame to stabilize
	HAL_Delay(2);

	return (transmitFrameResponse ==ILI9341_OK && HAL_Response==HAL_OK)? ILI9341_OK: ILI9341_ERROR;
}


void Test_SPI_Data(void) {
    uint8_t test_data = 0xFF;

    for(int i = 0; i < 20; i++) {
        ILI9341_CS_Low();
        ILI9341_DC_High();
        HAL_SPI_Transmit(&hspi1, &test_data, 1, HAL_MAX_DELAY);
        ILI9341_CS_High();
        HAL_Delay(200);
    }
}

ILI9341_Result_t TransmitFrameRemainder (const uint8_t* frameBuffer){
	uint32_t offset = ILI9341_DMA_CHUNK_SIZE*ILI9341_DMA_CHUNK_COUNT;
	uint32_t remainder = ILI9341_FRAME_BUFFER_SIZE%ILI9341_DMA_CHUNK_SIZE;
	HAL_StatusTypeDef HAL_Response = HAL_OK;
	if (remainder==0){
		return ILI9341_OK;
	}else{
		HAL_Response = HAL_SPI_Transmit_DMA(&ILI9341_SPI_HANDLE,(uint8_t*)&frameBuffer[offset],remainder);
		while(!DMA_Transfer_Complete);
		DMA_Transfer_Complete = false;

	}
	return (HAL_Response==HAL_OK) ? ILI9341_OK:ILI9341_ERROR;

}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if(hspi->Instance == SPI1) {
    	DMA_Transfer_Complete = true;
    }
}

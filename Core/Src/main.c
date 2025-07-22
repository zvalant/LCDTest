/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
//#include "ILI9341_STM32_Driver.h"
//#include "ILI9341_GFX.h"




SPI_HandleTypeDef hspi1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);

//globals
volatile bool DMA_Transfer_Complete = false;



// Low-level functions
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

void ILI9341_WriteCommand(uint8_t cmd) {
    ILI9341_CS_Low();
    ILI9341_DC_Low();  // Command mode
    HAL_Delay(1);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    ILI9341_CS_High();
}

void ILI9341_WriteData(uint8_t data) {
    ILI9341_CS_Low();
    ILI9341_DC_High(); // Data mode
    HAL_Delay(1);
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    HAL_Delay(1);
    ILI9341_CS_High();
}

void ILI9341_WriteData16(uint16_t data) {
    uint8_t buffer[2];
    buffer[0] = data >> 8;    // High byte
    buffer[1] = data & 0xFF;  // Low byte
    ILI9341_CS_Low();
    ILI9341_DC_High();
    HAL_SPI_Transmit(&hspi1, buffer, 2, HAL_MAX_DELAY);
}
void ILI9341_WritePixelData(int i , uint8_t r, uint8_t g, uint8_t b, uint8_t* frameBuffer){
	uint16_t data = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);
	uint8_t buffer[2];
	uint8_t dataBit1 = data>>8;
	uint8_t dataBit2 = data&0xFF;
	frameBuffer[2*i] = dataBit1;
	frameBuffer[(2*i)+1] = dataBit2;



}



void ILI9341_Init(void) {
    ILI9341_Reset();

    // Software reset
    ILI9341_WriteCommand(ILI9341_SWRESET);
    HAL_Delay(150);

    // Sleep out
    ILI9341_WriteCommand(ILI9341_SLPOUT);
    HAL_Delay(120);
    ILI9341_WriteCommand(0x3A);
    ILI9341_WriteData(0x55);
    ILI9341_WriteCommand(0x36);
    ILI9341_WriteData(0x08);

    // Display on
    ILI9341_WriteCommand(ILI9341_DISPON);
    HAL_Delay(120);

}

void ILI9341_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Column address set
    ILI9341_WriteCommand(ILI9341_CASET);
    ILI9341_WriteData16(x0);
    ILI9341_WriteData16(x1);

    // Page address set
    ILI9341_WriteCommand(ILI9341_PASET);
    ILI9341_WriteData16(y0);
    ILI9341_WriteData16(y1);

    // Memory write
    ILI9341_WriteCommand(ILI9341_RAMWR);
}

void ILI9341_FillScreen(uint16_t color, uint8_t rowCounter) {

    uint8_t color1 = 255;
    uint8_t color2 = 0;
    uint8_t color3 = 0;
    uint8_t frameBuffer[153600];
    int section = 51200;
    int sectionCount = 153600/section;

    for(int i = 0; i < 76800; i++) {
    	if(i%1920==0){
    		rowCounter++;
    		switch(rowCounter%3){
    		case 0:
    			color1 = 255;
    			color2 = 127;
    			color3 = 80;
    			break;
    		case 1:
    			color1 = 46;
    			color2 = 139;
    			color3 = 87;
    			break;
    		case 2:
    			color1 = 255;
    			color2 = 245;
    			color3 = 238;
    			break;
    		default:
    			break;
    		}

    	}
    	//ILI9341_WriteData16(color);

        ILI9341_WritePixelData(i, color1,color2,color3, frameBuffer);

    }// 240 * 320 pixels
    ILI9341_SetWindow(0, 0, 239, 319);
	ILI9341_CS_Low();
	ILI9341_DC_High();
    for (int i = 0; i <sectionCount;i++){
    	HAL_SPI_Transmit_DMA(&hspi1, &frameBuffer[i*section], section);
        while(!DMA_Transfer_Complete);
        DMA_Transfer_Complete = false;
    }

	ILI9341_CS_High();
	HAL_Delay(100);
}

void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if(x >= 240 || y >= 320) return;

    ILI9341_SetWindow(x, y, x, y);
    ILI9341_WriteData16(color);
}

// Simple test pattern
void ILI9341_TestPattern(void) {
    // Fill screen with different colors to test
	int frameCounter = 0;
	while(1){
		frameCounter++;
	    ILI9341_FillScreen(COLOR_RED, frameCounter);


	}
    HAL_Delay(1000);

    ILI9341_FillScreen(COLOR_GREEN, 0);
    HAL_Delay(1000);

    ILI9341_FillScreen(COLOR_BLUE, 0);
    HAL_Delay(1000);

    ILI9341_FillScreen(COLOR_WHITE, 0);
    HAL_Delay(1000);

    // Draw simple pattern
    ILI9341_FillScreen(COLOR_BLACK, 0);

    // Draw colored squares
    for(int x = 0; x < 240; x += 40) {
        for(int y = 0; y < 320; y += 40) {
            uint16_t color = (x + y) % 120 == 0 ? COLOR_RED :
                           (x + y) % 80 == 0 ? COLOR_GREEN : COLOR_BLUE;

            for(int i = 0; i < 30; i++) {
                for(int j = 0; j < 30; j++) {
                    ILI9341_DrawPixel(x + i, y + j, color);
                }
            }
        }
    }
}

// Simple test to verify SPI data transmission
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

// Test PD7 GPIO functionality before SPI
void Test_PD7_GPIO(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOD_CLK_ENABLE();

    // Configure PD7 as regular GPIO output for testing
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    // Toggle PD7 10 times
    for(int i = 0; i < 20; i++) {
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_7);
        HAL_Delay(10);
    }

    // Set back to low
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
}
void SPITest(void){
	ILI9341_CS_Low();
	ILI9341_DC_High();

}
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if(hspi->Instance == SPI1) {
    	DMA_Transfer_Complete = true;  // YOU set your own flag
    }
}
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

COM_InitTypeDef BspCOMInit;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  // First test raw SPI data transmission


  // Initialize display
  ILI9341_Init();
  Test_SPI_Data();



  // Run test pattern
  while(1) {
      ILI9341_TestPattern();
      HAL_Delay(1000);
  }
  /*
   *
  ILI9341_Init();

  // Simple Text writing (Text, Font, X, Y, Color, BackColor)
  // Available Fonts are FONT1, FONT2, FONT3 and FONT4
  ILI9341_FillScreen(WHITE);
  ILI9341_SetRotation(SCREEN_HORIZONTAL_2);
  ILI9341_DrawText("HELLO WORLD", FONT4, 90, 110, BLACK, WHITE);
  HAL_Delay(1000);

  //Writing numbers
  ILI9341_FillScreen(WHITE);
  static char BufferText[30];
  for(uint8_t i = 0; i <= 5; i++){
    sprintf(BufferText, "COUNT : %d", i);
    ILI9341_DrawText(BufferText, FONT3, 10, 10, BLACK, WHITE);
    ILI9341_DrawText(BufferText, FONT3, 10, 30, BLUE, WHITE);
    ILI9341_DrawText(BufferText, FONT3, 10, 50, RED, WHITE);
    ILI9341_DrawText(BufferText, FONT3, 10, 70, GREEN, WHITE);
    ILI9341_DrawText(BufferText, FONT3, 10, 90, YELLOW, WHITE);
    ILI9341_DrawText(BufferText, FONT3, 10, 110, PURPLE, WHITE);
    ILI9341_DrawText(BufferText, FONT3, 10, 130, ORANGE, WHITE);
    ILI9341_DrawText(BufferText, FONT3, 10, 150, MAROON, WHITE);
    ILI9341_DrawText(BufferText, FONT3, 10, 170, WHITE, BLACK);
    ILI9341_DrawText(BufferText, FONT3, 10, 190, BLUE, BLACK);
  }

  // Horizontal Line (X, Y, Length, Color)
  ILI9341_FillScreen(WHITE);
  ILI9341_DrawHLine(50, 120, 200, NAVY);
  HAL_Delay(1000);

  // Vertical Line (X, Y, Length, Color)
  ILI9341_FillScreen(WHITE);
  ILI9341_DrawVLine(160, 40, 150, DARKGREEN);
  HAL_Delay(1000);

  // Hollow Circle (Centre X, Centre Y, Radius, Color)
  ILI9341_FillScreen(WHITE);
  ILI9341_DrawHollowCircle(160, 120, 80, PINK);
  HAL_Delay(1000);

  // Filled Circle (Centre X, Centre Y, Radius, Color)
  ILI9341_FillScreen(WHITE);
  ILI9341_DrawFilledCircle(160, 120, 50, CYAN);
  HAL_Delay(1000);

  // Filled Rectangle (Start X, Start Y, Length X, Length Y)
  ILI9341_FillScreen(WHITE);
  ILI9341_DrawRectangle(50, 50, 220, 140, GREENYELLOW);
  HAL_Delay(1000);

  // Hollow Rectangle (Start X, Start Y, End X, End Y)
  ILI9341_FillScreen(WHITE);
  ILI9341_DrawHollowRectangleCoord(50, 50, 270, 190, DARKCYAN);
  HAL_Delay(1000);

  // Simple Pixel Only (X, Y, Color)
  ILI9341_FillScreen(WHITE);
  ILI9341_DrawPixel(100, 100, BLACK);
  HAL_Delay(1000);
*/
  /* USER CODE END 2 */

  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_RED);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = 4;
  RCC_OscInitStruct.PLL.PLLQ = 6;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES_TXONLY;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */
  __HAL_SPI_ENABLE(&hspi1);
  /* USER CODE END SPI1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ILI9341_DC_GPIO_Port, ILI9341_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, ILI9341_RST_Pin|ILI9341_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : ILI9341_DC_Pin ILI9341_RST_Pin ILI9341_CS_Pin */
  GPIO_InitStruct.Pin = ILI9341_DC_Pin|ILI9341_RST_Pin|ILI9341_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

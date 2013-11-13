/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    22-March-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_julia.h"
#include "string.h"

/** @addtogroup Fractal_Project
  * @{
  */

/** @addtogroup Fractal_Project_MAIN
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define  SCREEN_X_SIZE               ((uint16_t)320)
#define  SCREEN_Y_SIZE               ((uint8_t)208)
#define  PBAR_X_POS                  ((uint16_t)0x100)
#define  PBAR_Y_POS_H                ((uint8_t)0)
#define  PBAR_Y_POS_L                ((uint8_t)227)
#define  PBAR_COLOR                  ((uint16_t)0xf79e)
#define  ANIMATION_LENGHT            ((uint32_t)26)

#define SUBMODE_FPU_USED_MODE          ((uint8_t)0)
#define SUBMODE_FPU_NOT_USED_MODE      ((uint8_t)1)


#if (__FPU_USED == 1)

#define SCORE_FPU_MODE                  "FPU On"
#define PROGRESS_FPU_MODE               "FPU ON"

#else

#define SCORE_FPU_MODE                  "FPU Off"
#define PROGRESS_FPU_MODE               "FPU OFF"

#endif  /* __FPU_USED */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const int16_t animation_zoom[ANIMATION_LENGHT] =
  {
    120, 110, 100, 150, 200, 275, 350, 450,
    600, 800, 1000, 1200, 1500, 2000, 1500,
    1200, 1000, 800, 600, 450, 350, 275, 200,
    150, 100, 110
  };
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
uint8_t buffer[SCREEN_X_SIZE * SCREEN_Y_SIZE];
uint8_t text[50];
__IO FlagStatus key_pressed;
__IO FlagStatus switch_mode;

/* Private function prototypes -----------------------------------------------*/
void InitLCD(void);
void SetPBLCD(uint32_t position);
void PreparePBLCD(uint32_t position);
void UpdateLCD(uint8_t * image, uint16_t * clut, uint8_t * message);
void InitCLUT(uint16_t * clut);
void InitTIMER(void);
void StartTIMER(void);
uint32_t StopTIMER(void);
/* Private functions ---------------------------------------------------------*/


/** @defgroup Main_PublicFunction main program function
 *  @brief    Main funtions that builds example body
 *
@verbatim
 ===============================================================================
                 Running Fractal & execution time measurement
 ===============================================================================
@endverbatim
  * @{
  */

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
  /* Color palette */
  uint16_t        color[ITERATION];
  /* Modes */
  FlagStatus  auto_mode;
  FlagStatus  start_compute;

  /* Animation pointer */
  uint8_t   animation_pointer;
  /* Benchmark result */
  uint32_t  score_fpu;

  /* Initialise global variables */
  switch_mode = RESET;
  animation_pointer = 0;
  /* Initialise local variables */
  score_fpu = 0;

  /* Start in non automatic mode with FPU used */
  auto_mode = RESET;
  start_compute = RESET;

  /* LCD & port initialization */
  InitLCD();
  InitTIMER();
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_EXTI);
  STM_EVAL_PBInit(BUTTON_WAKEUP, BUTTON_MODE_EXTI);

  /* Generate the CLUT to be used for the fractal */
  InitCLUT(color);

  /* Clear Screen */
  memset(buffer,0x00,sizeof(buffer));
  UpdateLCD(buffer, color, "");

  /* Start generating the fractal */
  while (1)
  {
    /* Prepare LCD screen for progress bar*/
    PreparePBLCD(PBAR_Y_POS_L);
    LCD_SetTextColor(Red);

    LCD_DisplayStringLine(PBAR_Y_POS_L, PROGRESS_FPU_MODE);

    SetPBLCD(PBAR_Y_POS_L);

    /* Start the timer */
    StartTIMER();

    /* Make the calculation */
    GenerateJulia_fpu(SCREEN_X_SIZE,
	              SCREEN_Y_SIZE,
	              SCREEN_X_SIZE / 2,
	              SCREEN_Y_SIZE / 2,
	              animation_zoom[animation_pointer],
	              buffer);

      /* Get elapsed time */
      score_fpu = StopTIMER();
      if ((auto_mode != SET) )
      {
        sprintf((char*)text, "%s : %dms", SCORE_FPU_MODE, score_fpu >> 2);
      }

    /* Update LCD screen */
    LCD_SetTextColor(White);
    UpdateLCD(buffer, color, text);

    /* Wait for a key to be pressed */
    start_compute = RESET;
    if (auto_mode == RESET)
    {
      /* key pressed only allowed during while loop in non auto mode */
      key_pressed = RESET;
    }

    while (start_compute == RESET)
    {
      /* If Wakeup button is or has been pressed */
      if (switch_mode == SET)
      {
        /* Toggle mode */
        auto_mode = (FlagStatus)(1 - (uint8_t)auto_mode);
        switch_mode = RESET;
      }

      /* If automatic mode : launch immediately the computation */
      if (auto_mode == SET)
      {
        animation_pointer++;
        if (animation_pointer == ANIMATION_LENGHT)
        {
          animation_pointer = 0;
        }

        start_compute = SET;
      }
      /* If not automatic mode, wait for key to be pressed */
      else
      {
        start_compute = key_pressed;
      }
    }
    if (auto_mode == RESET)
    {
      animation_pointer++;
      if (animation_pointer == ANIMATION_LENGHT)
      {
        animation_pointer = 0;
      }
    }
  }
}

/**
  * @brief  Key Button handler
  * @param  None
  * @retval None
  */
void Key_Button_Handler(void)
{
  key_pressed = SET;
}

/**
  * @brief  Wakeup Button handler
  * @param  None
  * @retval None
  */
void Wakeup_Button_Handler(void)
{
  switch_mode = SET;
}

/** @defgroup Main_PrivateFunction Display and time measurement function
 *  @brief    LCD initialization and update funtions
 *
@verbatim
 ===============================================================================
                 LCD Initialization & Update and timer initialization
 ===============================================================================
@endverbatim
  * @{
  */

/**
  * @brief  Initializes the LCD for Display
  * @param  None
  * @retval None
  */
void InitLCD(void)
{
  /* Initialize the LCD */
  STM324xG_LCD_Init();

  /* Define the font */
  LCD_SetFont(&Font8x12);

  /* Set the LCD Text Color */
  LCD_SetTextColor(White);

  /* Clear the LCD */
  LCD_Clear(Black);

  /* Set the LCD Back Color */
  LCD_SetBackColor(Black);
}

/**
  * @brief  Prepares the progress bar on LCD
  * @param  position: vertical position of progress bar
  * @retval None
  */
void PreparePBLCD(uint32_t position)
{
  /* Local variable */
  uint32_t x = 0x00, y = 0x00;

  /* Clear the section */
  LCD_SetCursor(position - 1, PBAR_X_POS + 1);
  LCD_WriteRAM_Prepare();
  for (y = 0; y < 0xF; y++)
  {
    for (x = 0; x < SCREEN_X_SIZE; x++)
    {
      LCD_WriteRAM(Black);
    }
  }

  /* Draw progress bar */
  LCD_SetCursor(position + 4, PBAR_X_POS + 1);
  LCD_WriteRAM_Prepare();
  for (x = 0; x < SCREEN_Y_SIZE + 2; x++)
  {
    LCD_WriteRAM(PBAR_COLOR);
  }

  LCD_SetCursor(position + 5, PBAR_X_POS + 1);
  LCD_WriteRAM_Prepare();
  LCD_WriteRAM(PBAR_COLOR);
  for (x = 0; x < SCREEN_Y_SIZE; x++)
  {
    LCD_WriteRAM(Black);
  }
  LCD_WriteRAM(PBAR_COLOR);

  LCD_SetCursor(position + 6, PBAR_X_POS + 1);
  LCD_WriteRAM_Prepare();
  for (x = 0; x < SCREEN_Y_SIZE + 2; x++)
  {
    LCD_WriteRAM(PBAR_COLOR);
  }

}

/**
  * @brief  Prepares to increment progress bar
  * @param  position: vertical position of progress bar
  * @retval None
  */
void SetPBLCD(uint32_t position)
{
  LCD_SetCursor(position + 5, PBAR_X_POS);
  LCD_WriteRAM_Prepare();
}

/**
  * @brief  Displays new picture on LCD
  * @param  image: image array to display
  * @param  clut: pointer to color map
  * @param  message: Text string to display
  * @retval None
  */
void UpdateLCD(uint8_t * image, uint16_t * clut, uint8_t * message)
{
  /* Local variable */
  uint32_t x = 0x00, y = 0x00;

  /* Update LCD screen */
  LCD_SetCursor(0x0, 0x13F);
  LCD_WriteRAM_Prepare();
  /* Clear the header */
  for (y = 0; y < 0xF; y++)
  {
    for (x = 0; x < SCREEN_X_SIZE; x++)
    {
      LCD_WriteRAM(Black);
    }
  }
  /* Trace the separation line */
  for (x = 0; x < SCREEN_X_SIZE; x++)
  {
    LCD_WriteRAM(White);
  }
  /* Display the picture */
  for (y = 0; y < SCREEN_Y_SIZE; y++)
  {
    for (x = 0; x < SCREEN_X_SIZE; x++)
    {
      LCD_WriteRAM(clut[image[x+y*SCREEN_X_SIZE]]);
    }
  }
  /* Trace the bottom line */
  for (x = 0; x < SCREEN_X_SIZE; x++)
  {
    LCD_WriteRAM(White);
  }

  /* Clear the bottom section */
  for (y = 0; y < 0xF; y++)
  {
    for (x = 0; x < SCREEN_X_SIZE; x++)
    {
      LCD_WriteRAM(Black);
    }
  }

  /* Display the result  */
  LCD_DisplayStringLine(PBAR_Y_POS_H, message);
}

/**
  * @brief  Initializes memory map
  * @param  clut: pointer to memory map
  * @retval None
  */
void InitCLUT(uint16_t * clut)
{
  /* Local variables */
  uint32_t       i = 0x00;
  uint16_t  red = 0, green = 0, blue = 0;

  /* Color map generation */
  for (i = 0; i < ITERATION; i++)
  {
    /* Generate red, green and blue values */
    red = (i * 8 * 256 / ITERATION) % 256;
    green = (i * 6 * 256 / ITERATION) % 256;
    blue = (i * 4 * 256 / ITERATION) % 256;

    /* Reduce to RGB656 */
    red = red >> 3;
    red = red << 11;
    green = green >> 2;
    green = green << 5;
    blue = blue >> 3;

    /* Store the 16-bit value */
    clut[i] = red + green + blue;
  }
}

/**
  * @brief  Initializes Timer for execution time measurement
  * @param  None
  * @retval None
  */
void InitTIMER(void)
{
  RCC_ClocksTypeDef RCC_Clocks;

  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  RCC_GetClocksFreq(&RCC_Clocks);

  /* DeInit TIM1 */
  TIM_DeInit(TIM1);

  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = ((RCC_Clocks.PCLK1_Frequency / 1000));
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
}

/**
  * @brief  Resets timer counter and start counting
  * @param  None
  * @retval None
  */
void StartTIMER(void)
{
  /* Disable TIM1 counter */
  TIM_Cmd(TIM1, DISABLE);

  /* Initialize counter */
  TIM1->CNT = 0x0000;

  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);
}

/**
  * @brief  Stops timer count and returns counter value
  * @param  None
  * @retval Counter Value
  */
uint32_t StopTIMER(void)
{
  /* TIM1 counter enable */
  TIM_Cmd(TIM1, DISABLE);

  /* Return counter value */
  return (uint32_t)TIM1->CNT;
}

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

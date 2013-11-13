/**
  ******************************************************************************
  * @file    stm32f4xx_julia_fpu.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    22-March-2012
  * @brief   Julia calculation with fpu
  *          Only compiler option to select HW/SW FPU.
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
#include "stm32f4xx_julia.h"

/** @addtogroup Fractal_Project
  * @{
  */

/** @addtogroup Fractal_Project_JULIA
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#ifdef USE_STM324xG_EVAL
	#define LCD_RAM_BASEADDRESS    ((__IO uint16_t *)0x68000002)
	#define LCD_COLOR_RED            0xF800
#endif /* USE_STM324xG_EVAL */
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Julia Calculation with FPU option disable
  * @param  size_x: screen width in pixel
  * @param  size_y: screen height in pixel
  * @param  offset_x: x offset for starting point
  * @param  offset_y: y offset for starting point
  * @param  zoom: Iteration zoom ratio
  * @param  buffer: pointer to output buffer where values are stored
  * @retval None
  */
void GenerateJulia_fpu(uint16_t size_x, uint16_t size_y, uint16_t offset_x, uint16_t offset_y, uint16_t zoom, uint8_t * buffer)
{
  float       tmp1, tmp2;
  float       num_real, num_img;
  float       rayon;

  uint8_t       i;
  uint16_t        x, y;

  for (y = 0; y < size_y; y++)
  {
    for (x = 0; x < size_x; x++)
    {
      num_real = y - offset_y;
      num_real = num_real / zoom;
      num_img = x - offset_x;
      num_img = num_img / zoom;
      i = 0;
      rayon = 0;
      while ((i < ITERATION - 1) && (rayon < 4))
      {
        tmp1 = num_real * num_real;
        tmp2 = num_img * num_img;
        num_img = 2 * num_real * num_img + IMG_CONSTANT;
        num_real = tmp1 - tmp2 + REAL_CONSTANT;
        rayon = tmp1 + tmp2;
        i++;
      }
      /* Store the value in the buffer */
      buffer[x+y*size_x] = i;
    }
#ifdef USE_STM324xG_EVAL
    /* Progress bar update */
    *(LCD_RAM_BASEADDRESS) = LCD_COLOR_RED;
  }
#endif /* USE_STM324xG_EVAL */
}

/**
  * @}
  */
/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/**
  ******************************************************************************
  * @file    stm32f4xx_julia.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    22-March-2012
  * @brief   Common header for Julia calculation
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_JULIA_H
#define __STM32F4xx_JULIA_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/** @addtogroup Fractal_Project
  * @{
  */

/** @addtogroup Fractal_Project_JULIA
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define  ITERATION      	((uint32_t)128)
#define	 REAL_CONSTANT		(0.285f)
#define	 IMG_CONSTANT		(0.01f)
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void GenerateJulia_fpu(uint16_t size_x,
                       uint16_t size_y,
					   uint16_t offset_x,
					   uint16_t offset_y,
					   uint16_t zoom,
					   uint8_t * buffer);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_JULIA_H */

/**
  * @}
  */
/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

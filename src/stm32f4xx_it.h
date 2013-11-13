/** @file    stm32f4xx_it.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file contains the headers of the interrupt handlers.
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#ifdef UNDEF
#include "usb_conf.h"
#endif /* UNDEF */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef UNDEF
#ifdef USE_USB_OTG_FS
void OTG_FS_WKUP_IRQHandler(void);
void OTG_FS_IRQHandler(void);
#endif

#ifdef USE_USB_OTG_HS
void OTG_HS_WKUP_IRQHandler(void);
void OTG_HS_IRQHandler(void);
#endif
#endif /* UNDEF */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */

/** @file    stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides all exceptions handler and peripherals interrupt
  *          service routine.
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#ifdef UNDEF
#include "usb_core.h"
#include "usbd_core.h"
#include "usbd_cdc_core.h"
#include "lcd_log.h"
#include "stm32f4_discovery_lcd.h"
//#include "lcd_log.h"

extern USB_OTG_CORE_HANDLE           USB_OTG_FS_dev;
extern USB_OTG_CORE_HANDLE           USB_OTG_HS_dev;
extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
extern uint32_t USBD_OTG_EP1IN_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
extern uint32_t USBD_OTG_EP1OUT_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);
#endif
#endif /* UNDEF */

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/
// This function handles NMI exception.
void NMI_Handler(void) {
}

// This function handles Hard Fault exception.
void HardFault_Handler(void) {
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1);
}

// This function handles Memory Manage exception.
void MemManage_Handler(void) {
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1);
}

// This function handles Bus Fault exception.
void BusFault_Handler(void) {
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1);
}

// This function handles Usage Fault exception.
void UsageFault_Handler(void) {
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1);
}

// This function handles SVCall exception.
void SVC_Handler(void) {
}

// This function handles Debug Monitor exception.
void DebugMon_Handler(void) {
}

// This function handles PendSVC exception.
void PendSV_Handler(void) {
}

// This function handles SysTick Handler.
void SysTick_Handler(void) {
    /* Information panel */
//    LCD_SetTextColor(Green);
//    LCD_DisplayStringLine( LCD_PIXEL_HEIGHT - 42, USER_INFORMATION[x]);
//    LCD_SetTextColor(LCD_LOG_DEFAULT_COLOR);
}



#ifdef UNDEF
// This function handles OTG_FS_WKUP_IRQHandler Handler.
#ifdef USE_USB_OTG_FS
void OTG_FS_WKUP_IRQHandler(void) {
    if(USB_OTG_FS_dev.cfg.low_power) {
        // Do not sleep on exit from current ISR
        SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk);
        SystemInit();
        USB_OTG_UngateClock(&USB_OTG_FS_dev);
    }
    EXTI_ClearITPendingBit(EXTI_Line18);
}
#endif

// This function handles OTG_HS_WKUP_IRQHandler Handler.
#ifdef USE_USB_OTG_HS
void OTG_HS_WKUP_IRQHandler(void) {
    if(USB_OTG_HS_dev.cfg.low_power) {
        // Do not sleep on exit from current ISR
        SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk);
        SystemInit();
        USB_OTG_UngateClock(&USB_OTG_HS_dev);
    }
    EXTI_ClearITPendingBit(EXTI_Line20);
}
#endif

#ifdef USE_USB_OTG_FS
void OTG_FS_IRQHandler(void) {
    USBD_OTG_ISR_Handler (&USB_OTG_FS_dev);
}
#endif

// This function handles OTG_HS Handler.
#ifdef USE_USB_OTG_HS
void OTG_HS_IRQHandler (void) {
    USBD_OTG_ISR_Handler (&USB_OTG_HS_dev);
}
#endif

#ifdef USB_OTG_HS_DEDICATED_EP1_ENABLED
// This function handles EP1_IN Handler.
void OTG_HS_EP1_IN_IRQHandler (void) {
    USBD_OTG_EP1IN_ISR_Handler ( &USB_OTG_HS_dev );
}

// This function handles EP1_OUT Handler.
void OTG_HS_EP1_OUT_IRQHandler (void) {
    USBD_OTG_EP1OUT_ISR_Handler ( &USB_OTG_HS_dev );
}
#endif
#endif /* UNDEF */

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

// This function handles PPP interrupt request.
/*void PPP_IRQHandler(void) {
}*/

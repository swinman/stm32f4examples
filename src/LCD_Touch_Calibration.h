/** @file    LCD_Touch_Calibration.h
  * @author  CMP Team
  * @version V1.0.0
  * @date    28-December-2012
  * @brief   header for LCD_Touch_Calibration.c
  */

#ifndef _LCD_TOUCH_CALIBRATION_H_
#define _LCD_TOUCH_CALIBRATION_H_

typedef struct {
    uint16_t x;
    uint16_t y;
} Point_Struct;

typedef struct {
    /*scale factor for x direction*/
    float    xScale;
    /*scale factor for x direction*/
    float    yScale;
    /*Offset in x direction*/
    int16_t  xOffset;
    /*Offset in y direction*/
    int16_t  yOffset;
} Adjust_Struct;

#ifdef LCD_TOUCH_CALIBRATION_MODULE
    #define LCD_TOUCH_CALIBRATION_EXT
#else
    #define LCD_TOUCH_CALIBRATION_EXT extern
#endif

LCD_TOUCH_CALIBRATION_EXT Adjust_Struct adjust_Para;

void    Lcd_Touch_Calibration             (void);
void 	Calibration_Test_Dispose		  (void);
#endif

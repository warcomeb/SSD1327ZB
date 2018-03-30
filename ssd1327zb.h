/******************************************************************************
 * SSD1327ZB - Library for SSD1327ZB OLed Driver based on libohiboard
 * Copyright (C) 2018 Alessio Socci & Marco Giammarini
 *
 * Authors:
 *  Alessio Socci <alessios1284@hotmail.it>
 *  Marco Giammarini <m.giammarini@warcomeb.it>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

#ifndef __WARCOMEB_SSD1327ZB_H
#define __WARCOMEB_SSD1327ZB_H

#define WARCOMEB_SSD1327ZB_LIBRARY_VERSION     "1.0.0"
#define WARCOMEB_SSD1327ZB_LIBRARY_VERSION_M   1
#define WARCOMEB_SSD1327ZB_LIBRARY_VERSION_m   0
#define WARCOMEB_SSD1327ZB_LIBRARY_VERSION_bug 0
#define WARCOMEB_SSD1327ZB_LIBRARY_TIME        0

#include "libohiboard.h"
#include "GDL/gdl.h"

/*
 * The user must define these label...
 * The name of the define is:
 *     #define WARCOMEB_SSD1327ZB_HEIGHT         xx
 *     #define WARCOMEB_SSD1327ZB_WIDTH          xx
 */
#ifndef __NO_BOARD_H
#include "board.h"
#endif

#if !defined(WARCOMEB_SSD1327ZB_HEIGHT) | !defined(WARCOMEB_SSD1327ZB_WIDTH)
#error "You must define height and width of display!"
#else
// Every byte represent 2 pixel...
#define WARCOMEB_SSD1327ZB_BUFFERDIMENSION (WARCOMEB_SSD1327ZB_WIDTH*WARCOMEB_SSD1327ZB_HEIGHT/2)
#endif

#if defined(WARCOMEB_SSD1327ZB_HEIGHT) & ((WARCOMEB_SSD1327ZB_HEIGHT < 16) | (WARCOMEB_SSD1327ZB_HEIGHT > 128))
#error "The height must be between 16 and 128!"
#endif

#if defined(WARCOMEB_SSD1327ZB_WIDTH) & ((WARCOMEB_SSD1327ZB_WIDTH < 16) | (WARCOMEB_SSD1327ZB_WIDTH > 128))
#error "The width must be between 16 and 128!"
#endif

/**
 * A usefull enum that define all the possbile color for each pixel.
 */
typedef enum _SSD1327ZB_GrayScale
{
	SSD1327ZB_GRAYSCALE_0  = 0,
	SSD1327ZB_GRAYSCALE_1  = 1,
	SSD1327ZB_GRAYSCALE_2  = 2,
	SSD1327ZB_GRAYSCALE_3  = 3,
	SSD1327ZB_GRAYSCALE_4  = 4,
	SSD1327ZB_GRAYSCALE_5  = 5,
	SSD1327ZB_GRAYSCALE_6  = 6,
	SSD1327ZB_GRAYSCALE_7  = 7,
	SSD1327ZB_GRAYSCALE_8  = 8,
	SSD1327ZB_GRAYSCALE_9  = 9,
	SSD1327ZB_GRAYSCALE_10 = 10,
	SSD1327ZB_GRAYSCALE_11 = 11,
	SSD1327ZB_GRAYSCALE_12 = 12,
	SSD1327ZB_GRAYSCALE_13 = 13,
	SSD1327ZB_GRAYSCALE_14 = 14,
	SSD1327ZB_GRAYSCALE_15 = 15,
} SSD1327ZB_GrayScale;

typedef enum _SSD1327ZB_Product
{
	SSD1327ZB_PRODUCT_RAYSTAR_REX128128B   = 0x0001 | GDL_MODELTYPE_SSD1327ZB,
} SSD1327ZB_Product;

typedef struct SSD1327ZB_Device
{
    GDL_Device gdl;                         /**< Common part for each device */

#if defined WARCOMEB_GDL_PARALLEL

#elif defined WARCOMEB_GDL_I2C

    Gpio_Pins rstPin;            /**< Reset pin used for start-up the display */

#elif defined WARCOMEB_GDL_SPI

#endif

    /** Buffer to store display data */
    uint8_t buffer [WARCOMEB_SSD1327ZB_BUFFERDIMENSION];

} SSD1327ZB_Device;

/**
 *
 */
void SSD1327ZB_init (SSD1327ZB_Device* dev);

/**
 *
 *
 * @param[in] dev The handle of the device
 * @param[in] xPos The x position
 * @param[in] yPos The y position
 * @param[in] color The color of the pixel
 * @return
 */
GDL_Errors SSD1327ZB_drawPixel (SSD1327ZB_Device* dev,
                                uint8_t xPos,
                                uint8_t yPos,
							    SSD1327ZB_GrayScale color);

/**
 * The function print a line in the selected position with the selected
 * color.
 *
 * @param[in] dev The handle of the device
 * @param[in] xStart The starting x position
 * @param[in] yStart The starting y position
 * @param[in] xStop The ending x position
 * @param[in] yStop The ending y position
 * @param[in] color The color of the line
 */
void SSD1327ZB_drawLine (SSD1327ZB_Device* dev,
                         uint8_t xStart,
                         uint8_t yStart,
                         uint8_t xStop,
                         uint8_t yStop,
					     SSD1327ZB_GrayScale color);

/**
 * The function print a horizontal line from the selected position with
 * the selected width and color.
 *
 * @param[in] dev The handle of the device
 * @param[in] xStart The starting x position
 * @param[in] yStart The starting y position
 * @param[in] width The width of the line
 * @param[in] color The color of the line
 */
void SSD1327ZB_drawHLine (SSD1327ZB_Device* dev,
                          uint8_t xStart,
                          uint8_t yStart,
                          uint8_t width,
                          SSD1327ZB_GrayScale color);

/**
 * The function print a vertical line from the selected position with
 * the selected height and color.
 *
 * @param[in] dev The handle of the device
 * @param[in] xStart The starting x position
 * @param[in] yStart The starting y position
 * @param[in] height The height of the line
 * @param[in] color The color of the line
 */
void SSD1327ZB_drawVLine (SSD1327ZB_Device* dev,
                          uint8_t xStart,
                          uint8_t yStart,
                          uint8_t height,
                          SSD1327ZB_GrayScale color);

/**
 * The function draw a rectangle. It can be fill or not.
 *
 * @param[in] dev The handle of the device
 * @param[in] xStart The starting x position
 * @param[in] yStart The starting y position
 * @param[in] width The ending x position
 * @param[in] height The ending y position
 * @param[in] color The color of the rectangle
 * @param[in] isFill If TRUE the rectangle will be fill
 */
void SSD1327ZB_drawRectangle (SSD1327ZB_Device* dev,
                              uint16_t xStart,
                              uint16_t yStart,
                              uint16_t width,
                              uint16_t height,
							  SSD1327ZB_GrayScale color,
                              bool isFill);

/**
 * The function print a char in the selected position with the selected
 * color and size.
 * The starting point is the top-left corner of the char.
 *
 * @param[in] dev The handle of the device
 * @param[in] xPos The x position
 * @param[in] yPos The y position
 * @param[in] c The char to be draw
 * @param[in] color The foreground color of the char
 * @param[in] background The background color of the char
 * @param[in] size The size for the char, if 0 use default dimension
 * @return GDL_ERRORS_WRONG_POSITION if the dimension plus position of the char
 *         exceeds the width or height of the display, GDL_ERRORS_OK otherwise.
 */
GDL_Errors SSD1327ZB_drawChar (SSD1327ZB_Device* dev,
                               uint16_t xPos,
                               uint16_t yPos,
                               uint8_t c,
							   SSD1327ZB_GrayScale color,
                               SSD1327ZB_GrayScale background,
                               uint8_t size);


/**
 * @param[in] dev The handle of the device
 * @param[in] value The 256 contrast steps from 00h to FFh
 */
void SSD1327ZB_setContrast (SSD1327ZB_Device* dev, uint8_t value);

/**
 *
 *
 * @param[in] dev The handle of the device
 */
//void SSD1306_inverseDisplay (SSD1327ZB_Device* dev);

/**
 *
 *
 * @param[in] dev The handle of the device
 */
//void SSD1306_normalDisplay (SSD1327ZB_Device* dev);

/**
 * This function clear the display setting off all pixel
 *
 * @param[in] dev The handle of the device
 */
void SSD1327ZB_clear (SSD1327ZB_Device* dev);

/**
 *
 *
 * @param[in] dev The handle of the device
 */
void SSD1327ZB_flush (SSD1327ZB_Device* dev);

/**
 *
 *
 * @param[in] dev The handle of the device
 * @param[in] xStart The x start position into the buffer
 * @param[in] xStop The x stop position into the buffer
 * @param[in] yStart The y start position into the buffer
 * @param[in] yStop The y stop position into the buffer
 */
void SSD1327ZB_flushPart (SSD1327ZB_Device* dev,
                          uint8_t xStart,
                          uint8_t xStop,
                          uint8_t yStart,
                          uint8_t yStop);

#endif /* __WARCOMEB_SSD1327ZB_H */


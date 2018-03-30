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

#include "ssd1327zb.h"

#include <string.h>

#define SSD1327ZB_CMD_SETCOLUMNADDR              0x15
#define SSD1327ZB_CMD_SETROWADDR                 0x75
#define SSD1327ZB_CMD_SETCONTRAST                0x81
#define SSD1327ZB_CMD_SEGMENTREMAP               0xA0 /**< Set Display remap */
#define SSD1327ZB_CMD_STARTLINE                  0xA1 /**< Set start line */
#define SSD1327ZB_CMD_DISPLAYNORMAL              0xA4
#define SSD1327ZB_CMD_DISPLAYALLON               0xA5 /**< Entire Display is ON with gray scale level to GS15 */
#define SSD1327ZB_CMD_DISPLAYALLOFF              0xA6 /**< Entire Display is OFF with gray scale level to GS0 */
#define SSD1327ZB_CMD_DISPLAYINVERSE             0xA7
#define SSD1327ZB_CMD_DISPLAYOFF                 0xAE
#define SSD1327ZB_CMD_DISPLAYON                  0xAF

#define SSD1327ZB_REMAP_COLUMN                   0x01 /**< Enable column address remap */
#define SSD1327ZB_REMAP_NIBBLE                   0x02 /**< Enable nibble remap */
#define SSD1327ZB_REMAP_ADDR_INCREMENT           0x04 /**< Enable vertical address increment */
#define SSD1327ZB_REMAP_COM                      0x10 /**< Enable COM remap */
#define SSD1327ZB_REMAP_ODDEVEN_COM              0x40 /**< Enable COM split odd/even */

static void SSD1327ZB_write8 (SSD1327ZB_Device* dev, uint8_t value)
{
    ((value & 0x01) > 0) ? Gpio_set(dev->gdl.d0) : Gpio_clear(dev->gdl.d0);
    ((value & 0x02) > 0) ? Gpio_set(dev->gdl.d1) : Gpio_clear(dev->gdl.d1);
    ((value & 0x04) > 0) ? Gpio_set(dev->gdl.d2) : Gpio_clear(dev->gdl.d2);
    ((value & 0x08) > 0) ? Gpio_set(dev->gdl.d3) : Gpio_clear(dev->gdl.d3);
    ((value & 0x10) > 0) ? Gpio_set(dev->gdl.d4) : Gpio_clear(dev->gdl.d4);
    ((value & 0x20) > 0) ? Gpio_set(dev->gdl.d5) : Gpio_clear(dev->gdl.d5);
    ((value & 0x40) > 0) ? Gpio_set(dev->gdl.d6) : Gpio_clear(dev->gdl.d6);
    ((value & 0x80) > 0) ? Gpio_set(dev->gdl.d7) : Gpio_clear(dev->gdl.d7);
}

static void SSD1327ZB_sendCommand (SSD1327ZB_Device* dev, uint8_t command)
{
#if defined WARCOMEB_GDL_PARALLEL

    Gpio_set(dev->gdl.rd);
    Gpio_clear(dev->gdl.cs);
    Gpio_set(dev->gdl.wr);
    // Is command message
    Gpio_clear(dev->gdl.dc);
    // Enable writing
    Gpio_clear(dev->gdl.wr);
    SSD1327ZB_write8(dev,command);
    // Restore write pin
    Gpio_set(dev->gdl.wr);
    // Disable device
    Gpio_set(dev->gdl.cs);

#elif defined WARCOMEB_GDL_I2C

#elif defined WARCOMEB_GDL_SPI

#endif
}

static void SSD1327ZB_sendData (SSD1327ZB_Device* dev, uint8_t value)
{
#if defined WARCOMEB_GDL_PARALLEL

    Gpio_set(dev->gdl.rd);
    Gpio_clear(dev->gdl.cs);
    Gpio_set(dev->gdl.wr);
    // is data message
    Gpio_set(dev->gdl.dc);
    // Enable writing
    Gpio_clear(dev->gdl.wr);
    SSD1327ZB_write8(dev,value);
    // Restore write pin
    Gpio_set(dev->gdl.wr);
    // Disable device
    Gpio_set(dev->gdl.cs);

#elif defined WARCOMEB_GDL_I2C

#elif defined WARCOMEB_GDL_SPI

#endif
}

/**
 * The function set the current position into the display. The values are related to
 * the internal buffer of the display.
 *
 * @param[in] dev The handle of the device
 * @param[in] xStart The x start position into the buffer
 * @param[in] xStop The x stop position into the buffer
 * @param[in] yStart The y start position into the buffer
 * @param[in] yStop The y stop position into the buffer
 * @return Return an error if the requested position is wrong,
 *         GDL_ERRORS_OK otherwise.
 */
static GDL_Errors SSD1327ZB_setBufferPosition (SSD1327ZB_Device* dev,
                                               uint8_t xStart,
                                               uint8_t xStop,
                                               uint8_t yStart,
                                               uint8_t yStop)
{
    if ((xStart >= dev->gdl.width) || (yStart >= dev->gdl.height))
        return GDL_ERRORS_WRONG_POSITION;

    if ((xStop >= dev->gdl.width) || (yStop >= dev->gdl.height))
        return GDL_ERRORS_WRONG_POSITION;

    // Set column address
    SSD1327ZB_sendCommand(dev,SSD1327ZB_CMD_SETCOLUMNADDR);
    SSD1327ZB_sendCommand(dev,xStart/2);
    SSD1327ZB_sendCommand(dev,xStop/2);

    // Set row address
    SSD1327ZB_sendCommand(dev,SSD1327ZB_CMD_SETROWADDR);
    SSD1327ZB_sendCommand(dev,yStart);
    SSD1327ZB_sendCommand(dev,yStop);

    return GDL_ERRORS_OK;
}

void SSD1327ZB_init (SSD1327ZB_Device* dev)
{
    // Set the device model
    dev->gdl.model = GDL_MODELTYPE_SSD1327ZB;

    // Save display size
    dev->gdl.height = WARCOMEB_SSD1327ZB_HEIGHT;
    dev->gdl.width = WARCOMEB_SSD1327ZB_WIDTH;

    // Save default font size
    dev->gdl.fontSize = 1;
    dev->gdl.useCustomFont = FALSE;

    // Save callback for drawing pixel
    dev->gdl.drawPixel = SSD1327ZB_drawPixel;

//    memset(dev->buffer, 0x00, WARCOMEB_SSD1327ZB_BUFFERDIMENSION);

#if defined WARCOMEB_GDL_PARALLEL

    Gpio_config(dev->gdl.d0,GPIO_PINS_OUTPUT);
    Gpio_config(dev->gdl.d1,GPIO_PINS_OUTPUT);
    Gpio_config(dev->gdl.d2,GPIO_PINS_OUTPUT);
    Gpio_config(dev->gdl.d3,GPIO_PINS_OUTPUT);
    Gpio_config(dev->gdl.d4,GPIO_PINS_OUTPUT);
    Gpio_config(dev->gdl.d5,GPIO_PINS_OUTPUT);
    Gpio_config(dev->gdl.d6,GPIO_PINS_OUTPUT);
    Gpio_config(dev->gdl.d7,GPIO_PINS_OUTPUT);

    Gpio_config(dev->gdl.rd,GPIO_PINS_OUTPUT);
    Gpio_config(dev->gdl.dc,GPIO_PINS_OUTPUT);
    Gpio_config(dev->gdl.rs,GPIO_PINS_OUTPUT);
    Gpio_config(dev->gdl.cs,GPIO_PINS_OUTPUT);
    Gpio_config(dev->gdl.wr,GPIO_PINS_OUTPUT);

    Gpio_set(dev->gdl.dc);
    Gpio_set(dev->gdl.rd);
    Gpio_set(dev->gdl.wr);
    Gpio_set(dev->gdl.cs);
    Gpio_set(dev->gdl.rs);

#elif defined WARCOMEB_GDL_I2C

#elif defined WARCOMEB_GDL_SPI

#endif

    SSD1327ZB_sendCommand(dev,SSD1327ZB_CMD_DISPLAYON);
    // Wait 100ms after on
    dev->gdl.delayTime(100);

    // Set display to normal
    SSD1327ZB_sendCommand(dev,SSD1327ZB_CMD_DISPLAYNORMAL);

    // Select segment re-map, COM scan direction, COM hardware configuration and
    // de-select level
    // They depend on producer choice
    switch (dev->gdl.product)
    {
    case SSD1327ZB_PRODUCT_RAYSTAR_REX128128B:
        SSD1327ZB_sendCommand(dev,SSD1327ZB_CMD_SEGMENTREMAP);
        SSD1327ZB_sendCommand(dev,SSD1327ZB_REMAP_ODDEVEN_COM);

        SSD1327ZB_sendCommand(dev,SSD1327ZB_CMD_STARTLINE);
        SSD1327ZB_sendCommand(dev,0);
        break;
    }
}

GDL_Errors SSD1327ZB_drawPixel (SSD1327ZB_Device* dev,
                                uint8_t xPos,
                                uint8_t yPos,
							    SSD1327ZB_GrayScale color)
{
    if ((xPos >= dev->gdl.width) || (yPos >= dev->gdl.height))
        return GDL_ERRORS_WRONG_POSITION;

    uint16_t pos = ((uint16_t) xPos/2) + ((uint16_t) yPos*(dev->gdl.width/2));

    if (xPos%2)
        dev->buffer[pos] = (((color << 4) & 0xF0) | (dev->buffer[pos] & 0x0F));
    else
        dev->buffer[pos] = ((color & 0x0F) | (dev->buffer[pos] & 0xF0));

    return GDL_ERRORS_OK;
}

void SSD1327ZB_setContrast (SSD1327ZB_Device* dev, uint8_t value)
{
    SSD1327ZB_sendCommand(dev,SSD1327ZB_CMD_SETCONTRAST);
    SSD1327ZB_sendCommand(dev,value);
}

void SSD1327ZB_flush (SSD1327ZB_Device* dev)
{
    // Set the cursor to the starting point of the display
    // Print all the buffer
    SSD1327ZB_setBufferPosition(dev,0,dev->gdl.width-1,0,dev->gdl.height-1);

    for (uint16_t i = 0; i < WARCOMEB_SSD1327ZB_BUFFERDIMENSION; i++)
    {
        SSD1327ZB_sendData(dev,dev->buffer[i]);
    }
}

void SSD1327ZB_flushPart (SSD1327ZB_Device* dev,
                          uint8_t xStart,
                          uint8_t xStop,
                          uint8_t yStart,
                          uint8_t yStop)
{
    // Set the part of the display where change the pixels
    GDL_Errors error = SSD1327ZB_setBufferPosition(dev,xStart,xStop,yStart,yStop);
    if (error != GDL_ERRORS_OK) return;

    uint8_t xStartHalf = xStart/2;
    uint8_t xStopHalf = xStop/2;
    uint8_t widthHalf = dev->gdl.width/2;

    for (uint8_t i = yStart; i <= yStop; i++)
    {
        for (uint8_t j = xStartHalf; j <= xStopHalf; j++)
        {
            SSD1327ZB_sendData(dev,dev->buffer[j + (i * widthHalf)]);
        }
    }
}

void SSD1327ZB_clear (SSD1327ZB_Device* dev)
{
    // Reset memory buffer
    memset(dev->buffer, 0x00, WARCOMEB_SSD1327ZB_BUFFERDIMENSION);
    // Flush the new buffer
    SSD1327ZB_flush(dev);
}

void SSD1327ZB_drawLine (SSD1327ZB_Device* dev,
                         uint8_t xStart,
                         uint8_t yStart,
                         uint8_t xStop,
                         uint8_t yStop,
                         SSD1327ZB_GrayScale color)
{
    GDL_drawLine(&(dev->gdl),xStart,yStart,xStop,yStop,(uint8_t)color);
}

void SSD1327ZB_drawHLine (SSD1327ZB_Device* dev,
                          uint8_t xStart,
                          uint8_t yStart,
                          uint8_t width,
                          SSD1327ZB_GrayScale color)
{
    SSD1327ZB_drawLine(dev,xStart,yStart,xStart+width,yStart,color);
}

void SSD1327ZB_drawVLine (SSD1327ZB_Device* dev,
                          uint8_t xStart,
                          uint8_t yStart,
                          uint8_t height,
                          SSD1327ZB_GrayScale color)
{
    SSD1327ZB_drawLine(dev,xStart,yStart,xStart,yStart+height,color);
}

void SSD1327ZB_drawRectangle (SSD1327ZB_Device* dev,
                              uint16_t xStart,
                              uint16_t yStart,
                              uint16_t width,
                              uint16_t height,
                              SSD1327ZB_GrayScale color,
                              bool isFill)
{
    GDL_drawRectangle(&(dev->gdl),xStart,yStart,width,height,(uint8_t)color,isFill);
}

GDL_Errors SSD1327ZB_drawChar (SSD1327ZB_Device* dev,
                               uint16_t xPos,
                               uint16_t yPos,
                               uint8_t c,
                               SSD1327ZB_GrayScale color,
                               SSD1327ZB_GrayScale background,
                               uint8_t size)
{
    return GDL_drawChar(&(dev->gdl),xPos,yPos,c,(uint8_t)color,(uint8_t)background,size);
}

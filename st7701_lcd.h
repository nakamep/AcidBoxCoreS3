#ifndef ST7701_LCD_H
#define ST7701_LCD_H

#include "config.h"

#if defined(ARDUINO_M5STACK_CORES3) || defined(M5STACK_CORES3)

#include <SPI.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// ST7701 LCD Driver for M5Stack Core S3
// M5Stack Core S3 LCD: 2.0" 320x240 IPS display with ST7701 controller

// Pin definitions for M5Stack Core S3 LCD
#define LCD_SCK_PIN     7   // SPI Clock
#define LCD_MOSI_PIN    6   // SPI MOSI  
#define LCD_CS_PIN      5   // SPI Chip Select
#define LCD_DC_PIN      4   // Data/Command
#define LCD_RST_PIN     8   // Reset
#define LCD_BL_PIN      16  // Backlight

// Display specifications
#define LCD_WIDTH       320
#define LCD_HEIGHT      240
#define LCD_ROTATION    1   // Default rotation (landscape)

// SPI Configuration
#ifndef ST7701_SPI_SPEED
#define ST7701_SPI_SPEED 8000000  // 8MHz default (can be overridden)
#endif

// ST7701 Commands
#define ST7701_NOP      0x00
#define ST7701_SWRESET  0x01
#define ST7701_RDDID    0x04
#define ST7701_RDDST    0x09
#define ST7701_SLPIN    0x10
#define ST7701_SLPOUT   0x11
#define ST7701_PTLON    0x12
#define ST7701_NORON    0x13
#define ST7701_INVOFF   0x20
#define ST7701_INVON    0x21
#define ST7701_DISPOFF  0x28
#define ST7701_DISPON   0x29
#define ST7701_CASET    0x2A
#define ST7701_RASET    0x2B
#define ST7701_RAMWR    0x2C
#define ST7701_RAMRD    0x2E
#define ST7701_PTLAR    0x30
#define ST7701_MADCTL   0x36
#define ST7701_COLMOD   0x3A

// Colors (16-bit RGB565)
#define LCD_BLACK       0x0000
#define LCD_BLUE        0x001F
#define LCD_RED         0xF800
#define LCD_GREEN       0x07E0
#define LCD_CYAN        0x07FF
#define LCD_MAGENTA     0xF81F
#define LCD_YELLOW      0xFFE0
#define LCD_WHITE       0xFFFF

class ST7701_LCD {
public:
    ST7701_LCD();
    
    // Initialization and control
    bool begin();
    void reset();
    void displayOn();
    void displayOff();
    void setBacklight(uint8_t brightness);
    void setRotation(uint8_t rotation);
    
    // Drawing functions
    void fillScreen(uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    
    // Text functions
    void setCursor(int16_t x, int16_t y);
    void setTextColor(uint16_t color);
    void setTextSize(uint8_t size);
    void print(const char* text);
    void println(const char* text);
    
    // Display info
    int16_t width() const { return _width; }
    int16_t height() const { return _height; }
    
private:
    SPIClass* _spi;
    int16_t _width, _height;
    int16_t _cursor_x, _cursor_y;
    uint16_t _text_color;
    uint8_t _text_size;
    uint8_t _rotation;
    
    // Thread safety for fillRect operation
    SemaphoreHandle_t _fillRectMutex;
    
    // Low-level SPI functions
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void writeData16(uint16_t data);
    void writeDataBytes(uint8_t* data, uint32_t len);
    
    // Display control
    void setAddressWindow(int16_t x, int16_t y, int16_t w, int16_t h);
    void initDisplay();
    
    // Character rendering
    void drawChar(int16_t x, int16_t y, char c, uint16_t color, uint8_t size);
};

// Global instance (only available on M5Stack Core S3)
extern ST7701_LCD lcd;

#endif // M5STACK_CORES3

#endif // ST7701_LCD_H
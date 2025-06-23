#include "st7701_lcd.h"

#if defined(ARDUINO_M5STACK_CORES3) || defined(M5STACK_CORES3)

// Simple 5x7 font bitmap (ASCII 32-126)
static const uint8_t font5x7[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, // space
    0x00, 0x00, 0x2f, 0x00, 0x00, // !
    0x00, 0x07, 0x00, 0x07, 0x00, // "
    0x14, 0x7f, 0x14, 0x7f, 0x14, // #
    0x24, 0x2a, 0x7f, 0x2a, 0x12, // $
    0x23, 0x13, 0x08, 0x64, 0x62, // %
    0x36, 0x49, 0x55, 0x22, 0x50, // &
    0x00, 0x05, 0x03, 0x00, 0x00, // '
    0x00, 0x1c, 0x22, 0x41, 0x00, // (
    0x00, 0x41, 0x22, 0x1c, 0x00, // )
    0x14, 0x08, 0x3e, 0x08, 0x14, // *
    0x08, 0x08, 0x3e, 0x08, 0x08, // +
    0x00, 0x50, 0x30, 0x00, 0x00, // ,
    0x08, 0x08, 0x08, 0x08, 0x08, // -
    0x00, 0x60, 0x60, 0x00, 0x00, // .
    0x20, 0x10, 0x08, 0x04, 0x02, // /
    0x3e, 0x51, 0x49, 0x45, 0x3e, // 0
    0x00, 0x42, 0x7f, 0x40, 0x00, // 1
    0x42, 0x61, 0x51, 0x49, 0x46, // 2
    0x21, 0x41, 0x45, 0x4b, 0x31, // 3
    0x18, 0x14, 0x12, 0x7f, 0x10, // 4
    0x27, 0x45, 0x45, 0x45, 0x39, // 5
    0x3c, 0x4a, 0x49, 0x49, 0x30, // 6
    0x01, 0x71, 0x09, 0x05, 0x03, // 7
    0x36, 0x49, 0x49, 0x49, 0x36, // 8
    0x06, 0x49, 0x49, 0x29, 0x1e, // 9
    0x00, 0x36, 0x36, 0x00, 0x00, // :
    0x00, 0x56, 0x36, 0x00, 0x00, // ;
    0x08, 0x14, 0x22, 0x41, 0x00, // <
    0x14, 0x14, 0x14, 0x14, 0x14, // =
    0x00, 0x41, 0x22, 0x14, 0x08, // >
    0x02, 0x01, 0x51, 0x09, 0x06, // ?
    0x32, 0x49, 0x79, 0x41, 0x3e, // @
    0x7e, 0x11, 0x11, 0x11, 0x7e, // A
    0x7f, 0x49, 0x49, 0x49, 0x36, // B
    0x3e, 0x41, 0x41, 0x41, 0x22, // C
    0x7f, 0x41, 0x41, 0x22, 0x1c, // D
    0x7f, 0x49, 0x49, 0x49, 0x41, // E
    0x7f, 0x09, 0x09, 0x09, 0x01, // F
    0x3e, 0x41, 0x49, 0x49, 0x7a, // G
    0x7f, 0x08, 0x08, 0x08, 0x7f, // H
    0x00, 0x41, 0x7f, 0x41, 0x00, // I
    0x20, 0x40, 0x41, 0x3f, 0x01, // J
    0x7f, 0x08, 0x14, 0x22, 0x41, // K
    0x7f, 0x40, 0x40, 0x40, 0x40, // L
    0x7f, 0x02, 0x0c, 0x02, 0x7f, // M
    0x7f, 0x04, 0x08, 0x10, 0x7f, // N
    0x3e, 0x41, 0x41, 0x41, 0x3e, // O
    0x7f, 0x09, 0x09, 0x09, 0x06, // P
    0x3e, 0x41, 0x51, 0x21, 0x5e, // Q
    0x7f, 0x09, 0x19, 0x29, 0x46, // R
    0x46, 0x49, 0x49, 0x49, 0x31, // S
    0x01, 0x01, 0x7f, 0x01, 0x01, // T
    0x3f, 0x40, 0x40, 0x40, 0x3f, // U
    0x1f, 0x20, 0x40, 0x20, 0x1f, // V
    0x3f, 0x40, 0x38, 0x40, 0x3f, // W
    0x63, 0x14, 0x08, 0x14, 0x63, // X
    0x07, 0x08, 0x70, 0x08, 0x07, // Y
    0x61, 0x51, 0x49, 0x45, 0x43, // Z
};

ST7701_LCD::ST7701_LCD() : 
    _spi(nullptr),
    _width(LCD_WIDTH),
    _height(LCD_HEIGHT),
    _cursor_x(0),
    _cursor_y(0),
    _text_color(LCD_WHITE),
    _text_size(1),
    _rotation(LCD_ROTATION) {
}

bool ST7701_LCD::begin() {
    // Initialize SPI
    _spi = &SPI;
    _spi->begin(LCD_SCK_PIN, -1, LCD_MOSI_PIN, LCD_CS_PIN); // SCK, MISO, MOSI, SS
    
    // Initialize control pins
    pinMode(LCD_DC_PIN, OUTPUT);
    pinMode(LCD_RST_PIN, OUTPUT);
    pinMode(LCD_BL_PIN, OUTPUT);
    pinMode(LCD_CS_PIN, OUTPUT);
    
    // Reset display
    reset();
    
    delay(100);
    
    // Initialize display
    initDisplay();
    
    // Turn on backlight
    setBacklight(255);
    
    // Clear screen
    fillScreen(LCD_BLACK);
    
    return true;
}

void ST7701_LCD::reset() {
    digitalWrite(LCD_RST_PIN, HIGH);
    delay(10);
    digitalWrite(LCD_RST_PIN, LOW);
    delay(10);
    digitalWrite(LCD_RST_PIN, HIGH);
    delay(50);
}

void ST7701_LCD::initDisplay() {
    // ST7701 initialization sequence
    writeCommand(ST7701_SWRESET);
    delay(120);
    
    writeCommand(ST7701_SLPOUT);
    delay(120);
    
    // Color mode: 16-bit RGB565
    writeCommand(ST7701_COLMOD);
    writeData(0x55);
    
    // Memory access control
    writeCommand(ST7701_MADCTL);
    writeData(0x00);
    
    // Display inversion off
    writeCommand(ST7701_INVOFF);
    
    // Normal display mode
    writeCommand(ST7701_NORON);
    
    // Display on
    writeCommand(ST7701_DISPON);
    delay(100);
}

void ST7701_LCD::writeCommand(uint8_t cmd) {
    _spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    digitalWrite(LCD_CS_PIN, LOW);
    digitalWrite(LCD_DC_PIN, LOW);  // Command mode
    _spi->transfer(cmd);
    digitalWrite(LCD_CS_PIN, HIGH);
    _spi->endTransaction();
}

void ST7701_LCD::writeData(uint8_t data) {
    _spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    digitalWrite(LCD_CS_PIN, LOW);
    digitalWrite(LCD_DC_PIN, HIGH); // Data mode
    _spi->transfer(data);
    digitalWrite(LCD_CS_PIN, HIGH);
    _spi->endTransaction();
}

void ST7701_LCD::writeData16(uint16_t data) {
    _spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    digitalWrite(LCD_CS_PIN, LOW);
    digitalWrite(LCD_DC_PIN, HIGH); // Data mode
    _spi->transfer16(data);
    digitalWrite(LCD_CS_PIN, HIGH);
    _spi->endTransaction();
}

void ST7701_LCD::writeDataBytes(uint8_t* data, uint32_t len) {
    _spi->beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    digitalWrite(LCD_CS_PIN, LOW);
    digitalWrite(LCD_DC_PIN, HIGH); // Data mode
    _spi->transferBytes(data, nullptr, len);
    digitalWrite(LCD_CS_PIN, HIGH);
    _spi->endTransaction();
}

void ST7701_LCD::setAddressWindow(int16_t x, int16_t y, int16_t w, int16_t h) {
    // Column address set
    writeCommand(ST7701_CASET);
    writeData16(x);
    writeData16(x + w - 1);
    
    // Row address set
    writeCommand(ST7701_RASET);
    writeData16(y);
    writeData16(y + h - 1);
    
    // Memory write
    writeCommand(ST7701_RAMWR);
}

void ST7701_LCD::displayOn() {
    writeCommand(ST7701_DISPON);
}

void ST7701_LCD::displayOff() {
    writeCommand(ST7701_DISPOFF);
}

void ST7701_LCD::setBacklight(uint8_t brightness) {
    analogWrite(LCD_BL_PIN, brightness);
}

void ST7701_LCD::setRotation(uint8_t rotation) {
    _rotation = rotation % 4;
    
    writeCommand(ST7701_MADCTL);
    switch (_rotation) {
        case 0: // Portrait
            writeData(0x00);
            _width = LCD_WIDTH;
            _height = LCD_HEIGHT;
            break;
        case 1: // Landscape
            writeData(0x60);
            _width = LCD_HEIGHT;
            _height = LCD_WIDTH;
            break;
        case 2: // Portrait (flipped)
            writeData(0xC0);
            _width = LCD_WIDTH;
            _height = LCD_HEIGHT;
            break;
        case 3: // Landscape (flipped)
            writeData(0xA0);
            _width = LCD_HEIGHT;
            _height = LCD_WIDTH;
            break;
    }
}

void ST7701_LCD::fillScreen(uint16_t color) {
    fillRect(0, 0, _width, _height, color);
}

void ST7701_LCD::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (x < 0 || x >= _width || y < 0 || y >= _height) return;
    
    setAddressWindow(x, y, 1, 1);
    writeData16(color);
}

void ST7701_LCD::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    if (y < 0 || y >= _height) return;
    if (x < 0) { w += x; x = 0; }
    if (x + w > _width) w = _width - x;
    if (w <= 0) return;
    
    setAddressWindow(x, y, w, 1);
    for (int16_t i = 0; i < w; i++) {
        writeData16(color);
    }
}

void ST7701_LCD::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    if (x < 0 || x >= _width) return;
    if (y < 0) { h += y; y = 0; }
    if (y + h > _height) h = _height - y;
    if (h <= 0) return;
    
    setAddressWindow(x, y, 1, h);
    for (int16_t i = 0; i < h; i++) {
        writeData16(color);
    }
}

void ST7701_LCD::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > _width) w = _width - x;
    if (y + h > _height) h = _height - y;
    if (w <= 0 || h <= 0) return;
    
    setAddressWindow(x, y, w, h);
    uint32_t pixels = (uint32_t)w * h;
    for (uint32_t i = 0; i < pixels; i++) {
        writeData16(color);
    }
}

void ST7701_LCD::setCursor(int16_t x, int16_t y) {
    _cursor_x = x;
    _cursor_y = y;
}

void ST7701_LCD::setTextColor(uint16_t color) {
    _text_color = color;
}

void ST7701_LCD::setTextSize(uint8_t size) {
    _text_size = (size > 0) ? size : 1;
}

void ST7701_LCD::drawChar(int16_t x, int16_t y, char c, uint16_t color, uint8_t size) {
    if (c < 32 || c > 126) return; // Only printable ASCII
    
    uint8_t char_index = c - 32;
    const uint8_t* char_data = &font5x7[char_index * 5];
    
    for (uint8_t col = 0; col < 5; col++) {
        uint8_t line = pgm_read_byte(&char_data[col]);
        for (uint8_t row = 0; row < 8; row++) {
            if (line & 0x1) {
                if (size == 1) {
                    drawPixel(x + col, y + row, color);
                } else {
                    fillRect(x + col * size, y + row * size, size, size, color);
                }
            }
            line >>= 1;
        }
    }
}

void ST7701_LCD::print(const char* text) {
    while (*text) {
        if (*text == '\n') {
            _cursor_x = 0;
            _cursor_y += 8 * _text_size;
        } else if (*text == '\r') {
            _cursor_x = 0;
        } else {
            drawChar(_cursor_x, _cursor_y, *text, _text_color, _text_size);
            _cursor_x += 6 * _text_size;
            if (_cursor_x + 6 * _text_size > _width) {
                _cursor_x = 0;
                _cursor_y += 8 * _text_size;
            }
        }
        text++;
    }
}

void ST7701_LCD::println(const char* text) {
    print(text);
    _cursor_x = 0;
    _cursor_y += 8 * _text_size;
}

// Global instance (only available on M5Stack Core S3)
ST7701_LCD lcd;

#endif // M5STACK_CORES3
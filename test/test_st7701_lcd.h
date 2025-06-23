#ifndef TEST_ST7701_LCD_H
#define TEST_ST7701_LCD_H

#include <unity.h>

// Test ST7701 LCD driver compilation and basic functionality
#if defined(ARDUINO_M5STACK_CORES3) || defined(M5STACK_CORES3)
#include "../st7701_lcd.h"

void test_st7701_compilation() {
    // Test that ST7701_LCD class exists and can be instantiated
    ST7701_LCD test_lcd;
    TEST_ASSERT_TRUE(true); // If we reach here, compilation succeeded
}

void test_st7701_constants() {
    // Test that LCD constants are defined correctly
    TEST_ASSERT_EQUAL(320, LCD_WIDTH);
    TEST_ASSERT_EQUAL(240, LCD_HEIGHT);
    TEST_ASSERT_EQUAL(7, LCD_SCK_PIN);
    TEST_ASSERT_EQUAL(6, LCD_MOSI_PIN);
    TEST_ASSERT_EQUAL(5, LCD_CS_PIN);
    TEST_ASSERT_EQUAL(4, LCD_DC_PIN);
    TEST_ASSERT_EQUAL(8, LCD_RST_PIN);
    TEST_ASSERT_EQUAL(16, LCD_BL_PIN);
}

void test_st7701_color_definitions() {
    // Test color constants
    TEST_ASSERT_EQUAL(0x0000, LCD_BLACK);
    TEST_ASSERT_EQUAL(0xFFFF, LCD_WHITE);
    TEST_ASSERT_EQUAL(0xF800, LCD_RED);
    TEST_ASSERT_EQUAL(0x07E0, LCD_GREEN);
    TEST_ASSERT_EQUAL(0x001F, LCD_BLUE);
}

void test_st7701_global_instance() {
    // Test that global lcd instance exists
    TEST_ASSERT_EQUAL(320, lcd.width());
    TEST_ASSERT_EQUAL(240, lcd.height());
}

void test_st7701_commands() {
    // Test ST7701 command constants
    TEST_ASSERT_EQUAL(0x00, ST7701_NOP);
    TEST_ASSERT_EQUAL(0x01, ST7701_SWRESET);
    TEST_ASSERT_EQUAL(0x11, ST7701_SLPOUT);
    TEST_ASSERT_EQUAL(0x29, ST7701_DISPON);
    TEST_ASSERT_EQUAL(0x2A, ST7701_CASET);
    TEST_ASSERT_EQUAL(0x2B, ST7701_RASET);
    TEST_ASSERT_EQUAL(0x2C, ST7701_RAMWR);
    TEST_ASSERT_EQUAL(0x36, ST7701_MADCTL);
    TEST_ASSERT_EQUAL(0x3A, ST7701_COLMOD);
}

#else
// For non-M5Stack builds, create dummy tests that always pass
void test_st7701_compilation() {
    TEST_ASSERT_TRUE(true); // LCD driver not compiled for this target
}

void test_st7701_constants() {
    TEST_ASSERT_TRUE(true); // LCD driver not compiled for this target
}

void test_st7701_color_definitions() {
    TEST_ASSERT_TRUE(true); // LCD driver not compiled for this target
}

void test_st7701_global_instance() {
    TEST_ASSERT_TRUE(true); // LCD driver not compiled for this target
}

void test_st7701_commands() {
    TEST_ASSERT_TRUE(true); // LCD driver not compiled for this target
}
#endif

#endif // TEST_ST7701_LCD_H
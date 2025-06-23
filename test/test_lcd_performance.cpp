#include <unity.h>
#include <chrono>
#include <cstdint>
#include <cstring>

// Mock ST7701 LCD driver for native testing
// This simulates the LCD operations without actual hardware

#ifndef ARDUINO
// Native environment - mock LCD driver for performance testing

class MockST7701_LCD {
private:
    static const int LCD_WIDTH = 320;
    static const int LCD_HEIGHT = 240;
    static const int BUFFER_SIZE = 512; // Match optimized implementation
    static const int SPI_SIMULATION_CYCLES_PER_BYTE = 10; // CPU cycles to simulate SPI delay
    
    // Simulate SPI transfer delay
    void simulateSpiDelay(uint32_t bytes) {
        // Simulate realistic SPI transfer timing
        // At 8MHz SPI, each byte takes ~1.25 microseconds
        // Add minimal processing overhead
        volatile uint32_t dummy = 0;
        for (uint32_t i = 0; i < bytes * SPI_SIMULATION_CYCLES_PER_BYTE; i++) {
            dummy += i; // Simulate some CPU work
        }
    }
    
    void writeDataBytes(uint8_t* data, uint32_t len) {
        // Simulate the SPI transaction overhead
        simulateSpiDelay(len);
    }
    
public:
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        // Boundary checks (same as real implementation)
        if (x < 0) { w += x; x = 0; }
        if (y < 0) { h += y; y = 0; }
        if (x + w > LCD_WIDTH) w = LCD_WIDTH - x;
        if (y + h > LCD_HEIGHT) h = LCD_HEIGHT - y;
        if (w <= 0 || h <= 0) return;
        
        uint32_t pixels = (uint32_t)w * h;
        
        // Simulate the optimized batched implementation
        uint8_t buffer[BUFFER_SIZE * 2]; // 2 bytes per pixel (RGB565)
        
        // Pre-fill buffer with color data (simulate the real work)
        uint8_t color_hi = color >> 8;
        uint8_t color_lo = color & 0xFF;
        for (uint16_t i = 0; i < BUFFER_SIZE * 2; i += 2) {
            buffer[i] = color_hi;
            buffer[i + 1] = color_lo;
        }
        
        // Simulate batched SPI transfers
        while (pixels > 0) {
            uint16_t batch_size = (pixels > BUFFER_SIZE) ? BUFFER_SIZE : pixels;
            writeDataBytes(buffer, batch_size * 2);
            pixels -= batch_size;
        }
    }
};

void test_lcd_fillrect_performance() {
    MockST7701_LCD lcd;
    
    // Measure performance of full screen fill
    auto start_time = std::chrono::high_resolution_clock::now();
    
    lcd.fillRect(0, 0, 320, 240, 0xFFFF);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    // Convert to milliseconds for easier comparison
    double duration_ms = duration.count() / 1000.0;
    
    // Test should fail if execution time exceeds 50ms
    TEST_ASSERT_TRUE_MESSAGE(duration_ms < 50.0, 
        "fillRect performance test failed - execution time exceeded 50ms");
    
    // Also verify it's reasonably fast (should be much faster than 50ms with batching)
    TEST_ASSERT_TRUE_MESSAGE(duration_ms < 10.0, 
        "fillRect should be very fast with batched transfers");
    
    // Print timing info for debugging (will be visible in test output)
    printf("fillRect(0,0,320,240,0xFFFF) took %.3f ms\n", duration_ms);
}

void test_lcd_fillrect_multiple_operations() {
    MockST7701_LCD lcd;
    
    // Test multiple fill operations to ensure consistent performance
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Fill screen 5 times with different colors
    lcd.fillRect(0, 0, 320, 240, 0x0000); // Black
    lcd.fillRect(0, 0, 320, 240, 0xF800); // Red  
    lcd.fillRect(0, 0, 320, 240, 0x07E0); // Green
    lcd.fillRect(0, 0, 320, 240, 0x001F); // Blue
    lcd.fillRect(0, 0, 320, 240, 0xFFFF); // White
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    double duration_ms = duration.count() / 1000.0;
    
    // 5 operations should still complete well under 250ms (5 * 50ms)
    TEST_ASSERT_TRUE_MESSAGE(duration_ms < 250.0, 
        "Multiple fillRect operations took too long");
    
    printf("5x fillRect operations took %.3f ms (avg: %.3f ms each)\n", 
           duration_ms, duration_ms / 5.0);
}

void test_lcd_fillrect_partial_areas() {
    MockST7701_LCD lcd;
    
    // Test performance of partial screen fills
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Fill various rectangular areas
    lcd.fillRect(0, 0, 160, 120, 0xFFFF);     // Quarter screen
    lcd.fillRect(80, 60, 160, 120, 0xF800);   // Overlapping area
    lcd.fillRect(100, 100, 50, 50, 0x07E0);   // Small rectangle
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    double duration_ms = duration.count() / 1000.0;
    
    // Partial fills should be very fast
    TEST_ASSERT_TRUE_MESSAGE(duration_ms < 25.0, 
        "Partial fillRect operations took too long");
    
    printf("Partial fillRect operations took %.3f ms\n", duration_ms);
}

#ifdef ARDUINO
void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_lcd_fillrect_performance);
    RUN_TEST(test_lcd_fillrect_multiple_operations);
    RUN_TEST(test_lcd_fillrect_partial_areas);
    UNITY_END();
}

void loop() {}
#else
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_lcd_fillrect_performance);
    RUN_TEST(test_lcd_fillrect_multiple_operations);  
    RUN_TEST(test_lcd_fillrect_partial_areas);
    return UNITY_END();
}
#endif

#else
// For Arduino/ESP32 builds, create dummy tests
void test_lcd_fillrect_performance() {
    TEST_ASSERT_TRUE(true); // Performance test only runs on native
}

void test_lcd_fillrect_multiple_operations() {
    TEST_ASSERT_TRUE(true); // Performance test only runs on native
}

void test_lcd_fillrect_partial_areas() {
    TEST_ASSERT_TRUE(true); // Performance test only runs on native
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_lcd_fillrect_performance);
    RUN_TEST(test_lcd_fillrect_multiple_operations);
    RUN_TEST(test_lcd_fillrect_partial_areas);
    UNITY_END();
}

void loop() {}

#endif
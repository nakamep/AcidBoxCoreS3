#include <unity.h>
#include <chrono>
#include <cstdint>

// Mock ST7701 LCD Driver for Native Testing
// This simulates the ST7701 LCD driver behavior for performance testing
class MockST7701_LCD {
private:
    static const int16_t MOCK_WIDTH = 320;
    static const int16_t MOCK_HEIGHT = 240;
    
    // Simulate SPI transfer timing (microseconds per byte at 8MHz)
    static const int SPI_TRANSFER_TIME_US_PER_BYTE = 1; // Realistic timing simulation
    
public:
    MockST7701_LCD() {}
    
    int16_t width() const { return MOCK_WIDTH; }
    int16_t height() const { return MOCK_HEIGHT; }
    
    // Mock fillRect implementation that simulates the optimized ST7701 version
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        // Boundary checks (same as real implementation)
        if (x < 0) { w += x; x = 0; }
        if (y < 0) { h += y; y = 0; }
        if (x + w > MOCK_WIDTH) w = MOCK_WIDTH - x;
        if (y + h > MOCK_HEIGHT) h = MOCK_HEIGHT - y;
        if (w <= 0 || h <= 0) return;
        
        uint32_t pixels = (uint32_t)w * h;
        
        // Simulate the optimized batched transfer approach
        const uint16_t BUFFER_SIZE = 512; // Same as optimized implementation
        
        // Simulate SPI setup overhead (command + address setup)
        simulateDelay(10); // Setup overhead in microseconds
        
        // Simulate batched SPI transfers
        while (pixels > 0) {
            uint16_t batch_size = (pixels > BUFFER_SIZE) ? BUFFER_SIZE : pixels;
            // Simulate transfer time: 2 bytes per pixel at SPI speed
            int transfer_time = batch_size * 2 * SPI_TRANSFER_TIME_US_PER_BYTE;
            simulateDelay(transfer_time);
            pixels -= batch_size;
        }
        
        // Simulate SPI cleanup overhead
        simulateDelay(5); // Cleanup overhead in microseconds
    }
    
    void fillScreen(uint16_t color) {
        fillRect(0, 0, MOCK_WIDTH, MOCK_HEIGHT, color);
    }
    
private:
    void simulateDelay(int microseconds) {
        // Simulate processing time with a simple busy wait
        auto start = std::chrono::high_resolution_clock::now();
        auto end = start + std::chrono::microseconds(microseconds);
        while (std::chrono::high_resolution_clock::now() < end) {
            // Busy wait to simulate SPI transfer time
        }
    }
};

// Global mock LCD instance for testing
static MockST7701_LCD mock_lcd;

// Helper function to measure execution time in microseconds
template<typename Func>
long measureExecutionTime(Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return duration.count();
}

// Test fillRect performance for full screen (320x240)
void test_lcd_fillrect_full_screen_performance() {
    const uint16_t WHITE = 0xFFFF;
    const long MAX_EXECUTION_TIME_US = 50000; // 50ms = 50,000 microseconds
    
    // Measure execution time for full screen fillRect
    long execution_time = measureExecutionTime([&]() {
        mock_lcd.fillRect(0, 0, 320, 240, WHITE);
    });
    
    // Test should fail if execution time exceeds 50ms
    char error_msg[100];
    snprintf(error_msg, sizeof(error_msg), 
             "fillRect(0,0,320,240) took %ld µs, expected < %ld µs", 
             execution_time, MAX_EXECUTION_TIME_US);
    
    TEST_ASSERT_TRUE_MESSAGE(execution_time < MAX_EXECUTION_TIME_US, error_msg);
}

// Test fillRect performance for full screen using fillScreen method
void test_lcd_fillscreen_performance() {
    const uint16_t WHITE = 0xFFFF;
    const long MAX_EXECUTION_TIME_US = 50000; // 50ms = 50,000 microseconds
    
    // Measure execution time for fillScreen
    long execution_time = measureExecutionTime([&]() {
        mock_lcd.fillScreen(WHITE);
    });
    
    // Test should fail if execution time exceeds 50ms
    char error_msg[100];
    snprintf(error_msg, sizeof(error_msg), 
             "fillScreen() took %ld µs, expected < %ld µs", 
             execution_time, MAX_EXECUTION_TIME_US);
    
    TEST_ASSERT_TRUE_MESSAGE(execution_time < MAX_EXECUTION_TIME_US, error_msg);
}

// Test multiple consecutive fillRect operations
void test_lcd_multiple_fillrect_operations() {
    const uint16_t COLORS[] = {0x0000, 0xFFFF, 0xF800, 0x07E0, 0x001F}; // Black, White, Red, Green, Blue
    const int NUM_OPERATIONS = 5;
    const long MAX_TOTAL_TIME_US = 250000; // 250ms for 5 operations
    
    // Measure execution time for multiple operations
    long execution_time = measureExecutionTime([&]() {
        for (int i = 0; i < NUM_OPERATIONS; i++) {
            mock_lcd.fillRect(0, 0, 320, 240, COLORS[i]);
        }
    });
    
    char error_msg[100];
    snprintf(error_msg, sizeof(error_msg), 
             "%d fillRect operations took %ld µs, expected < %ld µs", 
             NUM_OPERATIONS, execution_time, MAX_TOTAL_TIME_US);
    
    TEST_ASSERT_TRUE_MESSAGE(execution_time < MAX_TOTAL_TIME_US, error_msg);
}

// Test fillRect performance for partial screen areas
void test_lcd_partial_screen_performance() {
    const uint16_t WHITE = 0xFFFF;
    const long MAX_EXECUTION_TIME_US = 15000; // 15ms for quarter screen
    
    // Test quarter screen (160x120 = 19,200 pixels)
    long execution_time = measureExecutionTime([&]() {
        mock_lcd.fillRect(0, 0, 160, 120, WHITE);
    });
    
    char error_msg[100];
    snprintf(error_msg, sizeof(error_msg), 
             "fillRect(0,0,160,120) took %ld µs, expected < %ld µs", 
             execution_time, MAX_EXECUTION_TIME_US);
    
    TEST_ASSERT_TRUE_MESSAGE(execution_time < MAX_EXECUTION_TIME_US, error_msg);
}

// Test that the mock LCD basic functionality works
void test_mock_lcd_basic_functionality() {
    TEST_ASSERT_EQUAL(320, mock_lcd.width());
    TEST_ASSERT_EQUAL(240, mock_lcd.height());
    
    // Test that fillRect doesn't crash with valid parameters
    mock_lcd.fillRect(0, 0, 100, 100, 0xFFFF);
    TEST_ASSERT_TRUE(true); // If we reach here, fillRect didn't crash
}

// Test boundary conditions
void test_lcd_boundary_conditions() {
    const long MAX_EXECUTION_TIME_US = 1000; // 1ms for boundary cases
    
    // Test zero-size rectangle (should return immediately)
    long execution_time = measureExecutionTime([&]() {
        mock_lcd.fillRect(0, 0, 0, 0, 0xFFFF);
    });
    
    TEST_ASSERT_TRUE(execution_time < MAX_EXECUTION_TIME_US);
    
    // Test out-of-bounds rectangle (should be clipped)
    execution_time = measureExecutionTime([&]() {
        mock_lcd.fillRect(-50, -50, 100, 100, 0xFFFF);
    });
    
    TEST_ASSERT_TRUE(execution_time < MAX_EXECUTION_TIME_US);
}
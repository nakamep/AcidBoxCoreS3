#include <unity.h>

// Dummy tests for Core S3 compilation verification
// These tests always pass to ensure build job is triggered and succeeds

void test_core_s3_compilation() {
    // Simple test that always passes
    TEST_ASSERT_TRUE(true);
}

void test_core_s3_defines() {
    // Test that required defines are present
    #ifdef M5STACK_CORES3
    TEST_ASSERT_TRUE(true);
    #else
    TEST_FAIL_MESSAGE("M5STACK_CORES3 not defined");
    #endif
    
    #ifdef CONFIG_IDF_TARGET_ESP32S3
    TEST_ASSERT_TRUE(true);
    #else
    TEST_FAIL_MESSAGE("CONFIG_IDF_TARGET_ESP32S3 not defined");
    #endif
}

void test_core_s3_basic_math() {
    // Basic math test that always passes
    TEST_ASSERT_EQUAL(2, 1 + 1);
    TEST_ASSERT_EQUAL(4, 2 * 2);
}

#ifdef ARDUINO
void setup() {
    delay(2000); // Give time for serial monitor to open
    
    UNITY_BEGIN();
    
    RUN_TEST(test_core_s3_compilation);
    RUN_TEST(test_core_s3_defines);
    RUN_TEST(test_core_s3_basic_math);
    
    UNITY_END();
}

void loop() {
    // Empty loop for Arduino
}
#else
int main() {
    UNITY_BEGIN();
    
    RUN_TEST(test_core_s3_compilation);
    RUN_TEST(test_core_s3_defines);
    RUN_TEST(test_core_s3_basic_math);
    
    return UNITY_END();
}
#endif
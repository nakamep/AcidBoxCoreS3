#include <unity.h>

// Dummy test for Core S3 compilation verification
// This test always passes to ensure build pipeline works
void test_core_s3_compilation() {
    TEST_ASSERT_TRUE(true);
}

void test_core_s3_defines() {
    // Verify that M5STACK_CORES3 is defined for this environment
    #ifdef M5STACK_CORES3
    TEST_ASSERT_TRUE(true);
    #else
    TEST_FAIL_MESSAGE("M5STACK_CORES3 should be defined for core_s3 environment");
    #endif
}

#ifdef ARDUINO
void setup() {
    delay(2000); // Give time for serial monitor to open
    
    UNITY_BEGIN();
    
    RUN_TEST(test_core_s3_compilation);
    RUN_TEST(test_core_s3_defines);
    
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
    
    return UNITY_END();
}
#endif
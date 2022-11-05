#include <gtest/gtest.h>
#include <vector>
#include "../src/cursor.h"

TEST(CURSOR_TEST, MAIN) {
    std::vector<uint8_t> test_stream{
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, // 0 - 10
            0x8e, 0xc3, 0xfc, 0x44, 0xaa, 0xbb, 0x00, 0x00, 0x00, 0x00, // 11 - 20
            0x00, 0x00, 0x33, 0x44, 0x0C, 0x00, 0x69, 0x61, 0x64, 0x66, // 21 - 30
            0x6B, 0x6C, 0x6A, 0x61, 0x73, 0x66, 0x64, 0x00, 0x0C, 0x00, // 31 - 40
            0x00, 0x00, 0x69, 0x61, 0x64, 0x66, 0x6B, 0x6C, 0x6A, 0x61, // 41 - 50
            0x73, 0x66, 0x64, 0x00, 0x60, 0x0A, 0x10, 0x00, 0x44, 0x45,
            0x56, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E, 0x53, 0x74, 0x72,
            0x69, 0x6E, 0x67, 0x00, 0x10, 0x00, 0x60, 0x0A, 0x48, 0x44,
            0x56, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E, 0x53, 0x74, 0x72,
            0x69, 0x6E, 0x67, 0x00, 0x56, 0x45, 0x52, 0x20, 0x39, 0x2E,
            0x34, 0x00
    };
    auto test_cursor = RecCursor(test_stream);
    std::size_t testpos = 5;

    EXPECT_EQ(test_cursor(), 0);
    EXPECT_EQ(test_cursor(testpos)(), testpos);
    EXPECT_THROW(test_cursor(test_stream.size()), std::string);

    std::size_t readtestpos = 10;
    float testfloat_20221111;
    test_cursor(readtestpos);
    test_cursor >> testfloat_20221111;
    EXPECT_FLOAT_EQ(testfloat_20221111, 2022.1111);
    EXPECT_EQ(test_cursor(), readtestpos + sizeof testfloat_20221111);

    // Read in chain
    int32_t testint_151521030, testint_1144193024;
    uint16_t testuint_48042;
    testfloat_20221111 = 0.0;
    test_cursor(readtestpos);
    test_cursor += -2;
    test_cursor -= 2;
    test_cursor >> testint_151521030
                >> testfloat_20221111
                >> testuint_48042
                >> 4
                >> testint_1144193024;
    EXPECT_EQ(testint_151521030, 151521030);
    EXPECT_FLOAT_EQ(testfloat_20221111, 2022.1111);
    EXPECT_EQ(testuint_48042, 48042);
    EXPECT_EQ(testint_1144193024, 1144193024);
    EXPECT_EQ(test_cursor(), 24);

    // Test expected bytes
    std::vector<uint8_t> test_pattern_container{0xc3, 0xfc, 0x44};
    uint8_t test_pattern_array[] = {0xc3, 0xfc, 0x44};
    uint32_t pattern_pos = 11;
    test_cursor(pattern_pos);
    EXPECT_TRUE(test_cursor.Test(test_pattern_array, sizeof test_pattern_array));
    EXPECT_TRUE(test_cursor.TestAndSkip(test_pattern_container.data(), test_pattern_container.size()));
    EXPECT_EQ(test_cursor(), pattern_pos + test_pattern_container.size());

    // Get pointer to current position
    test_cursor(8);
    EXPECT_EQ((int) *test_cursor.Ptr(), 8);

    // String operation test
    std::string test_string_2byte_pascal, test_string_4byte_pascal, test_de_string, test_hd_string, test_c_string;
    test_cursor(24);
    test_cursor >> test_string_2byte_pascal
                >> test_string_4byte_pascal
                >> test_de_string
                >> test_hd_string;

    EXPECT_EQ(test_string_2byte_pascal, "iadfkljasfd");
    EXPECT_EQ(test_string_4byte_pascal, "iadfkljasfd");
    EXPECT_EQ(test_de_string, "DEVersionString");
    EXPECT_EQ(test_hd_string, "HDVersionString");
    char char_buffer[8];
    test_cursor >> char_buffer;
    test_c_string.assign(char_buffer);
    EXPECT_EQ(test_c_string, "VER 9.4");

    // Cursor maintains a reference to stream, change of stream
    // should infect behavior of cursor.
    int testint_resize;
    test_stream.resize(0);
    EXPECT_THROW(test_cursor >> testint_resize, std::string);
}
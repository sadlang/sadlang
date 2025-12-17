#include <gtest/gtest.h>
#include <iostream>

TEST(SimpleTest, BasicTest) {
    std::cout << "Simple test running!" << std::endl;
    EXPECT_EQ(1 + 1, 2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    std::cout << "Starting tests..." << std::endl;
    return RUN_ALL_TESTS();
}

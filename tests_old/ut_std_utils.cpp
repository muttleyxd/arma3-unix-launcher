#include "gtest/gtest.h"

#include "std_utils.hpp"

#include <algorithm>
#include <string>
#include <vector>

TEST(StdUtils, Contains)
{
    using namespace StdUtils;

    std::vector<int> numbers{1, 2, 4, 5, 6};
    EXPECT_TRUE(Contains(numbers, 1));
    EXPECT_TRUE(Contains(numbers, 2));
    EXPECT_FALSE(Contains(numbers, 3));

    std::vector<std::string> strings{"test", "123", "addons"};
    EXPECT_TRUE(Contains(strings, "test"));
    EXPECT_TRUE(Contains(strings, "addons"));
    EXPECT_FALSE(Contains(strings, "ADDONS"));
}

TEST(StdUtils, ContainsKey)
{
    using namespace StdUtils;

    std::map<int, int> numbers{{0, 1}, {1, 1}, {3, 3}};
    EXPECT_TRUE(ContainsKey(numbers, 0));
    EXPECT_TRUE(ContainsKey(numbers, 1));
    EXPECT_FALSE(ContainsKey(numbers, 2));

    std::map<std::string, std::string> strings{{"test", "testValue"}, {"testKey", "testValue2"}};
    EXPECT_TRUE(ContainsKey(strings, "test"));
    EXPECT_TRUE(ContainsKey(strings, "testKey"));
    EXPECT_FALSE(ContainsKey(strings, "nothing here"));
}

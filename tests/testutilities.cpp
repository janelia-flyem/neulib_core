#include "gtest/gtest.h"
#include "core/utilities.h"

int fn() {
  static int count = 0;

  return ++count;
}

TEST(core, utilities)
{
  {
    int out = 0;
    int in = 1;
    neulib::Assign(&out, in);
    ASSERT_EQ(1, out);
  }

  {
    double out = 0.5;
    double in = 1.0;
    neulib::Assign(&out, in);
    ASSERT_EQ(1.0, out);
  }

  {
    ASSERT_EQ("0x0", neulib::ToString(0x0));
  }

  {
    auto memo = neulib::Memoize(fn);

    ASSERT_EQ(1, memo());
    ASSERT_EQ(1, memo());

    ASSERT_EQ(2, fn());
    ASSERT_EQ(1, memo());
  }
}

int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return ret;
}
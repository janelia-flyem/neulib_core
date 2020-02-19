#include "gtest/gtest.h"
#include "core/utilities.h"

int fn(int s) {
  static int count = s;

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
    ASSERT_EQ("0x1", neulib::ToString((void *)(1)));
  }

  {
    auto memo = neulib::Memoize(fn);

    ASSERT_EQ(1, memo(0));
    ASSERT_EQ(1, memo(0));

    ASSERT_EQ(2, fn(0));
    ASSERT_EQ(1, memo(0));

    ASSERT_EQ(3, memo(1));
    ASSERT_EQ(4, fn(0));
    ASSERT_EQ(1, memo(0));
    ASSERT_EQ(3, memo(1));
  }

  {
    ASSERT_EQ(1, neulib::ClipValue(0, 1, 3));
    ASSERT_EQ(2, neulib::ClipValue(2, 1, 3));
    ASSERT_EQ(3, neulib::ClipValue(5, 1, 3));

    ASSERT_EQ(2, neulib::ClipValue(2, 1, 3));
    ASSERT_EQ(1.0, neulib::ClipValue(0.0, 1.0, 3.0));
    ASSERT_EQ(3.0, neulib::ClipValue(4.0, 1.0, 3.0));

    int x0 = -1;
    int x1 = 5;
    ASSERT_TRUE(neulib::ClipRange(1, 3, x0, x1));
    ASSERT_EQ(1, x0);
    ASSERT_EQ(3, x1);

    x0 = -1;
    x1 = 2;
    ASSERT_TRUE(neulib::ClipRange(1, 3, x0, x1));
    ASSERT_EQ(1, x0);
    ASSERT_EQ(2, x1);

    x0 = 1;
    x1 = 5;
    ASSERT_TRUE(neulib::ClipRange(1, 3, x0, x1));
    ASSERT_EQ(1, x0);
    ASSERT_EQ(3, x1);

    x0 = 1;
    x1 = 3;
    ASSERT_TRUE(neulib::ClipRange(1, 3, x0, x1));
    ASSERT_EQ(1, x0);
    ASSERT_EQ(3, x1);

    x0 = 1;
    x1 = 2;
    ASSERT_TRUE(neulib::ClipRange(1, 3, x0, x1));
    ASSERT_EQ(1, x0);
    ASSERT_EQ(2, x1);

    x0 = 5;
    x1 = -1;
    ASSERT_FALSE(neulib::ClipRange(1, 3, x0, x1));

    x0 = -5;
    x1 = -1;
    ASSERT_FALSE(neulib::ClipRange(1, 3, x0, x1));

    x0 = 5;
    x1 = 10;
    ASSERT_FALSE(neulib::ClipRange(1, 3, x0, x1));
  }
}

int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return ret;
}

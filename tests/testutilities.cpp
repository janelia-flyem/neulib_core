#include "gtest/gtest.h"
#include "core/utilities.h"
#include "core/stringbuilder.h"
#include "core/sharedresourcepool.h"

int fn(int s) {
  static int count = s;

  return ++count;
}

int fn2(std::string s) {
  static int count = 0;

  return count + s.size();
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
    ASSERT_EQ(1, neulib::UnsignedCrop(1));
    ASSERT_EQ(0, neulib::UnsignedCrop(-1));

    ASSERT_EQ(0, neulib::UnsignedCrop(int64_t(-1)));
    ASSERT_EQ(214748364789llu, neulib::UnsignedCrop(int64_t(214748364789ll)));
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
    auto memo = neulib::Memoize(fn2);
    ASSERT_EQ(1, memo("x"));
    ASSERT_EQ(1, memo("x"));
    ASSERT_EQ(2, memo("xy"));
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

TEST(core, StringBuilder)
{
  {
    neulib::StringBuilder builder("");

    ASSERT_TRUE(std::string(builder).empty());
  }

  {
    std::string str = neulib::StringBuilder("test").append(":a").append(1);
    ASSERT_EQ("test:a1", str);
  }

  {
    std::string str = neulib::StringBuilder("test").append(":a").append(1, 3);
    ASSERT_EQ("test:a001", str);
  }

  {
    std::string str =
        neulib::StringBuilder("test").append(":a").append(int64_t(12345678910ll), 3);
    ASSERT_EQ("test:a12345678910", str);
  }

  {
    std::string str =
        neulib::StringBuilder("test").append(":a").append(uint64_t(12345678910ull), 13);
    ASSERT_EQ("test:a0012345678910", str);
  }

  {
    std::string str =
        neulib::StringBuilder("test").append(":a").append(uint64_t(12345678910ull), 13).
        append(":more:").append(uint64_t(12345678910ull));
    ASSERT_EQ("test:a0012345678910:more:12345678910", str);
  }

  {
    std::string str =
      neulib::StringBuilder("from test from").replace("from", "to");
    ASSERT_EQ("to test to", str);

    str =
      neulib::StringBuilder("from test from").replace("from", "");
    ASSERT_EQ(" test ", str);

    str =
      neulib::StringBuilder("from test from").replace("", "to");
    ASSERT_EQ("from test from", str);
  }

  {
    std::string str = neulib::StringBuilder("[$]_[$]/[$]").arg(1).arg("test").arg(1.5f);
    ASSERT_EQ("1_test/1.5", str);

    str = neulib::StringBuilder("[$]_[$]/[$]").arg(1).arg("test");
    ASSERT_EQ("1_test/[$]", str);

    str = neulib::StringBuilder("[$]_[\\$]/[$]").arg(1).arg("test");
    ASSERT_EQ("1_[\\$]/test", str);

    str = neulib::StringBuilder("[$]_[\\$]/[$]").arg(1).arg("test").argend();
    ASSERT_EQ("1_[$]/test", str);
  }
}

TEST(core, SharedResroucePool)
{
  auto factory = []() {
    int *v = new int;
    *v = 1;
    return v;
  };

  auto factory2 = []() {
    int *v = new int;
    *v = 2;
    return v;
  };

  neulib::SharedResourcePool<int> srp(factory);
  std::shared_ptr<int> s = srp.take();
  ASSERT_EQ(1, *s);
  ASSERT_EQ(0, srp.count());
  srp.add(s);
  ASSERT_EQ(1, srp.count());
  std::shared_ptr<int> s2 = srp.take();
  ASSERT_EQ(0, srp.count());
  ASSERT_EQ(s.get(), s2.get());
  auto s3 = srp.take(factory2);
  ASSERT_EQ(2, *s3);
  auto s4 = srp.take();
  ASSERT_EQ(1, *s4);
}

TEST(core, SharedResourcePoolMap)
{
  auto strFactory = [](const std::string &key) {
    std::string *v = new std::string;
    *v = key;
    return v;
  };

  auto strFactory2 = [](const std::string &key) {
    std::string *v = new std::string;
    *v = key + "_2";
    return v;
  };

  neulib::SharedResourcePoolMap<std::string> srpm(strFactory);
  auto v1 = srpm.take("test");
  ASSERT_EQ("test", *v1);

  auto v2 = srpm.take("test2");
  ASSERT_EQ("test2", *v2);

  auto v3 = srpm.take("test", strFactory2);
  ASSERT_EQ("test_2", *v3);

  srpm.add("test", v3);
  srpm.add("test2", v2);
  auto v4 = srpm.take("test");
  ASSERT_EQ(v3.get(), v4.get());

  v4 = srpm.take("test2");
  ASSERT_EQ(v2.get(), v4.get());

  auto v5 = srpm.take("");
  ASSERT_FALSE(v5);
}

TEST(core, SharedResourceRetriever)
{
  auto strFactory = [](const std::string &key) {
    std::string *v = new std::string;
    *v = key;
    return v;
  };

  auto srpmPtr =
      std::shared_ptr<neulib::SharedResourcePoolMap<std::string>>(
          new neulib::SharedResourcePoolMap<std::string>(strFactory));

  std::shared_ptr<std::string> v1;
  {
    neulib::SharedResourceRetriever<std::string> srr(srpmPtr, "test");
    v1 = srr.get();
  }

  auto v2 = srpmPtr->take("test");
  ASSERT_EQ("test", *v2);
  ASSERT_EQ(v1.get(), v2.get());
}

int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return ret;
}

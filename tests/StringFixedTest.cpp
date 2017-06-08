#include <gtest/gtest.h>

#include <StringFixed.hpp>

namespace {


class StringFixedTest : public ::testing::Test
{
protected:
    using SmallString = lang::StringFixed<2>;
    using String = lang::StringFixed<40>;
    using LongString = lang::StringFixed<128>;

    void assertEmpty(String &s) {
      // Size tests
      EXPECT_EQ(0, s.length());

      EXPECT_TRUE(s.empty());
      // Iterator tests
      EXPECT_TRUE(s.begin() == s.end());
    }
};

TEST_F(StringFixedTest, EmptyStringTest) {
    SCOPED_TRACE("EmptyStringTest");
    String str;
    assertEmpty(str);
}

TEST_F(StringFixedTest, NullTermTest) {
    String str;
    EXPECT_EQ(*str.data(), '\0');
}


///////// CONSTRUCTORS
TEST_F(StringFixedTest, ConstructFromCharTest) {
    String str('1');
    EXPECT_STREQ(str.data(), "1");
}

TEST_F(StringFixedTest, ConstructFromCharWithOperatorTest) {
    String str = '1';
    EXPECT_STREQ(str.data(), "1");
}

TEST_F(StringFixedTest, ConstructFromLiteralTest) {
    String str("1234");
    EXPECT_STREQ(str.data(), "1234");
}

TEST_F(StringFixedTest, ConstructFromLiteralWithOperatorTest) {
    String str = "1234";
    EXPECT_STREQ(str.data(), "1234");
}

TEST_F(StringFixedTest, ConstructFromConstCharWithLenTest) {
    const char* cStr = "1234";
    String str(cStr, 4);
    EXPECT_STREQ(str.data(), "1234");
}

TEST_F(StringFixedTest, ConstructFromFixedStringTest) {
    String str1 = "1234";
    String str2(str1);

    EXPECT_STREQ(str1.data(), str2.data());
}

TEST_F(StringFixedTest, ConstructFromFixedStringWithOperatorTest) {
    String str1 = "1234";
    String str2 = str1;

    EXPECT_STREQ(str1.data(), str2.data());
}

TEST_F(StringFixedTest, ConstructFromFixedStringWithDiffTemplateParamsTest) {
    LongString str1 = "12345";
    String str2(str1);

    EXPECT_STREQ(str1.data(), str2.data());
}

TEST_F(StringFixedTest, ConstructFromFixedStringWithDiffTemplateWithOperatorParamsTest) {
    LongString str1 = "12345";
    String str2 = str1;

    EXPECT_STREQ(str1.data(), str2.data());
}

TEST_F(StringFixedTest, ConstructFromStringViewTest) {
    std::experimental::string_view srtV = "123";
    String str(srtV);

    EXPECT_STREQ(str.data(), "123");
}

TEST_F(StringFixedTest, ConstructFromStringViewWithOperatorTest) {
    std::experimental::string_view srtV = "123";
    String str = srtV;

    EXPECT_STREQ(str.data(), "123");
}

///////// SET
TEST_F(StringFixedTest, SetFixedStringFromCstringTest) {
    String str = "1234";
    const char* cstr = "12354\0";
    str.set_cstring(cstr);

    EXPECT_STREQ(str.data(), cstr);
}

///////// COMPARE
TEST_F(StringFixedTest, CompareWithSameTypeTest) {
    SCOPED_TRACE("CompareWithSameTypeTest");
    String str = "1234";
    String strEq = str;
    String strBig = "12345";
    String strSmall = "123";

    EXPECT_TRUE(str.compare(strBig) > 0);
    EXPECT_TRUE(str.compare(strEq) == 0);
    EXPECT_TRUE(str.compare(strSmall) < 0);
}

TEST_F(StringFixedTest, CompareWithConstCharAndCCtringTypeTest) {
    SCOPED_TRACE("CompareWithSameTypeTest");
    String str = "1234";
    const char* strEq = "1234\0";
    const char* strBig = "12345\0";
    const char* strSmall = "123\0";

    EXPECT_TRUE(str.compare(strBig, std::strlen(strBig)) > 0);
    EXPECT_TRUE(str.compare(strEq, std::strlen(strEq)) == 0);
    EXPECT_TRUE(str.compare(strSmall, std::strlen(strSmall)) < 0);

    EXPECT_TRUE(str.compare_cstring(strBig) > 0);
    EXPECT_TRUE(str.compare_cstring(strEq) == 0);
    EXPECT_TRUE(str.compare_cstring(strSmall) < 0);
}

TEST_F(StringFixedTest, CompareWithLiteralTypeTest) {
    SCOPED_TRACE("CompareWithLiteralTypeTest");
    String str = "1234";

    EXPECT_TRUE(str.compare("12345") > 0);
    EXPECT_TRUE(str.compare("1234") == 0);
    EXPECT_TRUE(str.compare("123") < 0);
}

///////// EQUAL
TEST_F(StringFixedTest, EqualWithSameTypeTest) {
    String str1 = "1234";
    String str2 = str1;

    EXPECT_TRUE(str1 == str2);
}

TEST_F(StringFixedTest, EqualWithSameTypeDifferentTemplatesTest) {
    String str1 = "1234";
    LongString str2 = "4321";

    EXPECT_FALSE(str1 == str2);
}

TEST_F(StringFixedTest, EqualWithLiteralTest) {
    SCOPED_TRACE("EqualWithLiteralTest");
    String str = "1234";

    EXPECT_TRUE(str == "1234");
    EXPECT_TRUE("1234" == str);
}

TEST_F(StringFixedTest, EqualWithStringViewTest) {
    SCOPED_TRACE("EqualWithLiteralTest");
    String str = "1234";
    std::experimental::string_view sv(str.data(), str.length());

    EXPECT_TRUE(str == sv);
    EXPECT_TRUE(sv == str);
}

///////// NOT EQUAL
TEST_F(StringFixedTest, NotEqualWithSameTypeTest) {
    String str1 = "1234";
    String str2 = "12345";

    EXPECT_TRUE(str1 != str2);
}

TEST_F(StringFixedTest, NotEqualWithSameTypeDifferentTemplatesTest) {
    String str1 = "1234";
    LongString str2 = "12345";

    EXPECT_TRUE(str1 != str2);
}

TEST_F(StringFixedTest, NotEqualWithLiteralTest) {
    SCOPED_TRACE("NotEqualWithLiteralTest");
    String str = "1234";

    EXPECT_TRUE(str != "12345");
    EXPECT_TRUE("12345" != str);
}

TEST_F(StringFixedTest, NotEqualWithStringViewTest) {
    SCOPED_TRACE("NotEqualWithStringViewTest");
    String str = "1234";
    String otherStr = "12345";
    std::experimental::string_view sv(otherStr.data(), otherStr.length());

    EXPECT_TRUE(str != sv);
    EXPECT_TRUE(sv != str);
}

///////// ITERATOR
TEST_F(StringFixedTest, InteratorTest) {
    String str = "1234";
    String otherStr = "1234";

    EXPECT_TRUE(std::equal(str.begin(), str.end(), otherStr.begin()));
}

///////// CLEAR
TEST_F(StringFixedTest, ClearTest) {
    SCOPED_TRACE("ClearTest");
    String str = "1234";
    str.clear();

    assertEmpty(str);
}

///////// SET BAD ALLOC
TEST_F(StringFixedTest, SetBadAllocTest) {
    SmallString str;
    EXPECT_TRUE(str.set("123") == SmallString::BAD_ALLOC);
}

///////// ADD
TEST_F(StringFixedTest, AddCharTest) {
    String str = "123";
    str.add('4');

    EXPECT_STREQ(str.data(), "1234");
}

TEST_F(StringFixedTest, AddCstringTest) {
    String str = "123";
    char const* cstr = "456\0";
    str.add_cstring(cstr);

    EXPECT_STREQ(str.data(), "123456");
}

TEST_F(StringFixedTest, AddConstCharWithLenTest) {
    String str = "123";
    str.add("456", 3);

    EXPECT_STREQ(str.data(), "123456");
}

TEST_F(StringFixedTest, AddLiteralTest) {
    String str = "123";
    str.add("456");

    EXPECT_STREQ(str.data(), "123456");
}

TEST_F(StringFixedTest, AddStringFixedTest) {
    String str = "123";
    LongString otherStr = "456";
    str.add(otherStr);

    EXPECT_STREQ(str.data(), "123456");
}

TEST_F(StringFixedTest, AddLiteralWithOperatorTest) {
    String str = "123";
    str += "456";

    EXPECT_STREQ(str.data(), "123456");
}

TEST_F(StringFixedTest, AddStringFixedWithOperatorTest) {
    String str = "123";
    LongString otherStr = "456";
    str += otherStr;

    EXPECT_STREQ(str.data(), "123456");
}

TEST_F(StringFixedTest, AddStringViewest) {
    String str = "123";
    std::experimental::string_view srtV = "456";
    str.add(srtV);

    EXPECT_STREQ(str.data(), "123456");
}

TEST_F(StringFixedTest, AddStringViewWithOperatorTest) {
    String str = "123";
    std::experimental::string_view srtV = "456";
    str += srtV;

    EXPECT_STREQ(str.data(), "123456");
}

///////// ADD BAD ALLOC
TEST_F(StringFixedTest, AddBadAllocTest) {
    SmallString str;
    EXPECT_TRUE(str.add("123") == SmallString::BAD_ALLOC);
    EXPECT_FALSE(str.add("12") == SmallString::BAD_ALLOC);
}

///////// POP BACK
TEST_F(StringFixedTest, PopBackTest) {
    String str = "123";
    str.pop_back();

    EXPECT_STREQ(str.data(), "12");
}

///////// BACK
TEST_F(StringFixedTest, BackTest) {
    String str = "123";

    EXPECT_STREQ(&str.back(), "3");
}

}

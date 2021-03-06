#include <gtest/gtest.h>
#include <iv/alloc.h>
#include <iv/ustring.h>
#include <iv/unicode.h>
#include <iv/aero/aero.h>
#include "test_aero.h"

TEST(AeroParserCase, MainTest) {
  iv::core::Space space;
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("main");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
    iv::aero::Dumper dumper;
    EXPECT_TRUE(
        iv::core::ToUString("DIS(ALT(main))") == dumper.Dump(data.pattern()));
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("ma[in]");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("(ma[in])");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("[\\d-a]");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString(kURLRegExp);
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("a{10,}");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("a{10,20}");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("\\a");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
  {
    space.Clear();
    std::array<char16_t, 2> l = { { 92, 99 } };
    iv::core::UString str(l.begin(), l.end());
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
  {
    // see IE Blog
    space.Clear();
    iv::core::UString str = iv::core::ToUString("ma[in]]");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("a{10, 20}");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("a{10,20");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("a{ 10,20}");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("a{a10,20}");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_FALSE(error);
    ASSERT_TRUE(data.pattern());
  }
}

TEST(AeroParserCase, SyntaxInvalidTest) {
  iv::core::Space space;
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("ma[in");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_TRUE(error);
    ASSERT_FALSE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("ma([in]");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_TRUE(error);
    ASSERT_FALSE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("[b-a]");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_TRUE(error);
    ASSERT_FALSE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("[b-aab]");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_TRUE(error);
    ASSERT_FALSE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("a{20,10}");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_TRUE(error);
    ASSERT_FALSE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("{20,10}");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_TRUE(error);
    ASSERT_FALSE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("+");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_TRUE(error);
    ASSERT_FALSE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("*");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_TRUE(error);
    ASSERT_FALSE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("{20}");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_TRUE(error);
    ASSERT_FALSE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("{20,}");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_TRUE(error);
    ASSERT_FALSE(data.pattern());
  }
  {
    space.Clear();
    iv::core::UString str = iv::core::ToUString("{0,2}");
    iv::aero::Parser<iv::core::UStringPiece> parser(&space, str, iv::aero::NONE);
    int error = 0;
    iv::aero::ParsedData data = parser.ParsePattern(&error);
    ASSERT_TRUE(error);
    ASSERT_FALSE(data.pattern());
  }
}

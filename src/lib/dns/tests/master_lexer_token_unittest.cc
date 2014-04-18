// Copyright (C) 2012  Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <exceptions/exceptions.h>

#include <dns/master_lexer.h>

#include <gtest/gtest.h>

#include <string>

using namespace bundy::dns;

namespace {

const char TEST_STRING[] = "string token";
// This excludes the ending \0 character
const size_t TEST_STRING_LEN = sizeof(TEST_STRING) - 1;

class MasterLexerTokenTest : public ::testing::Test {
protected:
    MasterLexerTokenTest() :
        token_eof(MasterToken::END_OF_FILE),
        token_str(TEST_STRING, TEST_STRING_LEN),
        token_num(42),
        token_err(MasterToken::UNEXPECTED_END)
    {}

    const MasterToken token_eof; // an example of non-value type token
    const MasterToken token_str;
    const MasterToken token_num;
    const MasterToken token_err;
};


TEST_F(MasterLexerTokenTest, strings) {
    // basic construction and getter checks
    EXPECT_EQ(MasterToken::STRING, token_str.getType());
    EXPECT_EQ(std::string("string token"), token_str.getString());
    std::string strval = "dummy"; // this should be replaced
    token_str.getString(strval);
    EXPECT_EQ(std::string("string token"), strval);
    const MasterToken::StringRegion str_region =
        token_str.getStringRegion();
    EXPECT_EQ(TEST_STRING, str_region.beg);
    EXPECT_EQ(TEST_STRING_LEN, str_region.len);

    // Even if the stored string contains a nul character (in this case,
    // it happens to be at the end of the string, but could be in the middle),
    // getString() should return a string object containing the nul.
    std::string expected_str("string token");
    expected_str.push_back('\0');
    EXPECT_EQ(expected_str,
              MasterToken(TEST_STRING, TEST_STRING_LEN + 1).getString());
    MasterToken(TEST_STRING, TEST_STRING_LEN + 1).getString(strval);
    EXPECT_EQ(expected_str, strval);

    // Construct type of qstring
    EXPECT_EQ(MasterToken::QSTRING,
              MasterToken(TEST_STRING, sizeof(TEST_STRING), true).
              getType());
    // if we explicitly set 'quoted' to false, it should be normal string
    EXPECT_EQ(MasterToken::STRING,
              MasterToken(TEST_STRING, sizeof(TEST_STRING), false).
              getType());

    // getString/StringRegion() aren't allowed for non string(-variant) types
    EXPECT_THROW(token_eof.getString(), bundy::InvalidOperation);
    EXPECT_THROW(token_eof.getString(strval), bundy::InvalidOperation);
    EXPECT_THROW(token_num.getString(), bundy::InvalidOperation);
    EXPECT_THROW(token_num.getString(strval), bundy::InvalidOperation);
    EXPECT_THROW(token_eof.getStringRegion(), bundy::InvalidOperation);
    EXPECT_THROW(token_num.getStringRegion(), bundy::InvalidOperation);
}

TEST_F(MasterLexerTokenTest, numbers) {
    EXPECT_EQ(42, token_num.getNumber());
    EXPECT_EQ(MasterToken::NUMBER, token_num.getType());

    // It's copyable and assignable.
    MasterToken token(token_num);
    EXPECT_EQ(42, token.getNumber());
    EXPECT_EQ(MasterToken::NUMBER, token.getType());

    token = token_num;
    EXPECT_EQ(42, token.getNumber());
    EXPECT_EQ(MasterToken::NUMBER, token.getType());

    // it's okay to replace it with a different type of token
    token = token_eof;
    EXPECT_EQ(MasterToken::END_OF_FILE, token.getType());

    // Possible max value
    token = MasterToken(0xffffffff);
    EXPECT_EQ(4294967295u, token.getNumber());

    // getNumber() isn't allowed for non number types
    EXPECT_THROW(token_eof.getNumber(), bundy::InvalidOperation);
    EXPECT_THROW(token_str.getNumber(), bundy::InvalidOperation);
}

TEST_F(MasterLexerTokenTest, novalues) {
    // Just checking we can construct them and getType() returns correct value.
    EXPECT_EQ(MasterToken::END_OF_FILE, token_eof.getType());
    EXPECT_EQ(MasterToken::END_OF_LINE,
              MasterToken(MasterToken::END_OF_LINE).getType());
    EXPECT_EQ(MasterToken::INITIAL_WS,
              MasterToken(MasterToken::INITIAL_WS).getType());

    // Special types of tokens cannot have value-based types
    EXPECT_THROW(MasterToken t(MasterToken::STRING), bundy::InvalidParameter);
    EXPECT_THROW(MasterToken t(MasterToken::QSTRING), bundy::InvalidParameter);
    EXPECT_THROW(MasterToken t(MasterToken::NUMBER), bundy::InvalidParameter);
    EXPECT_THROW(MasterToken t(MasterToken::ERROR), bundy::InvalidParameter);
}

TEST_F(MasterLexerTokenTest, errors) {
    EXPECT_EQ(MasterToken::ERROR, token_err.getType());
    EXPECT_EQ(MasterToken::UNEXPECTED_END, token_err.getErrorCode());
    EXPECT_EQ("unexpected end of input", token_err.getErrorText());
    EXPECT_EQ("lexer not started", MasterToken(MasterToken::NOT_STARTED).
              getErrorText());
    EXPECT_EQ("unbalanced parentheses",
              MasterToken(MasterToken::UNBALANCED_PAREN).
              getErrorText());
    EXPECT_EQ("unbalanced quotes", MasterToken(MasterToken::UNBALANCED_QUOTES).
              getErrorText());
    EXPECT_EQ("no token produced", MasterToken(MasterToken::NO_TOKEN_PRODUCED).
              getErrorText());
    EXPECT_EQ("number out of range",
              MasterToken(MasterToken::NUMBER_OUT_OF_RANGE).
              getErrorText());
    EXPECT_EQ("not a valid number",
              MasterToken(MasterToken::BAD_NUMBER).getErrorText());
    EXPECT_EQ("unexpected quotes",
              MasterToken(MasterToken::UNEXPECTED_QUOTES).getErrorText());

    // getErrorCode/Text() isn't allowed for non number types
    EXPECT_THROW(token_num.getErrorCode(), bundy::InvalidOperation);
    EXPECT_THROW(token_num.getErrorText(), bundy::InvalidOperation);

    // Only the pre-defined error code is accepted.  Hardcoding '8' (max code
    // + 1) is intentional; it'd be actually better if we notice it when we
    // update the enum list (which shouldn't happen too often).
    //
    // Note: if you fix this testcase, you probably want to update the
    // getErrorText() tests above too.
    EXPECT_THROW(MasterToken(MasterToken::ErrorCode(8)),
                 bundy::InvalidParameter);

    // Check the coexistence of "from number" and "from error-code"
    // constructors won't cause confusion.
    EXPECT_EQ(MasterToken::NUMBER,
              MasterToken(static_cast<uint32_t>(MasterToken::NOT_STARTED)).
              getType());
}
}

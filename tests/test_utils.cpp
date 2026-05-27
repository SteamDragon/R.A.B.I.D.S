#include <gtest/gtest.h>
#include <utils.h>
#include <cstring>

TEST(TranslitRusEng, BasicRussianWords)
{
    EXPECT_EQ(TranslitRusEng("Привет"), "Privet");
    EXPECT_EQ(TranslitRusEng("Мир"), "Mir");
    EXPECT_EQ(TranslitRusEng("Щука"), "Shhuka");
    EXPECT_EQ(TranslitRusEng("Ёлка"), "Yolka");
}

TEST(TranslitRusEng, EnglishUnchanged)
{
    EXPECT_EQ(TranslitRusEng("Hello"), "Hello");
    EXPECT_EQ(TranslitRusEng("test123"), "test123");
}

TEST(TranslitRusEng, MixedCase)
{
    EXPECT_EQ(TranslitRusEng("Привет World"), "Privet World");
}

TEST(TranslitRusEng, EmptyString)
{
    EXPECT_EQ(TranslitRusEng(""), "");
}

TEST(TranslitRusEng, SpecificChars)
{
    EXPECT_EQ(TranslitRusEng("ъ"), "");
    EXPECT_EQ(TranslitRusEng("Ь"), "'");
    EXPECT_EQ(TranslitRusEng("Э"), "E");
    EXPECT_EQ(TranslitRusEng("Ю"), "Yu");
    EXPECT_EQ(TranslitRusEng("Я"), "Ya");
}

TEST(Trim, Ltrim)
{
    std::string s = "  hello";
    ltrim(s);
    EXPECT_EQ(s, "hello");
}

TEST(Trim, Rtrim)
{
    std::string s = "hello  ";
    rtrim(s);
    EXPECT_EQ(s, "hello");
}

TEST(Trim, Both)
{
    std::string s = "  hello  ";
    trim(s);
    EXPECT_EQ(s, "hello");
}

TEST(Trim, Empty)
{
    std::string s = "";
    trim(s);
    EXPECT_EQ(s, "");
}

TEST(Trim, NoChange)
{
    std::string s = "hello";
    trim(s);
    EXPECT_EQ(s, "hello");
}

TEST(PBKDF2, KnownVector)
{
    const char* pass = "password";
    const unsigned char* salt = (const unsigned char*)"salt";
    char hex[129] = {};
    PBKDF2_HMAC_SHA_512(pass, salt, 1, 64, hex);

    std::string expected = "867f70cf1ade02cff3752599a3a53dc4af34c7a669815ae5d513554e1c8cf252"
                           "c02d470a285a0501bad999bfe943c08f050235d7d68b1da55e63f73b60a57fce";
    EXPECT_EQ(std::string(hex), expected);
}

TEST(PBKDF2, DifferentIterations)
{
    const char* pass = "password";
    const unsigned char* salt = (const unsigned char*)"salt";
    char hex1[129] = {};
    char hex2[129] = {};
    PBKDF2_HMAC_SHA_512(pass, salt, 1, 64, hex1);
    PBKDF2_HMAC_SHA_512(pass, salt, 1000, 64, hex2);
    EXPECT_STRNE(hex1, hex2);
}

TEST(CurrentTime, ReturnsNonEmpty)
{
    std::string t = CurrentTime();
    EXPECT_FALSE(t.empty());
}

TEST(CurrentTime, Format)
{
    std::string t = CurrentTime();
    EXPECT_EQ(t.size(), 19); // "YYYY-MM-DD HH:MM:SS"
}

TEST(RandomString, CorrectLength)
{
    auto result = random_string(16, []() { return 'a'; });
    EXPECT_EQ(result.size(), 16);
    EXPECT_EQ(result, "aaaaaaaaaaaaaaaa");
}

TEST(Charset, NonEmpty)
{
    auto c = charset();
    EXPECT_FALSE(c.empty());
    EXPECT_EQ(c.size(), 62);
}

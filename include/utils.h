#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

using char_array = std::vector<char>;

char_array charset();
std::string CurrentTime();
void PBKDF2_HMAC_SHA_512(const char* pass, const unsigned char* salt, int32_t iterations, uint32_t outputBytes, char* hexResult);
std::string random_string(size_t length, std::function<char(void)> const& rand_char);
std::string TranslitRusEng(const std::string& str);
void ltrim(std::string& s);
void rtrim(std::string& s);
void trim(std::string& s);

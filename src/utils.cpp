#include "utils.h"
#include <cstring>
#include <cstdio>
#include <cctype>
#include <algorithm>
#include <openssl/evp.h>
#include <ctime>

char_array charset()
{
    return char_array(
        {'0', '1', '2', '3', '4',
         '5', '6', '7', '8', '9',
         'A', 'B', 'C', 'D', 'E', 'F',
         'G', 'H', 'I', 'J', 'K',
         'L', 'M', 'N', 'O', 'P',
         'Q', 'R', 'S', 'T', 'U',
         'V', 'W', 'X', 'Y', 'Z',
         'a', 'b', 'c', 'd', 'e', 'f',
         'g', 'h', 'i', 'j', 'k',
         'l', 'm', 'n', 'o', 'p',
         'q', 'r', 's', 't', 'u',
         'v', 'w', 'x', 'y', 'z'});
};

std::string CurrentTime()
{
    char buf[80];
    time_t current_time;
    struct tm local_time;
    time(&current_time);
    localtime_r(&current_time, &local_time);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &local_time);
    return buf;
}

void PBKDF2_HMAC_SHA_512(const char* pass, const unsigned char* salt, int32_t iterations, uint32_t outputBytes, char* hexResult)
{
    std::vector<unsigned char> digest(outputBytes);
    PKCS5_PBKDF2_HMAC(pass, strlen(pass), salt, strlen((char*)salt), iterations, EVP_sha512(), outputBytes, digest.data());
    for (unsigned int i = 0; i < digest.size(); i++)
    {
        sprintf(hexResult + (i * 2), "%02x", 255 & digest[i]);
    }
}

std::string random_string(size_t length, std::function<char(void)> const& rand_char)
{
    std::string str(length, 0);
    std::generate_n(str.begin(), length, rand_char);
    return str;
}

std::string TranslitRusEng(const std::string& str)
{
    std::vector<std::string> rus = {"А", "а", "Б", "б", "В", "в", "Г", "г", "Ґ", "ґ", "Д", "д", "Е", "е", "Є", "є", "Ж", "ж", "З", "з", "И", "и", "І", "і", "Ї", "ї", "Й", "й", "К", "к",
                                    "Л", "л", "М", "м", "Н", "н", "О", "о", "П", "п", "Р", "р", "С", "с", "Т", "т", "У", "у", "Ф", "ф", "Х", "х", "Ц", "ц", "Ч", "ч", "Ш", "ш",
                                    "Щ", "щ", "Ь", "ь", "Ю", "ю", "Я", "я", "Ы", "ы", "Ъ", "ъ", "Ё", "ё", "Э", "э"};

    std::vector<std::string> eng = {"A", "a", "B", "b", "V", "v", "G", "g", "G", "g", "D", "d", "E", "e", "E", "E", "Zh", "zh", "Z", "z", "I", "i", "I", "I", "Yi", "yi", "J", "j", "K", "k",
                                    "L", "l", "M", "m", "N", "n", "O", "o", "P", "p", "R", "r", "S", "s", "T", "t", "U", "u", "F", "f", "H", "h", "Ts", "ts", "ch", "ch", "Sh", "sh",
                                    "Shh", "shh", "'", "'", "Yu", "yu", "Ya", "ya", "Y", "y", "", "", "Yo", "yo", "E", "e"};
    std::string ret;

    for (size_t i = 0; i < str.length(); i++)
    {
        bool found = false;
        for (int j = 0; j < 74; j++)
        {
            if (str.substr(i, 2).compare(rus[j]) == 0)
            {
                ret += eng[j];
                found = true;
                break;
            }
        }

        if (!found)
            ret += str.substr(i, 1);
    }

    return ret;
}

void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                    { return !std::isspace(ch); }));
}

void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                         { return !std::isspace(ch); })
                .base(),
            s.end());
}

void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}

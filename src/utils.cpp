#include "utils.h"
#include <cstring>
#include <cstdio>
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

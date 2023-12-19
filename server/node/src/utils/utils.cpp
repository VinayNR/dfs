#include "utils.h"

// #include <openssl/md5.h>
#include <iomanip>
#include <sstream>

void deleteAndNullifyPointer(char *& ptr, bool isArray) {
    isArray ? delete[] ptr : delete ptr;
    ptr = nullptr;
}

// unsigned int computeMD5(const std::string& input) {
//     unsigned char digest[MD5_DIGEST_LENGTH];
//     MD5((const unsigned char*)input.c_str(), input.length(), digest);

//     // Convert the first 4 bytes of the MD5 hash to an unsigned int
//     unsigned int hashInt = *((unsigned int*)digest);

//     // return the hash int
//     return hashInt;
// }
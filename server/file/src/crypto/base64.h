#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

// Function to base64 encode a string
std::string base64Encode(const std::string& input) {
    BIO* bio = BIO_new(BIO_f_base64());
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO* bioMem = BIO_new(BIO_s_mem());
    bio = BIO_push(bio, bioMem);

    BIO_write(bio, input.c_str(), input.length());
    BIO_flush(bio);

    BUF_MEM* bufferPtr;
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string result(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);

    return result;
}

std::string base64Decode(const std::string& input) {
    // Replace base64url-specific characters
    std::string base64Str = input;
    size_t padding = base64Str.length() % 4;
    if (padding > 0) {
        base64Str.append(4 - padding, '=');
    }
    std::replace(base64Str.begin(), base64Str.end(), '-', '+');
    std::replace(base64Str.begin(), base64Str.end(), '_', '/');

    // Base64 decoding
    BIO* bio = BIO_new(BIO_f_base64());
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO* bioMem = BIO_new_mem_buf(base64Str.c_str(), -1);
    bio = BIO_push(bio, bioMem);

    // Decode
    std::vector<char> buffer(base64Str.size());
    int decodedLength = BIO_read(bio, buffer.data(), base64Str.size());

    // Clean up
    BIO_free_all(bio);

    // Return the decoded string
    return std::string(buffer.data(), decodedLength);
}
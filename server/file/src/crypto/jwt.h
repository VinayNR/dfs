#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

#include "base64.h"

#include "../../../../lib/json.hpp"

using json = nlohmann::json;

/*
* Structs needed for JWT tokens
*/
struct TokenHeader {
    std::string alg;
    std::string typ;

    std::string serialize() const {
        json header;
        header["alg"] = alg;
        header["typ"] = typ;
        return header.dump();
    }

    static TokenHeader deserialize(const std::string& jsonStr) {
        try {
            json root = json::parse(jsonStr);

            TokenHeader header;
            if (root.contains("alg") && root["alg"].is_string()) {
                header.alg = root["alg"];
            }
            if (root.contains("typ") && root["typ"].is_string()) {
                header.typ = root["typ"];
            }
            return header;
        } catch (const std::exception& e) {
            // Handle parsing error
            throw std::runtime_error("Failed to parse JSON: " + std::string(e.what()));
        }
    }
};

struct TokenPayload {
    std::string sub;
    std::string iss;
    std::string exp;

    std::string serialize() {
        json payload;
        payload["sub"] = sub;
        payload["iss"] = iss;
        payload["exp"] = exp;

        return payload.dump();
    }

    static TokenPayload deserialize(const std::string& jsonStr) {
        try {
            json root = json::parse(jsonStr);

            std::cout << "Json string: " << jsonStr << std::endl;

            TokenPayload payload;
            if (root.contains("sub") && root["sub"].is_string()) {
                payload.sub = root["sub"];
            }
            if (root.contains("iss") && root["iss"].is_string()) {
                payload.iss = root["iss"];
            }
            if (root.contains("exp") && root["exp"].is_string()) {
                payload.exp = root["exp"];
            }
            return payload;
        } catch (const std::exception& e) {
            // Handle parsing error
            throw std::runtime_error("Failed to parse JSON: " + std::string(e.what()));
        }
    }
};

std::string sign_hmac_sha256(const std::string& message, const std::string& key) {
    EVP_PKEY *pkey = EVP_PKEY_new_raw_private_key(EVP_PKEY_HMAC, nullptr,
                                                  reinterpret_cast<const unsigned char*>(key.c_str()), key.length());
    if (!pkey) {
        // Handle error
        return "";
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        EVP_PKEY_free(pkey);
        // Handle error
        return "";
    }

    if (EVP_DigestSignInit(ctx, nullptr, EVP_sha256(), nullptr, pkey) <= 0 ||
        EVP_DigestSignUpdate(ctx, message.c_str(), message.length()) <= 0) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(ctx);
        // Handle error
        return "";
    }

    size_t sig_len;
    if (EVP_DigestSignFinal(ctx, nullptr, &sig_len) <= 0) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(ctx);
        // Handle error
        return "";
    }

    std::vector<unsigned char> signature(sig_len);
    if (EVP_DigestSignFinal(ctx, signature.data(), &sig_len) <= 0) {
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(ctx);
        // Handle error
        return "";
    }

    EVP_PKEY_free(pkey);
    EVP_MD_CTX_free(ctx);

    std::stringstream ss;
    for (size_t i = 0; i < sig_len; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(signature[i]);
    }

    return ss.str();
}

// Function to create a JWT token
std::string createJWTToken(const std::string& header, const std::string& payload, const std::string& key) {
    std::string encoded_header = base64Encode(header);
    std::string encoded_payload = base64Encode(payload);

    std::string signature_input = encoded_header + "." + encoded_payload;
    std::string signature = sign_hmac_sha256(signature_input, key);

    return encoded_header + "." + encoded_payload + "." + signature;
}

bool verifyTokenSingature(const std::string& token, const std::string& key) {
    // Split the token into three parts: header, payload, and signature
    std::cout << "Verifying token" << std::endl;

    std::size_t firstDot = token.find('.');
    std::size_t lastDot = token.rfind('.');

    if (firstDot == std::string::npos || lastDot == std::string::npos || firstDot == lastDot) {
        // Invalid token format
        return false;
    }

    std::string encoded_header = token.substr(0, firstDot);
    std::string encoded_payload = token.substr(firstDot + 1, lastDot - firstDot - 1);
    std::string signature = token.substr(lastDot + 1);

    // Perform verification logic using the key
    std::string signature_input = encoded_header + "." + encoded_payload;
    std::string calculated_signature = sign_hmac_sha256(signature_input, key);

    return (signature == calculated_signature);
}
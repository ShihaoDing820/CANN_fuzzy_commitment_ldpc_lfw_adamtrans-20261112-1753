#include "fuzzy_commitment_lfw_crypto.h"

#include <openssl/crypto.h>
#include <openssl/sha.h>

#include <stdexcept>

FuzzyCommitmentLfwDigest FuzzyCommitmentLfwSha256(
    const std::uint8_t* data,
    std::size_t size)
{
    FuzzyCommitmentLfwDigest digest{};
    if (SHA256(data, size, digest.data()) == nullptr) {
        throw std::runtime_error("SHA-256 failed");
    }
    return digest;
}

bool FuzzyCommitmentLfwDigestEqual(
    const FuzzyCommitmentLfwDigest& left,
    const FuzzyCommitmentLfwDigest& right)
{
    return CRYPTO_memcmp(left.data(), right.data(), left.size()) == 0;
}


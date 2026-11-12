#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

constexpr std::size_t kFuzzyCommitmentLfwDigestBytes = 32;
using FuzzyCommitmentLfwDigest =
    std::array<std::uint8_t, kFuzzyCommitmentLfwDigestBytes>;

FuzzyCommitmentLfwDigest FuzzyCommitmentLfwSha256(
    const std::uint8_t* data,
    std::size_t size);

bool FuzzyCommitmentLfwDigestEqual(
    const FuzzyCommitmentLfwDigest& left,
    const FuzzyCommitmentLfwDigest& right);


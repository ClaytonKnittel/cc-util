#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

#include "absl/numeric/bits.h"

namespace util {

template <size_t N>
struct BitSetRepr {
  using value = std::conditional_t<
      N <= 8, uint8_t,
      std::conditional_t<N <= 16, uint16_t,
                         std::conditional_t<N <= 32, uint32_t, uint64_t>>>;
};

template <size_t N, typename I = BitSetRepr<N>::value>
class BitSet {
  static constexpr size_t kBitsPerEntry = std::numeric_limits<I>::digits;
  static constexpr size_t kArraySize = (N + kBitsPerEntry - 1) / kBitsPerEntry;

  // A mask over the bits that are part of the BitSet in the last entry of
  // `data_`.
  static constexpr I kRemainderMask = (I(0x1) << (N % kBitsPerEntry)) - 1;

 public:
  constexpr BitSet() = default;

  constexpr BitSet(const BitSet&) = default;
  constexpr BitSet& operator=(const BitSet&) = default;

  // Bitwise AND/OR/XOR.
  BitSet& operator&=(const BitSet& b);
  BitSet& operator|=(const BitSet& b);
  BitSet& operator^=(const BitSet& b);

  // Bitwise NOT.
  BitSet operator~() const;

  // Returns the value of the bit at `pos`.
  constexpr bool Test(size_t pos) const;

  // Sets the bit at position `pos` to `value` (default `true`).
  constexpr BitSet& Set(size_t pos, bool value = true);

  // Resets (zeros) the bit at position `pos`.
  constexpr BitSet& Reset(size_t pos);

  // Flips the bit at position `pos`.
  constexpr BitSet& Flip(size_t pos);

  // Returns the count of `true` bits in the BitSet.
  constexpr size_t Popcount() const;

  // Counts the number of consecutive leading zeros, starting from the
  // highest-position bit.
  constexpr size_t LeadingZeros() const;

  // Counts the number of consecutive leading ones, starting from the
  // highest-position bit.
  constexpr size_t LeadingOnes() const;

  // Counts the number of consecutive trailing zeros, starting from the
  // lowest-position bit.
  constexpr size_t TrailingZeros() const;

  // Counts the number of consecutive trailing ones, starting from the
  // lowest-position bit.
  constexpr size_t TrailingOnes() const;

 private:
  // Returns the index into data and the index into the number at that index of
  // the bit at position `pos` as a pair.
  constexpr static std::pair<size_t, uint32_t> Idx(size_t pos);

  I data_[kArraySize] = {};
};

template <size_t N, typename I>
BitSet<N, I>& BitSet<N, I>::operator&=(const BitSet<N, I>& b) {
  for (size_t idx = 0; idx < kArraySize; idx++) {
    data_[idx] &= b.data_[idx];
  }
}

template <size_t N, typename I>
BitSet<N, I>& BitSet<N, I>::operator|=(const BitSet<N, I>& b) {
  for (size_t idx = 0; idx < kArraySize; idx++) {
    data_[idx] |= b.data_[idx];
  }
}

template <size_t N, typename I>
BitSet<N, I>& BitSet<N, I>::operator^=(const BitSet<N, I>& b) {
  for (size_t idx = 0; idx < kArraySize; idx++) {
    data_[idx] ^= b.data_[idx];
  }
}

template <size_t N, typename I>
BitSet<N, I> BitSet<N, I>::operator~() const {
  BitSet b;
  for (size_t idx = 0; idx < kArraySize; idx++) {
    b.data_[idx] = ~data_[idx];
    if (idx == kArraySize - 1) {
      b.data_[idx] &= kRemainderMask;
    }
  }
  return b;
}

template <size_t N, typename I>
constexpr bool BitSet<N, I>::Test(size_t pos) const {
  auto [idx, bidx] = Idx(pos);
  return (data_[idx] >> bidx) & 0x1;
}

template <size_t N, typename I>
constexpr BitSet<N, I>& BitSet<N, I>::Set(size_t pos, bool value) {
  auto [idx, bidx] = Idx(pos);
  if (value) {
    data_[idx] |= I(0x1) << bidx;
  } else {
    data_[idx] &= ~(I(0x1) << bidx);
  }
  return *this;
}

template <size_t N, typename I>
constexpr BitSet<N, I>& BitSet<N, I>::Reset(size_t pos) {
  return Set(pos, /*value=*/false);
}

template <size_t N, typename I>
constexpr BitSet<N, I>& BitSet<N, I>::Flip(size_t pos) {
  auto [idx, bidx] = Idx(pos);
  data_[idx] ^= I(0x1) << bidx;
  return *this;
}

template <size_t N, typename I>
constexpr size_t BitSet<N, I>::Popcount() const {
  size_t cnt = 0;
  for (I val : data_) {
    cnt += absl::popcount(val);
  }
  return cnt;
}

template <size_t N, typename I>
constexpr size_t BitSet<N, I>::LeadingZeros() const {
  for (size_t idx = kArraySize - 1; idx < kArraySize; idx--) {
    size_t leading_zeros = absl::countl_zero(data_[idx]);
    if (leading_zeros < kBitsPerEntry) {
      return N + leading_zeros - (idx + 1) * kBitsPerEntry;
    }
  }
  return N;
}

template <size_t N, typename I>
constexpr size_t BitSet<N, I>::LeadingOnes() const {
  for (size_t idx = kArraySize - 1; idx < kArraySize; idx--) {
    size_t leading_ones;
    if (idx == kArraySize - 1) {
      leading_ones = absl::countl_one(~kRemainderMask | data_[idx]);
    } else {
      leading_ones = absl::countl_one(data_[idx]);
    }
    if (leading_ones < kBitsPerEntry) {
      return N + leading_ones - (idx + 1) * kBitsPerEntry;
    }
  }
  return N;
}

template <size_t N, typename I>
constexpr size_t BitSet<N, I>::TrailingZeros() const {
  for (size_t idx = 0; idx < kArraySize; idx++) {
    size_t trailing_zeros = absl::countr_zero(data_[idx]);
    if (trailing_zeros < kBitsPerEntry) {
      return trailing_zeros + idx * kBitsPerEntry;
    }
  }
  return N;
}

template <size_t N, typename I>
constexpr size_t BitSet<N, I>::TrailingOnes() const {
  for (size_t idx = 0; idx < kArraySize; idx++) {
    size_t trailing_ones = absl::countr_one(data_[idx]);
    if (trailing_ones < kBitsPerEntry) {
      return trailing_ones + idx * kBitsPerEntry;
    }
  }
  return N;
}

/* static */
template <size_t N, typename I>
constexpr std::pair<size_t, uint32_t> BitSet<N, I>::Idx(size_t pos) {
  return std::make_pair(pos / kBitsPerEntry, pos % kBitsPerEntry);
}

}  // namespace util

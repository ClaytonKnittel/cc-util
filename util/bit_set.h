#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>
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

template <size_t N, typename I>
class BitSet;

template <size_t N, typename I, typename C>
class BitSetIterator {
  friend BitSet<N, I>;

 public:
  using value_type = size_t;
  using reference = const size_t&;
  using pointer = const size_t*;
  using difference_type = size_t;
  using iterator_category = std::forward_iterator_tag;

  using BitSetT =
      std::conditional_t<C::value, const BitSet<N, I>, BitSet<N, I>>;

  constexpr BitSetIterator(const BitSetIterator&) = default;
  constexpr BitSetIterator& operator=(const BitSetIterator&) = default;

  // Returns the index of this set bit in the BitSet.
  constexpr value_type operator*() const;

  constexpr bool operator==(const BitSetIterator&) const;
  constexpr bool operator!=(const BitSetIterator&) const;

  constexpr BitSetIterator& operator++();
  constexpr BitSetIterator operator++(int);

  template <typename U = C>
  constexpr typename std::enable_if_t<!U::value, void> ClearAt();

 private:
  // Constructs the end() iterator.
  BitSetIterator();
  // Constructs an iterator starting at the given position.
  explicit BitSetIterator(BitSetT& bit_set, size_t starting_pos = 0);

  void FindNextBit();

  BitSetT* bit_set_;
  size_t idx_;
  uint32_t bidx_;
  I cache_;
};

template <size_t N, typename I = BitSetRepr<N>::value>
class BitSet {
  friend BitSetIterator<N, I, std::true_type>;
  friend BitSetIterator<N, I, std::false_type>;

  static constexpr size_t kBitsPerEntry = std::numeric_limits<I>::digits;
  static constexpr size_t kArraySize = (N + kBitsPerEntry - 1) / kBitsPerEntry;

  // A mask over the bits that are part of the BitSet in the last entry of
  // `data_`.
  static constexpr I kRemainderMask = (I(0x1) << (N % kBitsPerEntry)) - 1;

 public:
  using value_type = size_t;
  using const_reference = const size_t&;
  using const_pointer = const size_t*;
  using iterator = BitSetIterator<N, I, std::false_type>;
  using const_iterator = BitSetIterator<N, I, std::true_type>;

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

  // Counts the number of consecutive trailing zeros, starting from `from` and
  // checking consecutive higher-index bits. If the bit at `from` is set, `from
  // is returned.
  constexpr size_t TrailingZeros(size_t from = 0) const;

  // Counts the number of consecutive trailing ones, starting from `from` and
  // checking consecutive higher-index bits. If the bit at `from` is not set,
  // `from` is returned.
  constexpr size_t TrailingOnes(size_t from = 0) const;

  constexpr iterator begin(size_t from = 0);
  constexpr const_iterator begin(size_t from = 0) const;
  constexpr iterator end();
  constexpr const_iterator end() const;

 private:
  // Returns the index into data and the index into the number at that index of
  // the bit at position `pos` as a pair.
  constexpr static std::pair<size_t, uint32_t> Idx(size_t pos);

  I data_[kArraySize] = {};
};

template <size_t N, typename I, typename C>
constexpr size_t BitSetIterator<N, I, C>::operator*() const {
  return idx_ * BitSet<N, I>::kBitsPerEntry + bidx_;
}

template <size_t N, typename I, typename C>
constexpr bool BitSetIterator<N, I, C>::operator==(
    const BitSetIterator& it) const {
  return idx_ == it.idx_ && bidx_ == it.bidx_;
}

template <size_t N, typename I, typename C>
constexpr bool BitSetIterator<N, I, C>::operator!=(
    const BitSetIterator& it) const {
  return !(*this == it);
}

template <size_t N, typename I, typename C>
constexpr BitSetIterator<N, I, C>& BitSetIterator<N, I, C>::operator++() {
  FindNextBit();
  return *this;
}

template <size_t N, typename I, typename C>
constexpr BitSetIterator<N, I, C> BitSetIterator<N, I, C>::operator++(int) {
  BitSetIterator it = *this;
  ++(*this);
  return it;
}

template <size_t N, typename I, typename C>
template <typename U>
constexpr typename std::enable_if_t<!U::value, void>
BitSetIterator<N, I, C>::ClearAt() {
  bit_set_->data_[idx_] &= ~(I(0x1) << bidx_);
}

template <size_t N, typename I, typename C>
BitSetIterator<N, I, C>::BitSetIterator()
    : bit_set_(nullptr), idx_(BitSet<N, I>::kArraySize), bidx_(0) {}

template <size_t N, typename I, typename C>
BitSetIterator<N, I, C>::BitSetIterator(BitSetT& bit_set, size_t starting_pos)
    : bit_set_(&bit_set),
      idx_((starting_pos + BitSet<N, I>::kBitsPerEntry - 1) /
               BitSet<N, I>::kBitsPerEntry -
           1),
      cache_(
          starting_pos != 0
              ? bit_set.data_[idx_] &
                    ~((I(0x1) << (starting_pos % BitSet<N, I>::kBitsPerEntry)) -
                      1)
              : 0) {
  FindNextBit();
}

template <size_t N, typename I, typename C>
void BitSetIterator<N, I, C>::FindNextBit() {
  while (cache_ == 0) {
    idx_++;
    bidx_ = 0;
    if (idx_ == BitSet<N, I>::kArraySize) {
      return;
    }
    cache_ = bit_set_->data_[idx_];
  }

  bidx_ = absl::countr_zero(cache_);
  cache_ &= ~(I(0x1) << bidx_);
}

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
constexpr size_t BitSet<N, I>::TrailingZeros(size_t from) const {
  auto [idx, bidx] = Idx(from);
  size_t trailing_zeros =
      absl::countr_zero(data_[idx] & ~((I(0x1) << bidx) - 1));
  if (trailing_zeros < kBitsPerEntry) {
    return trailing_zeros + idx * kBitsPerEntry;
  }
  for (idx++; idx < kArraySize; idx++) {
    size_t trailing_zeros = absl::countr_zero(data_[idx]);
    if (trailing_zeros < kBitsPerEntry) {
      return trailing_zeros + idx * kBitsPerEntry;
    }
  }
  return N;
}

template <size_t N, typename I>
constexpr size_t BitSet<N, I>::TrailingOnes(size_t from) const {
  auto [idx, bidx] = Idx(from);
  size_t trailing_ones = absl::countr_one(data_[idx] | ((I(0x1) << bidx) - 1));
  if (trailing_ones < kBitsPerEntry) {
    return trailing_ones + idx * kBitsPerEntry;
  }
  for (idx++; idx < kArraySize; idx++) {
    size_t trailing_ones = absl::countr_one(data_[idx]);
    if (trailing_ones < kBitsPerEntry) {
      return trailing_ones + idx * kBitsPerEntry;
    }
  }
  return N;
}

template <size_t N, typename I>
constexpr BitSet<N, I>::iterator BitSet<N, I>::begin(size_t from) {
  return BitSetIterator<N, I, std::false_type>(*this, /*starting_pos=*/from);
}

template <size_t N, typename I>
constexpr BitSet<N, I>::const_iterator BitSet<N, I>::begin(size_t from) const {
  return BitSetIterator<N, I, std::true_type>(*this, /*starting_pos=*/from);
}

template <size_t N, typename I>
constexpr BitSet<N, I>::iterator BitSet<N, I>::end() {
  return BitSetIterator<N, I, std::false_type>();
}

template <size_t N, typename I>
constexpr BitSet<N, I>::const_iterator BitSet<N, I>::end() const {
  return BitSetIterator<N, I, std::true_type>();
}

/* static */
template <size_t N, typename I>
constexpr std::pair<size_t, uint32_t> BitSet<N, I>::Idx(size_t pos) {
  return std::make_pair(pos / kBitsPerEntry, pos % kBitsPerEntry);
}

}  // namespace util

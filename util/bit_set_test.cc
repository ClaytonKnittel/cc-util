#include "util/bit_set.h"

#include <cstddef>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace util {

TEST(BitSetTest, TestSizes) {
  EXPECT_EQ(sizeof(BitSet<1>), 1);
  EXPECT_EQ(sizeof(BitSet<8>), 1);
  EXPECT_EQ(sizeof(BitSet<9>), 2);
  EXPECT_EQ(sizeof(BitSet<16>), 2);
  EXPECT_EQ(sizeof(BitSet<17>), 4);
  EXPECT_EQ(sizeof(BitSet<32>), 4);
  EXPECT_EQ(sizeof(BitSet<33>), 8);
  EXPECT_EQ(sizeof(BitSet<64>), 8);
  EXPECT_EQ(sizeof(BitSet<127>), 16);
  EXPECT_EQ(sizeof(BitSet<1024>), 128);
}

TEST(BitSetTest, TestEmpty) {
  static constexpr size_t kSize = 500;
  BitSet<kSize> b;

  for (size_t pos = 0; pos < kSize; pos++) {
    EXPECT_FALSE(b.Test(pos));
  }

  EXPECT_EQ(b.Popcount(), 0);
  EXPECT_EQ(b.LeadingZeros(), kSize);
  EXPECT_EQ(b.LeadingOnes(), 0);
  EXPECT_EQ(b.TrailingZeros(), kSize);
  EXPECT_EQ(b.TrailingOnes(), 0);
}

TEST(BitSetTest, TestFull) {
  static constexpr size_t kSize = 489;
  BitSet<kSize> b = ~BitSet<kSize>();

  for (size_t pos = 0; pos < kSize; pos++) {
    EXPECT_TRUE(b.Test(pos));
  }

  EXPECT_EQ(b.Popcount(), kSize);
  EXPECT_EQ(b.LeadingZeros(), 0);
  EXPECT_EQ(b.LeadingOnes(), kSize);
  EXPECT_EQ(b.TrailingZeros(), 0);
  EXPECT_EQ(b.TrailingOnes(), kSize);
}

TEST(BitSetTest, TestSingleBit) {
  static constexpr size_t kSize = 189;
  BitSet<kSize> b;

  for (size_t pos = 0; pos < kSize; pos++) {
    BitSet<kSize> s = b;
    s.Set(pos);

    EXPECT_EQ(s.Popcount(), 1);
    EXPECT_EQ(s.LeadingZeros(), kSize - pos - 1);
    EXPECT_EQ(s.LeadingOnes(), pos == kSize - 1 ? 1 : 0);

    for (size_t from = 0; from < kSize; from++) {
      EXPECT_EQ(s.TrailingZeros(from), from <= pos ? pos : kSize);
      EXPECT_EQ(s.TrailingOnes(from), from == pos ? pos + 1 : from);
    }
  }
}

TEST(BitSetTest, TestIterate) {
  static constexpr size_t kSize = 403;
  BitSet<kSize> b;

  b.Set(10);
  b.Set(11);
  b.Set(12);
  b.Set(13);
  b.Set(14);
  b.Set(15);
  b.Set(250);
  b.Set(255);

  EXPECT_THAT(b, testing::ElementsAre(10, 11, 12, 13, 14, 15, 250, 255));
}

TEST(BitSetTest, TestIterateFull) {
  static constexpr size_t kSize = 444;
  BitSet<kSize> b = ~BitSet<kSize>();

  std::vector<size_t> els;
  els.reserve(kSize);
  for (size_t i = 0; i < kSize; i++) {
    els.push_back(i);
  }
  EXPECT_THAT(b, testing::ElementsAreArray(els));
}

TEST(BitSetTest, TestIterateFrom) {
  static constexpr size_t kSize = 444;
  const BitSet<kSize> b = ~BitSet<kSize>();

  std::vector<size_t> els;
  els.reserve(kSize);
  for (size_t i = 55; i < kSize; i++) {
    els.push_back(i);
  }
  EXPECT_THAT(els, testing::ElementsAreArray(b.begin(/*from=*/55), b.end()));
}

TEST(BitSetTest, TestIterateFromMultiple) {
  static constexpr size_t kSize = 88;
  const BitSet<kSize> b = ~BitSet<kSize>();

  std::vector<size_t> els;
  els.reserve(kSize);
  for (size_t i = 64; i < kSize; i++) {
    els.push_back(i);
  }
  EXPECT_THAT(els, testing::ElementsAreArray(b.begin(/*from=*/64), b.end()));
}

TEST(BitSetTest, TestClearIterator) {
  static constexpr size_t kSize = 222;
  BitSet<kSize> b;

  b.Set(81);
  b.Set(12);
  b.Set(14);
  b.Set(0);
  b.Set(88);
  b.Set(220);

  auto it = b.begin();
  it.ClearAt();
  ++it;
  ++it;
  ++it;
  it.ClearAt();
  ++it;
  ++it;
  it.ClearAt();
  ++it;

  EXPECT_EQ(it, b.end());
  EXPECT_THAT(b, testing::ElementsAre(12, 14, 88));
}

}  // namespace util

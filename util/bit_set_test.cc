#include "util/bit_set.h"

#include <cstddef>

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
    EXPECT_EQ(s.TrailingZeros(), pos);
    EXPECT_EQ(s.TrailingOnes(), pos == 0 ? 1 : 0);
  }
}

}  // namespace util

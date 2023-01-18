#include <vector>
#include <cassert>


template <typename T>
static inline T GetIntelValue(const char* RMsgBuf, int BitStart, int BitLen, bool Sign)
{
  // make a 64 bit value
  int64_t RMsgInt = 0;
  std::memcpy(&RMsgInt, RMsgBuf, 8);

  // move it left
  RMsgInt = RMsgInt << (64 - (BitStart + BitLen));

  // move to the right end and fill with sign bits
  RMsgInt = RMsgInt >> (64 - BitLen);

  // check sign
  bool sign = false;
  if (Sign) sign = (RMsgInt & 0x8000000000000000) != 0;
  else     RMsgInt = RMsgInt & ~(0xFFFFFFFFFFFFFFFF << BitLen);

  // set value
  if (sign)
  {
    return static_cast<T>(static_cast<int64_t>(RMsgInt));
  }
  else
  {
    return static_cast<T>(static_cast<uint64_t>(RMsgInt));
  }
}

float get_float(const std::vector<char>& input, int byte_offset, int bit_offset, int length_bits)
{
  assert(bit_offset == 0);
  assert(length_bits == 32);

  float ret{ 0.0f };
  std::memcpy(&ret, &input[byte_offset], sizeof(float));
  return ret;
}

double get_double(const std::vector<char>& input, int byte_offset, int bit_offset, int length_bits)
{
  assert(bit_offset == 0);
  assert(length_bits == 64);

  double ret{ 0.0f };
  std::memcpy(&ret, &input[byte_offset], sizeof(double));
  return ret;
}

template <typename T>
T get_value(const std::vector<char>& input, int byte_offset, int bit_offset, int length_bits, float scale, float offset, bool is_signed, double min, double max)
{
  assert(scale == 1.0f);
  assert(offset == 0.0f);

  T value = GetIntelValue<T>(&input[byte_offset], bit_offset, length_bits, is_signed);

  // assert that conversion is in min / max range
  //assert(value >= min);
  //assert(value <= max);
  return value;
}

template <>
bool get_value(const std::vector<char>& input, int byte_offset, int bit_offset, int length_bits, float scale, float offset, bool is_signed, double min, double max)
{
  uint8_t value = GetIntelValue<uint8_t>(&input[byte_offset], bit_offset, length_bits, is_signed);
  bool ret = value != 0;
  return ret;
}

template <>
double get_value(const std::vector<char>& input, int byte_offset, int bit_offset, int length_bits, float scale, float offset, bool is_signed, double min, double max)
{
  double ret{ 0.0 };

  if (is_signed)
  {
    int64_t value = GetIntelValue<int64_t>(&input[byte_offset], bit_offset, length_bits, is_signed);
    ret = value * scale + offset;
  }
  else
  {
    uint64_t value = GetIntelValue<uint64_t>(&input[byte_offset], bit_offset, length_bits, is_signed);
    ret = value * scale + offset;
  }

  // assert that conversion is in min / max range
  //assert(ret >= min);
  //assert(ret <= max);
  return ret;
}

//template<> uint64_t get_value<uint64_t>(const std::vector<char>& input, int byte_offset, int bit_offset, int length_bits, float scale, float offset, bool is_signed, double min, double max);
//template<> int64_t get_value<int64_t>(const std::vector<char>& input, int byte_offset, int bit_offset, int length_bits, float scale, float offset, bool is_signed, double min, double max);

#pragma once

#include "MemoryBitStream.hpp"

namespace csgotools {

    class DemoMemoryBitStream : public MemoryBitStream {
        static constexpr int32 kMaxVarInt64Bytes = 10;
        static constexpr int32 kMaxVarInt32Bytes = 5;
        static constexpr int32 kCoordIntegerBits = 14;
        static constexpr int32 kCoordFractionalBits = 5;
        static constexpr int32 kCoordDenominator = (1 << DemoMemoryBitStream::kCoordFractionalBits);
        static constexpr int32 kCoordResolution = static_cast<int32>(1.0f / DemoMemoryBitStream::kCoordDenominator);
        static constexpr int32 kCoordIntegerBitsMP = 11;
        static constexpr int32 kCoordFractionalBitsMPLowPrecision = 3;
        static constexpr int32 kNormalFractionalBits = 11;
        static constexpr int32 kNormalDominator = ((1 << DemoMemoryBitStream::kNormalFractionalBits) - 1);
        static constexpr int32 kNormalResolution = static_cast<int32>(1.0f / DemoMemoryBitStream::kNormalDominator);
        static constexpr int32 kCoordDenominatorLowPrecision = (1 << DemoMemoryBitStream::kCoordFractionalBitsMPLowPrecision);
        static constexpr int32 kCoordResolutionLowPrecision = static_cast<int32>(1.0f / DemoMemoryBitStream::kCoordDenominatorLowPrecision);
    public:

        DemoMemoryBitStream() {}

        DemoMemoryBitStream(const char* buffer, const uint32& num_bytes)
            : MemoryBitStream{buffer, num_bytes} {}

        explicit DemoMemoryBitStream(const MemoryBitStream& other)
            : MemoryBitStream{other} {}

        explicit DemoMemoryBitStream(MemoryBitStream&& other)
            : MemoryBitStream{other} {}

        DemoMemoryBitStream(const DemoMemoryBitStream& other)
            : MemoryBitStream(other) {}

        DemoMemoryBitStream(DemoMemoryBitStream&& other) noexcept
            : MemoryBitStream(std::move(other)) {}

        DemoMemoryBitStream& operator=(const DemoMemoryBitStream& other) {
            if (this == &other)
                return *this;
            MemoryBitStream::operator =(other);
            return *this;
        }

        DemoMemoryBitStream& operator=(DemoMemoryBitStream&& other) noexcept {
            if (this == &other)
                return *this;
            MemoryBitStream::operator =(std::move(other));
            return *this;
        }

        int32 ReadVarInt32() {
            uint32 result = 0;
            int32 count = 0;
            uint32 b;

            do {
                if (count == DemoMemoryBitStream::kMaxVarInt32Bytes) {
                    return result;
                }

                b = ReadByte();
                result |= (b & 0x7F) << (7 * count);
                ++count;
            } while (b & 0x80);

            return result;
        }

        int64 ReadVarInt64() {
            uint64 result = 0;
            int32 count = 0;
            uint64 b;

            do {
                if (count == DemoMemoryBitStream::kMaxVarInt64Bytes) {
                    return result;
                }
                b = ReadUBitLong(8);
                result |= static_cast<uint64>(b & 0x7F) << (7 * count);
                ++count;
            } while (b & 0x80);

            return result;
        }

        int32 ReadSignedVarInt32() { return ZigZagDecode32(ReadVarInt32()); }
        int64 ReadSignedVarInt64() { return ZigZagDecode64(ReadVarInt64()); }

        float ReadBitCoord() {
            int32 int_val;
            int32 fract_val;
            int32 sign_bit;
            float value = 0.0f;

            int_val = ReadBit();
            fract_val = ReadBit();

            if (int_val || fract_val) {
                sign_bit = ReadBit();

                if (int_val) {
                    int_val = ReadUBitLong(DemoMemoryBitStream::kCoordIntegerBits) + 1;
                }

                if (fract_val) {
                    fract_val = ReadUBitLong(DemoMemoryBitStream::kCoordFractionalBits);
                }

                value = int_val + (static_cast<float>(fract_val) * DemoMemoryBitStream::kCoordResolution);

                if (sign_bit) {
                    value = -value;
                }
            }

            return value;
        }

        float ReadBitCoordMP(BitCoordType coord_type) {
            bool integral = (coord_type == BitCoordType::CW_INTEGRAL);
            bool low_precision = (coord_type == BitCoordType::CW_LOW_PRECISION);

            int32 int_val;
            int32 fract_val;
            int32 sign_bit = 0;
            float value = 0.0f;

            bool in_bounds = ReadBit() ? true : false;

            if (integral) {
                int_val = ReadBit();

                if (int_val) {
                    sign_bit = ReadBit();

                    if (in_bounds) {
                        value = static_cast<float>(ReadUBitLong(DemoMemoryBitStream::kCoordIntegerBitsMP) + 1);
                    } else {
                        value = static_cast<float>(ReadUBitLong(DemoMemoryBitStream::kCoordIntegerBits) + 1);
                    }
                }
            } else {
                int_val = ReadBit();
                sign_bit = ReadBit();

                if (int_val) {
                    if (in_bounds) {
                        int_val = ReadUBitLong(DemoMemoryBitStream::kCoordIntegerBitsMP) + 1;
                    } else {
                        int_val = ReadUBitLong(DemoMemoryBitStream::kCoordIntegerBits) + 1;
                    }
                }

                fract_val = ReadUBitLong(low_precision ? DemoMemoryBitStream::kCoordFractionalBitsMPLowPrecision : DemoMemoryBitStream::kCoordFractionalBits);

                value = int_val + (static_cast<float>(fract_val) * (low_precision ? DemoMemoryBitStream::kCoordFractionalBitsMPLowPrecision : DemoMemoryBitStream::kCoordResolution));
            }

            if (sign_bit) {
                value = -value;
            }

            return value;
        }

        float ReadBitFloat() {
            uint32 value = ReadUBitLong(32);
            return *(reinterpret_cast<float*>(&value));
        }

        float ReadBitNormal() {
            int32 sign_bit = ReadBit();

            uint32 fract_val = ReadUBitLong(DemoMemoryBitStream::kNormalFractionalBits);

            float value = static_cast<float>(fract_val) * DemoMemoryBitStream::kNormalResolution;

            if (sign_bit) {
                value = -value;
            }

            return value;
        }

        float ReadBitCellCoord(int32 bits, BitCoordType coord_type) {
            bool integral = (coord_type == BitCoordType::CW_INTEGRAL);
            bool low_precision = (coord_type == BitCoordType::CW_LOW_PRECISION);

            int32 int_val;
            int32 fract_val;
            float value;

            if (integral) {
                value = static_cast<float>(ReadUBitLong(bits));
            } else {
                int_val = ReadUBitLong(bits);
                fract_val = ReadUBitLong(low_precision ? DemoMemoryBitStream::kCoordFractionalBitsMPLowPrecision : DemoMemoryBitStream::kCoordFractionalBits);
                value = int_val + (static_cast<float>(fract_val) * (low_precision ? DemoMemoryBitStream::kCoordResolutionLowPrecision : DemoMemoryBitStream::kCoordResolution));
            }

            return value;
        }

    };

}

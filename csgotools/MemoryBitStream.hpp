#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "Steam.hpp"

#define SWAP16(n) (((n&0xFF00)>>8)|((n&0x00FF)<<8))
#define SWAP32(n) ((SWAP16((n&0xFFFF0000)>>16))|((SWAP16(n&0x0000FFFF))<<16))
#define SWAP64(n) ((SWAP32((n&0xFFFFFFFF00000000)>>32))|((SWAP32(n&0x00000000FFFFFFFF))<<32))

namespace csgotools {

    class MemoryBitStream {
    protected:
        char* buffer_ = nullptr;
        uint32 size_bits_{};
        uint32 current_position_{};
    public:

        enum class BitCoordType {
            CW_NONE,
            CW_LOW_PRECISION,
            CW_INTEGRAL
        };

        MemoryBitStream() = default;

        MemoryBitStream(const char* buffer, const uint32& num_bytes) {
            buffer_ = new char[num_bytes];
            std::memcpy(buffer_, buffer, num_bytes);
            size_bits_ = num_bytes * 8;
            current_position_ = 0;
        }

        ~MemoryBitStream() {
            delete buffer_;
        }

        MemoryBitStream(const MemoryBitStream& other) {
            *this = other;
        }

        MemoryBitStream(MemoryBitStream&& other) noexcept {
            if (&other == this) {
                return;
            }

            *this = other;
        }

        MemoryBitStream& operator=(const MemoryBitStream& other) {
            uint32 bytes = other.size_bits_ / 8;

            this->size_bits_ = other.size_bits_;
            this->buffer_ = new char[bytes];
            std::memcpy(this->buffer_, other.buffer_, bytes);
            this->current_position_ = other.current_position_;

            return *this;
        }

        MemoryBitStream& operator=(MemoryBitStream&& other) noexcept {
            if (&other == this) {
                return *this;
            }

            uint32 bytes = other.size_bits_ / 8;

            this->size_bits_ = other.size_bits_;
            this->buffer_ = new char[bytes];
            std::memcpy(this->buffer_, other.buffer_, bytes);
            this->current_position_ = other.current_position_;

            delete[] other.buffer_;
            other.buffer_ = nullptr;
            other.size_bits_ = 0;
            other.current_position_ = 0;
            return *this;
        }

        bool IsEnd() const {
            if (current_position_ >= size_bits_) {
                return true;
            }

            return false;
        }

        void Alloc(const uint32& num_bytes) {
            size_bits_ = num_bytes * 8;
            buffer_ = new char[num_bytes];
            current_position_ = 0;
        }

        char* GetBufferPointer() const {
            return buffer_;
        }

        void Clear() {
            delete[] buffer_;
            current_position_ = 0;
            size_bits_ = 0;
        }

        void SetPosition(uint32 position) { current_position_ = position; }
        uint32 GetCurrentPositionBits() const { return current_position_; }
        uint32 GetCurrentPosition() const { return current_position_ / 8; }
        uint32 GetBitsSize() const { return size_bits_; }
        uint32 GetSize() const { return size_bits_ * 8; }

        static MemoryBitStream CreateFromFile(const std::string& file_name) {
            std::string string_buffer;

            std::ifstream file;
            file.open(file_name.c_str(), std::ios::in | std::ios::binary);

            if (file.is_open()) {
                size_t file_size;
                file.seekg(0, std::ios::end);
                file_size = static_cast<size_t>(file.tellg());
                file.seekg(0, std::ios::beg);

                if (file_size <= 0) {
                    std::cerr << "MemoryBitStream::CreateFromFile: File too small: " << file_size << " bytes" << std::endl;
                    file.close();
                    return MemoryBitStream{};
                }

                string_buffer.resize(file_size);
                file.read(&string_buffer[0], file_size);
                file.close();

                return MemoryBitStream{string_buffer.c_str(), file_size};
            } else {
                std::cerr << "MemoryBitStream::CreateFromFile: Could not open file: " << file_name << std::endl;
            }

            return MemoryBitStream{};
        }

        bool ReadBit() {
            bool result = false;

            if (buffer_ && current_position_ < size_bits_) {
                char current_byte = buffer_[current_position_ / 8];
                result = (current_byte >> (current_position_ % 8)) & 1;
                ++current_position_;
            }

            return result;
        }

        uint32 ReadUBitLong(uint32 num_bits) {
            uint32 result = 0;
            ReadBits(&result, num_bits);
            return result;
        }

        int32 ReadSBitLong(uint32 num_bits) {
            return (ReadUBitLong(num_bits) << (32 - num_bits)) >> (32 - num_bits);
        }

        uint32 ReadUBitVar() {
            uint32 ret = ReadUBitLong(6);
            switch (ret & (16 | 32)) {
                case 16:
                {
                    ret = (ret & 15) | (ReadUBitLong(4) << 4);
                } break;
                case 32:
                {
                    ret = (ret & 15) | (ReadUBitLong(8) << 4);
                } break;
                case 48:
                {
                    ret = (ret & 15) | (ReadUBitLong(32 - 4) << 4);
                } break;
                default: break;
            }
            return ret;
        }

        void ReadBits(void* buffer, size_t size) {
            for (size_t i = 0; i < size; i++) {
                reinterpret_cast<char*>(buffer)[i / 8] |= ReadBit() << (i % 8);
            }
        }

        int8 ReadByte() {
            char result = 0;
            if (current_position_ % 8 == 0) {
                result = buffer_[current_position_ / 8];
                current_position_ += 8;
            } else {
                ReadBits(&result, 8);
            }

            return result;
        }

        void SkipBits(size_t bits) {
            current_position_ += bits;
        }

        void SkipBytes(size_t size) {
            current_position_ += size * 8;
        }

        void ReadBytes(void* buffer, size_t size) {
            for (size_t i = 0; i < size; ++i) {
                static_cast<char*>(buffer)[i] = ReadByte();
            }
        }

        int16 ReadInt16() {
            int16 result;
            ReadBytes(static_cast<void*>(&result), sizeof(int16));
            return result;
        }

        int32 ReadInt32() {
            int32 result;
            ReadBytes(static_cast<void*>(&result), sizeof(int32));
            return result;
        }

        int64 ReadInt64() {
            int64 result = 0;
            ReadBytes(static_cast<void*>(&result), sizeof(int64));
            return result;
        }

        float ReadFloat() {
            float result;
            ReadBytes(static_cast<void*>(&result), sizeof(float));
            return result;
        }

        std::string ReadString(uint32 limit) {
            std::string output;
            uint32 current_pos = 0;

            while (true) {
                char val = ReadByte();
                // if current char is NULL 
                // or we reached our limit we break
                if (val == 0 || current_pos >= limit) {
                    break;
                }

                if (current_pos < limit) {
                    output.push_back(val);
                    ++current_pos;
                }
            }

            return output;
        }

        std::string ReadFixedString(uint32 limit) {
            char* tmp = new char[limit];
            ReadBytes(tmp, limit);
            std::string result{tmp};
            delete[] tmp;
            return result;
        }

        static inline int32 ZigZagDecode32(uint32 n) {
            return(n >> 1) ^ - static_cast<int32>(n & 1);
        }

        static inline int64 ZigZagDecode64(uint64 n) {
            return(n >> 1) ^ - static_cast<int64>(n & 1);
        }
    };

}

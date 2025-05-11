#pragma once

#include <cstdint> 
#include <algorithm> 
#include <type_traits> 

namespace bixit::abstract_chain {

    enum class Endianness {
        BIG,
        LITTLE
    };

    constexpr Endianness getSystemEndianness() {
        union {
            uint32_t i;
            char c[4];
        } bint = {0x01020304};

        return (bint.c[0] == 0x01) ? Endianness::BIG : Endianness::LITTLE;
    }

    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, T>::type
    changeEndianness(T value) {
        static_assert(sizeof(T) <= 8, "Type size exceeds 8 bytes.  Unsupported.");

        T result = value;
        unsigned char* bytes = reinterpret_cast<unsigned char*>(&result);
        std::reverse(bytes, bytes + sizeof(T));
        return result;
    }

    uint64_t convert_from_uint8_array_to_uint64(
        const uint8_t* buffer,
        size_t bitLength,
        Endianness endianness,
        uint64_t* result) 
    {
        
        memset(result, 0, sizeof(uint64_t));

        if (buffer == nullptr || result == nullptr) { return 1; } // Invalid pointers
        if (bitLength > 64) { return 2; }  // Too many bits
        if (bitLength == 0) { return 0; }
        
        size_t numBytes = (bitLength + 7) / 8; 

        if (endianness == Endianness::BIG) {
            //Invert the bytes' order
            uint8_t inv_buffer[sizeof(uint64_t)] = {0};
            for (size_t i = 0; i < numBytes; ++i) {
                inv_buffer[sizeof(uint64_t)-1-i] = buffer[i];
            }            
            memcpy(result, inv_buffer, sizeof(uint64_t));
            *result = *result >> (64 - bitLength);
        } else if (endianness == Endianness::LITTLE) {
            memcpy(result, buffer, numBytes);
        } else {
            return 4; // invalid endianness
        }
        return 0;
    }


    int convert_from_uint64_to_uint8_array(
        const uint64_t value,
        const size_t bitLength,
        const Endianness endianness,
        uint8_t* buffer)
    {

        if (buffer == nullptr) { return 1; } // Invalid pointer
        if (bitLength > 64) { return 2; }   // Too many bits
        if (bitLength == 0) { return 0; }   // Nothing to do
        
        size_t numBytes = (bitLength + 7) / 8;        
        memset(buffer, 0, numBytes);

        uint64_t workValue = value;
        if (endianness == Endianness::BIG) {
            // Per big endian, mantenendo i bit meno significativi a destra
            for (size_t i = 0; i < numBytes; ++i) {
                buffer[i] = (workValue >> (8 * (numBytes - 1 - i))) & 0xFF;
            }
            
            // Se bitLength non è multiplo di 8, dobbiamo spostare i bit all'interno dell'ultimo byte
            if (bitLength % 8 != 0) {
                // Shift everything right within the last byte
                for (size_t i = 0; i < numBytes; i++) {
                    buffer[i] >>= (8 - (bitLength % 8));
                }
            }
        } else if (endianness == Endianness::LITTLE) {
            // Per little endian, vogliamo che i bit meno significativi siano all'inizio
            for (size_t i = 0; i < numBytes; ++i) {
                buffer[i] = (workValue >> (8 * i)) & 0xFF;
            }
        /*
        if (endianness == Endianness::BIG) {
            // For big endian, we need to shift the value to the most significant bits first
            // if bitLength is not a multiple of 8
            if (bitLength % 8 != 0) {
                workValue <<= (8 - (bitLength % 8));
            }
            
            // Fill buffer in big endian order (most significant byte first)
            for (size_t i = 0; i < numBytes; ++i) {
                buffer[i] = (workValue >> (8 * (numBytes - 1 - i))) & 0xFF;
            }
        } else if (endianness == Endianness::LITTLE) {
            // For little endian, we copy the bytes directly
            for (size_t i = 0; i < numBytes; ++i) {
                buffer[i] = (workValue >> (8 * i)) & 0xFF;
            }
            
            // If bitLength is not a multiple of 8, mask off unused bits in the last byte
            if (bitLength % 8 != 0) {
                buffer[numBytes - 1] &= (1 << (bitLength % 8)) - 1;
            }
            */
        } else {
            return 4; // Invalid endianness
        }
        
        return 0;
    }
}
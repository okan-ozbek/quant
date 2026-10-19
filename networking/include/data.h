//
// Created by Dorza on 18/10/2026.
//

#ifndef QUANT_DATA_H
#define QUANT_DATA_H

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

using Bytes = std::vector<std::byte>;

struct Data
{
    std::uint32_t textLength;
    std::string text;
};

inline Bytes Encode(const Data& data)
{
    Bytes bytes(sizeof(data.textLength) + data.text.size());

    // Encode text length
    for (std::size_t i = 0; i < sizeof(data.textLength); ++i) {
        bytes[i] = static_cast<std::byte>((data.textLength >> (i * 8)) & 0xFF);
    }

    // Encode text
    std::memcpy(bytes.data() + sizeof(data.textLength), data.text.data(), data.text.size());

    return bytes;
}

inline Data Decode(const Bytes& bytes)
{
    Data data{};

    // Decode text length
    for (std::size_t i = 0; i < sizeof(data.textLength); ++i) {
        data.textLength |= static_cast<std::uint32_t>(bytes[i]) << (i * 8);
    }

    // Decode text
    data.text.assign(reinterpret_cast<const char*>(bytes.data() + sizeof(data.textLength)), data.textLength);

    return data;
}

#endif //QUANT_DATA_H

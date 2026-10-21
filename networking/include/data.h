//
// Created by Dorza on 18/10/2026.
//

#ifndef QUANT_DATA_H
#define QUANT_DATA_H

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <cstddef>
#include <stdexcept>

#include "base.h"

using Bytes = std::vector<std::byte>;

struct Data
{
    std::uint32_t textLength;
    std::string text;
};

inline Bytes Encode(const std::string& data)
{
    if (data.size() > MAX_PAYLOAD_SIZE) {
        throw std::runtime_error("Payload size exceeds maximum limit");
    }

    const Data dataPacket{ static_cast<std::uint32_t>(data.size()), data };

    Bytes bytes(sizeof(dataPacket.textLength) + dataPacket.text.size());

    // Encode text length
    for (std::size_t i = 0; i < sizeof(dataPacket.textLength); ++i) {
        bytes[i] = static_cast<std::byte>((dataPacket.textLength >> (i * 8)) & 0xFF);
    }

    if (dataPacket.textLength != dataPacket.text.size()) {
        throw std::runtime_error("Text length does not match actual text size");
    }

    // Encode text
    std::memcpy(bytes.data() + sizeof(dataPacket.textLength), dataPacket.text.data(), dataPacket.text.size());

    return bytes;
}

inline Data Decode(const Bytes& bytes)
{
    Data data{};

    if (bytes.size() < sizeof(data.textLength)) {
        throw std::runtime_error("Insufficient bytes for decoding text length");
    }

    // Decode text length
    for (std::size_t i = 0; i < sizeof(data.textLength); ++i) {
        data.textLength |= static_cast<std::uint32_t>(bytes[i]) << (i * 8);
    }

    if (data.textLength > MAX_PAYLOAD_SIZE) {
        throw std::runtime_error("Payload size exceeds maximum limit");
    }

    if (bytes.size() < sizeof(data.textLength) + data.textLength) {
        throw std::runtime_error("Insufficient bytes for decoding text content");
    }

    // Decode text
    data.text.assign(reinterpret_cast<const char*>(bytes.data() + sizeof(data.textLength)), data.textLength);

    return data;
}

#endif //QUANT_DATA_H

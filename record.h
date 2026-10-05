#pragma once
#include <string>
#include <cstdint>
#include "checksum.h"
#include <cstring>
// Turns a key and value into one byte string ready to write to the WAL.
// Layout: [key length][value length][checksum][key][value]
std::string encodeRecord(const std::string& key, const std::string& value, uint8_t type){
    uint32_t keylen = key.size();
    uint32_t vallen = value.size();

    // Checksum covers the key + value bytes, so we can detect
    // if either got corrupted or torn during a crash.
    uint32_t checksum = crc32(key + value);

    std::string record;
    record.append(reinterpret_cast<const char*>(&type), sizeof(type));
    record.append(reinterpret_cast<const char*>(&keylen), sizeof(keylen));
    record.append(reinterpret_cast<const char*>(&vallen), sizeof(vallen));
    record.append(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
    record.append(key);
    record.append(value);
    return record;
}

struct DecodedRecord {
    uint8_t type;
    std::string key;
    std::string value;
    bool valid;
    size_t bytesConsumed;
};

// Reverse of encodeRecord: reads one record from `data` starting at `offset`.
// Returns the key, value, whether the record is valid (checksum matched and
// wasn't torn), and how many bytes it consumed (so the caller knows where the
// next record starts). Bounds checks guard against records cut short by a crash.
//return the result with all the data
DecodedRecord decodeRecord(const std::string& data, size_t offset) {
    DecodedRecord result;

    if (offset + 13 > data.size()) {
        result.valid = false;
        return result;
    }
    uint8_t type;
    memcpy(&type, data.data() + offset, 1);
    uint32_t keylen;
    memcpy (&keylen, data.data() + offset + 1, 4);
    uint32_t vallen;
    memcpy (&vallen , data.data() + offset + 5, 4);
    uint32_t checksum;
    memcpy (&checksum, data.data() + offset + 9, 4);
    if (offset + 13 + keylen + vallen > data.size()){
        result.valid = false;
        return result;
    }
    std::string key = data.substr(offset + 13, keylen);
    std::string value = data.substr(offset+ 13 + keylen, vallen);
    result.valid = (crc32(key +value)== checksum);
    result.key = key;
    result.value = value;
    result.bytesConsumed = 13 + keylen + vallen;
    result.type = type;
    return result; 
}

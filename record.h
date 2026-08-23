#include <string>
#include <cstdint>
#include "checksum.h"
// Turns a key and value into one byte string ready to write to the WAL.
// Layout: [key length][value length][key][value]
std::string encodeRecord(const std::string& key, const std::string& value){
    uint32_t keylen = key.size();
    uint32_t vallen = value.size();

    // Checksum covers the key + value bytes, so we can detect
    // if either got corrupted or torn during a crash.
    uint32_t checksum = crc32(key + value);

    std::string record;
    record.append(reinterpret_cast<const char*>(&keylen), sizeof(keylen));
    record.append(reinterpret_cast<const char*>(&vallen), sizeof(vallen));
    record.append(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
    record.append(key);
    record.append(value);
    return record;
}
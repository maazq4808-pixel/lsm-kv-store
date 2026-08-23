#include <cstdint>
#include <string>

// Computes a CRC32 checksum over the given bytes.

uint32_t crc32(const std::string& data) {
    uint32_t crc = 0xFFFFFFFF;

    for (unsigned char byte : data) {
        crc ^= byte;
        for (int i = 0; i < 8; i++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc = crc >> 1;
            }
        }
    }

    return ~crc;
}
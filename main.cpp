#include "record.h"
#include <iostream>

int main() {
    std::string encoded = encodeRecord("ali", "5555-1234");
    DecodedRecord r = decodeRecord(encoded, 0);

    std::cout << "key: " << r.key << "\n";
    std::cout << "value: " << r.value << "\n";
    std::cout << "valid: " << r.valid << "\n";
    std::cout << "bytesConsumed: " << r.bytesConsumed << "\n";
}
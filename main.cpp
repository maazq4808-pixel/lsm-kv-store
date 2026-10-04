#include "sstable.h"
#include <map>
#include <string>
#include <iostream>

int main() {
    // make a small sorted map (stands in for a memtable)
    std::map<std::string, std::string> data;
    data["ali"] = "5555-1234";
    data["bob"] = "5555-0000";
    data["sara"] = "5555-6789";

    // write it to an SSTable file
    writeSSTable("test.sst", data);

    std::cout << "SSTable written to test.sst\n";
}
#include "sstable.h"
#include <map>
#include <string>
#include <iostream>

int main() {
    // build a small sorted map and write it to an SSTable
    std::map<std::string, std::string> data;
    data["ali"] = "5555-1234";
    data["bob"] = "5555-0000";
    data["sara"] = "5555-6789";
    writeSSTable("test.sst", data);

    // helper to print a lookup result
    auto show = [](const std::string& key, std::optional<std::string> r) {
        if (r.has_value())
            std::cout << key << " = " << r.value() << "\n";
        else
            std::cout << key << " = NOT FOUND\n";
    };

    // read keys back from the SSTable
    show("ali",  readFromSstable("test.sst", "ali"));
    show("bob",  readFromSstable("test.sst", "bob"));
    show("sara", readFromSstable("test.sst", "sara"));
    show("zzz",  readFromSstable("test.sst", "zzz"));   // not in the file
}
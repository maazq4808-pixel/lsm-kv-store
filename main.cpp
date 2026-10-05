#include "db.h"
#include <iostream>

int main() {
    DB db;
    db.open("wal.log");
    db.put("ali", "1");
    db.put("bob", "2");
    db.put("cat", "3");   // flushes ali,bob,cat to sstable0
    db.put("dog", "4");   // dog stays in memtable

    auto show = [](const std::string& k, std::optional<std::string> r){
        std::cout << k << " = " << (r.has_value() ? r.value() : "NOT FOUND") << "\n";
    };
    show("ali", db.get("ali"));   // in SSTable → should be found now
    show("dog", db.get("dog"));   // in memtable → found
    show("zzz", db.get("zzz"));   // nowhere → not found
    db.close();
}
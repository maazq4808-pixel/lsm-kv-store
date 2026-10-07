#include "db.h"
#include <iostream>

int main() {
    DB db;
    db.open("wal.log");

    auto show = [](const std::string& k, std::optional<std::string> r){
        std::cout << k << " = " << (r.has_value() ? r.value() : "NOT FOUND") << "\n";
    };

    // put 3 keys → flushes to sstable0 (ali, bob, cat now on disk)
    db.put("ali", "1");
    db.put("bob", "2");
    db.put("cat", "3");

    // now delete ali — it's already in sstable0
    db.del("ali");
    db.put("bob", "2");
    db.put("cat", "3");


    // the real test: is ali still deleted, even though its value is in sstable0?
    show("ali", db.get("ali"));  // should be NOT FOUND (tombstone shadows sstable0's value)
    show("bob", db.get("bob"));   // should be found (2)

    db.close();
}
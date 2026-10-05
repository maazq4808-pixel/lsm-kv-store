#include "sstable.h"
#include <map>
#include <string>
#include <iostream>
#include "db.h"
#include <iostream>

int main() {
    DB db;
    db.open("wal.log");

    db.put("ali", "1");
    db.put("bob", "2");
    db.put("cat", "3");    // memtable hits 3 → should flush to sstable0.sst
    db.put("dog", "4");

    std::cout << "done\n";
    db.close();
}
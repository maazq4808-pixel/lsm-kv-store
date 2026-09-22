#include "db.h"
#include <iostream>

int main() {
    DB db;
    db.open("wal.log");     // recovery runs here — should rebuild memtable from disk
    std::cout << db.get("ali") << "\n";
    std::cout << db.get("sara") << "\n";
    db.close();
}
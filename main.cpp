#include "db.h"
#include <iostream>

int main() {
    DB db;
    db.open("wal.log");
    db.put("ali", "5555-1234");
    db.put("sara", "5555-6789");
    db.close();
    std::cout << "wrote data\n";
}
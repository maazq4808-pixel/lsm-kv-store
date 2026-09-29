#include "db.h"
#include <iostream>
#include "record.h"

int main() {
    DB db;
    db.open("wal.log");

    db.put("ali", "5555-1234");
    db.put("sara", "5555-6789");
    db.del("ali");                    // delete ali

    std::cout << "ali: [" << db.get("ali") << "]\n";   // should be empty (deleted)
    std::cout << "sara: [" << db.get("sara") << "]\n";  // should still be there

    db.close();
}
#include "db.h"
#include <iostream>
// Quick manual test for LogFile: write two records,
// sync them to disk, then check wal.log with `cat` to confirm.
int main(){
    DB db;
    db.open("wal.log");
    db.put("ali","5555-1234");
    db.put("sara","5555-6789");
    std::cout << db.get("sara") << "\n";
    std::cout << db.get("ali") << "\n";
    db.close();
}
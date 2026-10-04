#include "db.h"
#include <iostream>

int main() {
    DB db;
    db.open("crash.wal");      // recovery runs here on the crash-severed WAL

    // check some early keys that definitely got written before the crash
    for (int i = 0; i < 20; i++) {
        auto r = db.get("key" + std::to_string(i));
        if (r.has_value()) {
            std::cout << "key" << i << " = " << r.value() << "\n";
        } else {
            std::cout << "key" << i << " = NOT FOUND\n";
        }
    }

    db.close();
    std::cout << "recovery completed without crashing\n";
}
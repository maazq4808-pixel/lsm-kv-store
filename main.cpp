#include "db.h"
#include <iostream>

int main() {
    DB db;
    db.open("wal.log");

    db.put("ali", "5555-1234");
    db.put("sara", "5555-6789");
    db.del("ali");                       // delete ali

    // helper lambda to print an optional result
    auto show = [](const std::string& name, std::optional<std::string> r) {
        if (r.has_value()) {
            std::cout << name << ": found [" << r.value() << "]\n";
        } else {
            std::cout << name << ": not found\n";
        }
    };

    show("ali", db.get("ali"));          // deleted → not found
    show("sara", db.get("sara"));        // → found [5555-6789]
    show("bob", db.get("bob"));          // never stored → not found

    db.close();
}
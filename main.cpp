#include "db.h"
#include <iostream>
#include <chrono>

int main() {
    const int N = 10000;

    // ---------- Mode 1: sync every write (safe) ----------
    {
        system("rm -f bench1.wal");     // fresh file
        DB db;
        db.setSyncInterval(1);           // sync every write
        db.open("bench1.wal");

        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < N; i++) {
            db.put("key" + std::to_string(i), "value");
        }
        auto end = std::chrono::steady_clock::now();
        db.close();

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "sync every write (N=1):   " << ms << " ms\n";
    }

    // ---------- Mode 2: batched (sync every 100) ----------
    {
        system("rm -f bench2.wal");
        DB db;
        db.setSyncInterval(100);         // sync every 100 writes
        db.open("bench2.wal");

        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < N; i++) {
            db.put("key" + std::to_string(i), "value");
        }
        auto end = std::chrono::steady_clock::now();
        db.close();

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        std::cout << "batched (N=100):          " << ms << " ms\n";
    }
}
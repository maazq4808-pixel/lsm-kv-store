#include "log_file.h"
#include "memtable.h"
#include "record.h"
#include <string>

// Ties WAL + Memtable together. This is the actual class a user
// of the library interacts with directly.
//
// Durability comes from writing to the WAL before updating the
// memtable — if we crash between those two steps, the WAL still
// has the record and it can be recovered on next open().
class DB {
public:
    // Opens the database at `path`. Currently just opens the WAL file.
    // Will later also replay the WAL to rebuild memtable_ on startup,
    // once recovery is implemented.
    void open(const std::string& path) {
        wal_.open(path);
    }

    // Stores a key-value pair. Order matters here:
    // 1. Encode + write to WAL, then sync to physical disk (durable)
    // 2. Only THEN update the memtable (fast lookups)
    // If we crash between steps 1 and 2, recovery can still rebuild
    // this write from the WAL — nothing is lost.
    void put(const std::string& key, const std::string& value) {
        std::string encoded = encodeRecord(key, value);
        wal_.append(encoded);
        wal_.sync();

        memtable_.put(key, value);
    }

    // Reads only check the memtable — fast, no disk access needed.
    // NOTE: currently returns "" for a missing key, same limitation
    // as Memtable::get(). Needs fixing before delete() is added,
    // since "" can't be distinguished from "key not found."
    std::string get(const std::string& key) {
        return memtable_.get(key);
    }

    // Closes the WAL file. Does NOT lose any data — all writes
    // were already sync()'d to disk individually inside put().
    void close() {
        wal_.close();
    }

private:
    LogFile wal_;       // durable, on-disk copy of every write
    Memtable memtable_;  // fast, in-memory copy for reads
};
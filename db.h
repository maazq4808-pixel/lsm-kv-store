#pragma once
#include "log_file.h"
#include "memtable.h"
#include "record.h"
#include <string>
#include <optional>

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
        recover(path);
    }

    // Stores a key-value pair. Order matters here:
    // 1. Encode + write to WAL, then sync to physical disk (durable)
    // 2. Only THEN update the memtable (fast lookups)
    // If we crash between steps 1 and 2, recovery can still rebuild
    // this write from the WAL — nothing is lost.
    void put(const std::string& key, const std::string& value) {
        std::string encoded = encodeRecord(key, value, 0);
        wal_.append(encoded);
        writesSinceSync_ +=1;
        if (writesSinceSync_ >= N_){
            wal_.sync();
            writesSinceSync_ = 0;
        }

        memtable_.put(key, value);
        // If the memtable has grown past the flush threshold, write it out to a
// new SSTable file on disk and clear it. This keeps the memtable (RAM)
// bounded — older data lives in SSTables, only recent writes stay in memory.
        if (memtable_.size()>= flushThreshold_){
            std::string filename = "sstable" + std::to_string(sstCounter_) + ".sst";
            writeSSTable(filename, memtable_.getData());
            memtable_.clear();
            sstCounter_ += 1;
        }

    }
    // Deletes a key. Since the WAL is append-only, we can't erase the old
    // record — instead we append a tombstone (type 1, empty value) that
    // shadows it. On read, the tombstone means the key is gone. Also removes
    // the key from the memtable so lookups don't find it.
    void del(const std::string& key){
        std::string encoded =encodeRecord(key,"", 1);
        wal_.append(encoded);
        writesSinceSync_ += 1;
        if (writesSinceSync_ >= N_){
            wal_.sync();
            writesSinceSync_ = 0;
        }

        memtable_.del(key);
    }


    // Reads only check the memtable — fast, no disk access needed.
    // NOTE: currently returns "" for a missing key, same limitation
    // as Memtable::get(). Needs fixing before delete() is added,
    // since "" can't be distinguished from "key not found."
    std::optional<std::string> get(const std::string& key) {
        return memtable_.get(key);
    }

    // Flushes any pending (un-synced) writes to disk, then closes the WAL.
    // A clean close never loses data; only a crash mid-batch can.
    void close() {
        wal_.sync();
        writesSinceSync_ = 0;
        wal_.close();
    }
    // Replays the WAL into the memtable on startup: puts re-add keys,
    // tombstones re-delete them. Stops at the first invalid/torn record
    // (the crash point), discarding anything after it.
    void recover(const std::string& path) {
        std::string data = wal_.readAll(path);
        size_t offset = 0;
        while(offset < data.size()){
                DecodedRecord r = decodeRecord(data, offset);
            if (r.valid == false){
                break;
            }
            if (r.type == 1){
                memtable_.del(r.key);
            }
            else{
                memtable_.put(r.key, r.value);
            }
            
            
            offset += r.bytesConsumed;
            }
        }  
    void setSyncInterval(size_t n){
        N_ = n;
            }
    

private:
    LogFile wal_;       // durable, on-disk copy of every write
    Memtable memtable_;  // fast, in-memory copy for reads
    size_t N_ = 1;
    size_t writesSinceSync_ = 0; //sync threshold
    size_t sstCounter_ = 0;  //counter to count the number of sst files created
    size_t flushThreshold_ = 3; //threshold to flush sstable file

};


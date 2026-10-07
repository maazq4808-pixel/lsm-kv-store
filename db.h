#pragma once
#include "log_file.h"
#include "memtable.h"
#include "record.h"
#include <string>
#include <optional>
#include "sstable.h"

// Ties WAL + Memtable together. This is the actual class a user
// of the library interacts with directly.
//
// Durability comes from writing to the WAL before updating the
// memtable — if we crash between those two steps, the WAL still
// has the record and it can be recovered on next open().
class DB {
public:
    // Opens the database: opens the WAL, then replays it to rebuild
// the memtable (see recover()).
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
    // Deletes a key. The WAL is append-only, so we cannot erase old
    // records. We append a tombstone (type 1, empty value) to the WAL
    // and store a tombstone in the memtable. On flush, the tombstone
    // goes to the SSTable, where it hides older values of the key.
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


    // Looks up a key, newest data first: memtable, then SSTables from
    // newest to oldest. The first record found for the key is the answer.
    // A tombstone means deleted: return nullopt and stop searching.
    std::optional<std::string> get(const std::string& key) {
        std::optional<Entry> entry = memtable_.getEntry(key);
        if (entry.has_value()){
            if (entry.value().isTombstone == true){
                return std::nullopt;
            }
            return entry.value().value;
        }
        for (int i = sstCounter_  - 1; i >= 0; i--){
            std::string filename = "sstable" + std::to_string(i) + ".sst";
            auto found = readFromSstable (filename, key);
            if (found.has_value()){
                if (found.value().isTombstone == true ){
                    return std::nullopt;

                }
            
                return found.value().value;
            }
        }
          return std::nullopt;  
         

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


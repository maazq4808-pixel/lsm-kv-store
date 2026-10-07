#pragma once
#include <string>
#include <map>
#include "log_file.h"
#include "record.h"
#include <optional>
#include "memtable.h"
// Writes a memtable's contents to an SSTable file on disk.
//
// An SSTable is a sorted, immutable file of key-value entries. Because the
// memtable (std::map) is already sorted by key, we just iterate it in order
// and write each entry — the file comes out sorted for free.
//
// Each entry reuses the WAL record format via encodeRecord:
//   [type:1][keyLen:4][valLen:4][checksum:4][key][value]
// type is 0 (value) for all entries here, since the memtable only holds
// live values (deletes erase from the map).
//
// Each entry becomes a record: type 1 for tombstones (deleted keys),
// type 0 for values. Reuses encodeRecord.
void writeSSTable(const std::string& path,
                  const std::map<std::string, Entry>& data) {
    LogFile out;
    out.open(path);

    // Walk the map in sorted order, encode each entry, write it to disk.
    for (const auto& entry : data) {
        uint8_t type = 0;
        if (entry.second.isTombstone == true){
          type = 1;
        }
        std::string encoded = encodeRecord(entry.first, entry.second.value, type);
        out.append(encoded);
    }

    out.sync();   // force the whole SSTable to disk — it's permanent storage
    out.close();
}

// Searches an SSTable file for a single key and returns its value.
//
// Reads the whole file, then scans entries in order (decodeRecord) until it
// finds a matching key. If the match is a tombstone (type 1), the key was
// deleted, so returns nullopt. If no entry matches, returns nullopt too.
// Stops early at the first corrupt/torn record.
//
// This is a linear scan for now; a sparse index will make it a binary search
// later so we don't read the whole file.

std :: optional<std::string> readFromSstable(const std::string& path,const std::string& key){
    LogFile in;
    std :: string data = in.readAll(path);
    size_t offset = 0;

    while ( offset < data.size()){
        DecodedRecord r = decodeRecord(data, offset);
        if (r.valid == false){
            break;

        }
        if (r.key == key){
         if (r.type == 1){
            return std::nullopt;
        }
        else {
            return r.value;
        }
    }
        offset += r.bytesConsumed;

}
    return std::nullopt;
}
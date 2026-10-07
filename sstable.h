#pragma once
#include <string>
#include <map>
#include "log_file.h"
#include "record.h"
#include <optional>
#include "memtable.h"
// Writes a memtable's contents to a new SSTable file on disk.
//
// An SSTable is a sorted, immutable file of records. The memtable
// (std::map) is already sorted by key, so we write entries in map order
// and the file is sorted.
//
// Each entry uses the WAL record format (encodeRecord):
//   [type:1][keyLen:4][valLen:4][checksum:4][key][value]
// type 0 = value, type 1 = tombstone (deleted key, empty value).
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

// Looks up one key in an SSTable file.
//
// Returns:
//   Entry{value, false}  -> key found with a value
//   Entry{"", true}      -> key found as a tombstone (deleted)
//   std::nullopt         -> key is not in this file
//
// The caller must stop searching on a tombstone. A tombstone is an
// answer ("deleted"), not "not found".
//
// Linear scan for now. A sparse index will replace it later.
// Stops at the first invalid record.
std :: optional<Entry> readFromSstable(const std::string& path,const std::string& key){
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
            return Entry {"",true};
        }
        else {
            return Entry{r.value,false};
        }
    }
        offset += r.bytesConsumed;

}
    return std::nullopt;
}
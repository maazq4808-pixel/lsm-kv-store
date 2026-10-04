#pragma once
#include <string>
#include <map>
#include "log_file.h"
#include "record.h"

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
// This writes just the data block for now; the sparse index and footer
// come later.
void writeSSTable(const std::string& path,
                  const std::map<std::string, std::string>& data) {
    LogFile out;
    out.open(path);

    // Walk the map in sorted order, encode each entry, write it to disk.
    for (const auto& entry : data) {
        std::string encoded = encodeRecord(entry.first, entry.second, 0);
        out.append(encoded);
    }

    out.sync();   // force the whole SSTable to disk — it's permanent storage
    out.close();
}
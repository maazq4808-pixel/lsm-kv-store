#pragma once
#include "log_file.h"
#include "record.h"
#include <vector>

// The manifest is the list of live SSTable files. It is an append-only
// log on disk, like the WAL. On open(), the DB reads it to find every
// SSTable and the next file number.
//
// Each entry uses the record format (encodeRecord):
//   key   = SSTable file name
//   value = "" (empty)
//   type  = 0: file added, 1: file removed (Phase 3, compaction)
// The CRC32 in each record detects a torn last entry after a crash.
//
// Rule: add a file to the manifest only AFTER the SSTable is fully
// written and synced. A file is part of the database only when the
// manifest lists it.
class Manifest {
    public:
        // Opens the manifest file. Creates it if it does not exist.
        // Uses O_APPEND (LogFile::open), so old entries are kept.
        void open(const std::string& path){
            manifest_.open(path);
            path_ = path;
        }
        void close(){
        manifest_.close();

        }
        // Appends one "file added" entry and syncs it to disk.
        // After this returns, the SSTable is live and survives a restart.
        void add(const std::string& filename){
            std::string encoded = encodeRecord(filename, "", 0);
            manifest_.append(encoded);
            manifest_.sync();
        }

        std::vector<std::string> load(){
            std::string data = manifest_.readAll(path_);
            std::vector<std::string> names;
            
            size_t offset = 0;
            while(offset < data.size()){
                DecodedRecord r = decodeRecord(data, offset);
                if (r.valid == false){
                    break;
                }
                if (r.type == 0){
                    names.push_back(r.key);
                
                }
                
                offset += r.bytesConsumed;
            
          

        }
        return names;
    }




    private:
    LogFile manifest_;   // the manifest file on disk
    std::string path_;
};


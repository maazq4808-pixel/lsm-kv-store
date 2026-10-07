#pragma once
#include <string>
#include <map>
#include <optional>

// In-memory sorted store for recent writes. Backed by a std::map, which keeps
// keys sorted automatically — this is what lets us flush to a sorted SSTable
// cheaply and do range scans later. Lives in RAM, so it's lost on a crash;
// the WAL is what makes writes durable.
struct Entry{
    std::string value;
    bool isTombstone;
};
class Memtable {
    public :
        void put(const std::string& key, const std::string& value){
        data_[key] = Entry{value, false};
    }
        std::optional<std::string> get(const std::string& key){
            auto it = data_.find(key);
            if(it == data_.end()){
                return std::nullopt;
            }
            if (it->second.isTombstone){
                return std::nullopt;
            }
            return it->second.value;
    }
        void del(const std::string& key){
            data_[key]= Entry{"", true};

        }
        size_t size(){
            return data_.size();
        }
        const std::map<std::string, Entry>& getData(){
            return data_;
        }
        void clear(){
            data_.clear();
        }
        std::optional<Entry> getEntry(const std::string& key){
            auto it = data_.find(key);
            if (it == data_.end()){
                return std::nullopt;
            }
            return it->second;
        }
    private :
        std::map<std::string, Entry> data_;
};
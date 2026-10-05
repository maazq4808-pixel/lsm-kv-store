#pragma once
#include <string>
#include <map>
#include <optional>

// In-memory sorted store for recent writes. Backed by a std::map, which keeps
// keys sorted automatically — this is what lets us flush to a sorted SSTable
// cheaply and do range scans later. Lives in RAM, so it's lost on a crash;
// the WAL is what makes writes durable.

class Memtable {
    public :
        void put(const std::string& key, const std::string& value){
        data_[key] = value;
    }
        std::optional<std::string> get(const std::string& key){
            auto it = data_.find(key);
            if(it == data_.end()){
                return std::nullopt;
            }
            return it->second;
    }
        void del(const std::string& key){
            data_.erase(key);

        }
        size_t size(){
            return data_.size();
        }
        const std::map<std::string, std::string>& getData(){
            return data_;
        }
        void clear(){
            data_.clear();
        }
    private :
        std::map<std::string, std::string> data_;
};
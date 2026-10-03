#include <string>
#include <map>
#include <optional>

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
    private :
        std::map<std::string, std::string> data_;
};
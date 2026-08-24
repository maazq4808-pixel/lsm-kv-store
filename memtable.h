#include <string>
#include <map>

class Memtable {
    public :
        void put(const std::string& key, const std::string& value){
        data_[key] = value;
    }
        std::string get(const std::string& key){
        return data_[key];
    }
    private :
        std::map<std::string, std::string> data_;
};
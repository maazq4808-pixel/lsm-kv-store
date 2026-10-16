#include "db.h"
#include <cassert>
#include <filesystem>
#include <iostream>


namespace fs = std::filesystem;
void cleanup() {


fs::remove("wal.log");
fs::remove("MANIFEST");
 for (const auto& entry : fs::directory_iterator(".")){
 if (entry.path().extension()==".sst"){
    fs::remove(entry.path());
 }
 }
}

void testRestart() {
    cleanup();

    {
        DB db;
        db.open("wal.log");
        db.put("k1", "v1");
        db.put("k2", "v2");
        db.put("k3", "v3");
        db.put("k4", "v4");
        db.put("k5", "v5");
        db.put("k6", "v6");
        db.close();
    }
    fs::remove("wal.log");
    {
        DB db2;
        db2.open("wal.log");
        assert(db2.get("k1") == "v1");
        assert(db2.get("k2") == "v2");
        assert(db2.get("k3") == "v3");
        assert(db2.get("k4") == "v4");
        assert(db2.get("k5") == "v5");
        assert(db2.get("k6") == "v6");
        assert(!db2.get("zzz").has_value());
        db2.close();
    }

    std::cout << "testRestart passed\n";
}
void testFlushAfterRestart(){
cleanup();
{
 DB db;
        db.open("wal.log");
        db.put("k1", "v1");
        db.put("k2", "v2");
        db.put("k3", "v3");
        db.put("k4", "v4");
        db.put("k5", "v5");
        db.put("k6", "v6");
        db.close();

}
fs::remove("wal.log");
{
DB db2;
db2.open("wal.log");
 db2.put("k7", "v7");
 db2.put("k8", "v8");
 db2.put("k9", "v9");
 db2.close();
 assert(fs::exists("sstable2.sst"));
}
fs::remove("wal.log");
{
DB db3;
db3.open("wal.log");
        assert(db3.get("k1") == "v1");
        assert(db3.get("k2") == "v2");
        assert(db3.get("k3") == "v3");
        assert(db3.get("k4") == "v4");
        assert(db3.get("k5") == "v5");
        assert(db3.get("k6") == "v6");
        assert(db3.get("k7") == "v7");
        assert(db3.get("k8") == "v8");
        assert(db3.get("k9") == "v9");
        db3.close();
        }


}
int main() {
testRestart();
testFlushAfterRestart();
std::cout << "All tests passed\n";
}

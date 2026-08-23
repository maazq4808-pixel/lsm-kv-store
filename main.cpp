#include "log_file.h"
#include <iostream>
#include "record.h"
// Quick manual test for LogFile: write two records,
// sync them to disk, then check wal.log with `cat` to confirm.
int main(){
    std::string encoded = encodeRecord("ali", "555-1234");
    std::cout <<"total record size = "<<encoded.size()<<"bytes\n";
    LogFile wal;
    wal.open("wal.log");
    wal.append(encoded);
    wal.sync();
    wal.close();


    std::cout<<"done\n";
}
#include "log_file.h"
#include <iostream>
// Quick manual test for LogFile: write two records,
// sync them to disk, then check wal.log with `cat` to confirm.
int main(){
    LogFile wal;
    wal.open("wal.log");
    wal.append("first number\n");
    wal.append("second number\n");
    wal.sync();
    wal.close();


    std::cout<<"done\n";
}
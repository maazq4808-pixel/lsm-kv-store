#include <iostream>
#include <chrono>
#include <string>
#include <fcntl.h>
#include <unistd.h>

int main(){
    const int N = 1000;
    std :: string record = "key12345 value67890 some padding here\n";
    int fd = open("test_nonsync.log", O_WRONLY | O_CREAT |O_TRUNC, 0644);
    auto start = std :: chrono :: steady_clock :: now();
    for (int i = 0; i<N; i++){
        write (fd, record.data(), record.size());
}
auto end = std::chrono::steady_clock::now();
close(fd);

auto ms1 = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
std::cout << "write only:      " << ms1 << "ms\n";


fd = open("test_sync.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);

start = std :: chrono:: steady_clock::now();
for(int i = 0; i<N; i++){
    write(fd,record.data(), record.size());
    fsync(fd);
}
end = std::chrono::steady_clock::now();


close(fd);

auto ms2 = std :: chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
std::cout << "write +fsync:    " << ms2 << "ms\n";

return 0;
}
#include <fcntl.h>
#include <string>
#include <unistd.h>
// Thin wrapper around raw OS file operations (open/write/fsync/close).
// Keeps the file descriptor private so nothing outside this class
// touches raw OS calls directly.
class LogFile {
    public :
// Opens or creates the file at `path`.
// O_APPEND: every write lands at the end, existing data is never
// overwritten — required since this is a log, not a normal file.
// O_CREAT: create the file if it doesn't exist yet.

        void open( const std::string& path){
            fd_  = ::open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
        }
// Writes `data` to the file. Goes into OS memory first —
    // NOT guaranteed to be on disk yet. Call sync() for that.
        void append( const std::string& data){
            write(fd_, data.data(),data.size());
        }
// Forces everything written so far onto physical disk.
// This is what makes writes crash-safe — without this,
// data can be lost if the power cuts.
        void sync(){
            fsync(fd_);
        }
// Closes the file. Does NOT call fsync — any unsynced data
// could still be lost if you forget to call sync() first.
        void close() {
            ::close(fd_);
        }
        // Reads the whole WAL file into memory, so we can walk through it
// and rebuild the memtable on startup. Used only during recovery.
        std::string readAll(const std::string& path){
            int readFD = ::open(path.c_str(), O_RDONLY);
            if (readFD < 0) {
                return "";
            }

        std::string contents;   // will hold the whole file by the end
        char buffer[4096];      // read in 4KB chunks, not all at once
        ssize_t readbytes;  // keep reading chunks until read() says there's nothing left (0)

        while ((readbytes = read(readFD, buffer, sizeof(buffer)))>0) {
            contents.append(buffer , readbytes);
        }
        ::close(readFD);
        return contents;
    }
    private:
    // File descriptor for the open file. -1 means "not open yet" —
    // real file descriptors from open() are always >= 0
        int fd_ = -1;
};
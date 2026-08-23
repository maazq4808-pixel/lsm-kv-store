## 2026-08-22 — fsync measurement
write only: 1 ms
write + fsync: 10,374 ms
~1 ms per fsync. So max ~1000 durable writes/sec if I sync every put.
## 2026-08-23 — LogFile built and tested

Built LogFile class: wraps open/write/fsync/close in a small C++ class.
- open() uses O_APPEND so existing log data is never overwritten
- append() writes bytes via write()
- sync() forces data to physical disk via fsync()
- close() releases the file descriptor (does NOT imply fsync — must sync() first)

Tested with main.cpp: wrote two records, synced, closed.
Confirmed with `cat wal.log` — data persisted correctly.
Confirmed O_APPEND works — running main.cpp again added new lines instead of overwriting.

### Next
- Design WAL record format: [length][checksum][key][value]
- Add CRC32 checksum function
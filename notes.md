## 2026-08-XX — WAL record format complete

Built encodeRecord() in record.h: turns key+value into one binary record.
Layout: [keyLen: 4 bytes][valLen: 4 bytes][checksum: 4 bytes][key][value]

Checksum uses CRC32 (checksum.h) over key+value bytes — lets us detect
torn/corrupted records on replay after a crash.

Verified with xxd: wrote encodeRecord("ali", "555-1234"), confirmed
exact byte layout matches design (23 bytes total).

### Next
- Write decodeRecord() — read a record back from raw bytes, verify checksum
- Then: memtable (std::map) + wiring put()/get()
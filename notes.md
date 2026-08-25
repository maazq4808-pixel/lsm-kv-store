# LSM Key-Value Store — Engineering Log

## 2026-08-22 — fsync measurement

Machine: WSL2 Ubuntu on Windows, ext4 filesystem

Wrote 10,000 records (38 bytes each) to a file two ways:

| Mode | Time |
|---|---|
| write() only | 1 ms |
| write() + fsync() each | 10,374 ms |

~10,000x difference. About 1 ms per fsync.

### What this means
- Durable writes are capped around 1,000/sec if I fsync every put
- The cost is per-fsync-call, not per-byte
- So batching writes and syncing once should get most of the speed back

---

## 2026-08-23 — LogFile built and tested

Built LogFile class (log_file.h): wraps open/write/fsync/close in a
small C++ class. Keeps fd private, everything else uses the wrapper.

- open() uses O_APPEND so existing log data is never overwritten
- append() writes bytes via write()
- sync() forces data to physical disk via fsync()
- close() releases the file descriptor (does NOT imply fsync — must
  sync() first)

Tested with main.cpp: wrote two records, synced, closed.
Confirmed with `cat wal.log` — data persisted correctly.
Confirmed O_APPEND works — running main.cpp again added new lines
instead of overwriting.

Debugged real compiler errors: typo'd O_WRONLY as "0_WRONGLY" (zero
instead of letter O, extra G), missing semicolons, missing semicolon
after class definition (caused a huge cascade of unrelated-looking
errors — good lesson in reading gcc output).

---

## 2026-08-24 — WAL record format complete

Built encodeRecord() in record.h: turns key+value into one binary
record. Layout: [keyLen: 4 bytes][valLen: 4 bytes][checksum: 4 bytes]
[key][value]. Lengths stored as uint32_t (fixed 4 bytes, not text) so
they can be read back exactly without ambiguity.

Checksum uses CRC32 (checksum.h) over key+value bytes — lets us
detect torn/corrupted records on replay after a crash. Didn't
implement CRC32 from scratch — used the standard algorithm, treated
as a known building block (same as not reimplementing std::sort).

Verified with xxd: wrote encodeRecord("ali", "555-1234"), confirmed
exact byte layout matches design (23 bytes total).

Bug along the way: main.cpp had `("ali", "555-1234")` instead of
`encodeRecord("ali", "555-1234")` — the comma operator silently
discarded "ali" and evaluated to just the value string. Looked like
a totally different bug (missing key in output) until traced back to
this.

### Memtable + DB class, working end to end

Built Memtable (memtable.h): thin wrapper around std::map for
put/get. Sorted automatically by key — will matter later for scan().

Built DB (db.h): combines LogFile + encodeRecord + Memtable.
- put(): encode -> write to WAL -> sync -> THEN update memtable.
  This order is the actual durability guarantee — if a crash
  happens between the WAL write and the memtable update, the WAL
  still has the record and it's recoverable.
- get(): reads from memtable only (fast, no disk access)
- open()/close(): currently just forward to the WAL's open/close

Tested: put("ali", "5555-1234"), put("sara", "5555-6789"),
retrieved both back correctly via get(). Confirmed with xxd that
both records are genuinely written to wal.log on disk, correctly
formatted.

### Known gaps
- get() on a missing key silently returns "" instead of signaling
  "not found" — need to fix before adding delete()
- No delete() yet
- No recovery yet: closing and reopening the program currently
  loses the memtable, even though wal.log still has the data on
  disk. This is the next and most important remaining piece of
  Phase 1.

### Next
- Recovery: on db.open(), read wal.log from the start, decode each
  record, verify its checksum, and replay it into memtable_. Stop
  replaying at the first corrupted/incomplete record (that's where
  a crash would have happened).
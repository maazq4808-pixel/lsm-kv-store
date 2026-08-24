## 2026-08-24 — Memtable written

Built Memtable (memtable.h): wraps std::map for put/get.
Not yet tested standalone, not yet wired into anything.

### Next
- Write db.h: DB class combining LogFile + encodeRecord + Memtable
- Update main.cpp to test DB::put() / DB::get() end to end
- Fix Memtable::get() returning "" silently for missing keys
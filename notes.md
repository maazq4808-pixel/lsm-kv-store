## 2026-08-22 — fsync measurement
write only: 1 ms
write + fsync: 10,374 ms
~1 ms per fsync. So max ~1000 durable writes/sec if I sync every put.
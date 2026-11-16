# Logic Gate Simulation
### AND Gates

These SystemC examples model two-input logic gates. Each gate updates its
`out` signal when either input (`a` or `b`) changes.

| a | b | AND output |
|---|---|------------|
| 0 | 0 | 0 |
| 0 | 1 | 0 |
| 1 | 0 | 0 |
| 1 | 1 | 1 |

### OR Gates

These SystemC examples model two-input logic gates. Each gate updates its
`out` signal when either input (`a` or `b`) changes.

| a | b | OR output |
|---|---|-----------|
| 0 | 0 | 0 |
| 0 | 1 | 1 |
| 1 | 0 | 1 |
| 1 | 1 | 1 |


```bash
g++ -std=c++17 and.cc -o and -lsystemc
./and

g++ -std=c++17 or.cc -o or -lsystemc
./or
```


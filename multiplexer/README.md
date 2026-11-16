# 4-to-1 Multiplexer (SystemC)

`mux_4o1.cc` models a 4-to-1 multiplexer. It routes one of four integer inputs
(`i1`–`i4`) to `out`, based on the two selector bits.

## Selector mapping

`sel1` is the high bit and `sel2` is the low bit:

| sel1 | sel2 | Output |
|---:|---:|---|
| 0 | 0 | `i1` |
| 0 | 1 | `i2` |
| 1 | 0 | `i3` |
| 1 | 1 | `i4` |

## Test scenario

The testbench sets the inputs to `1`, `2`, `3`, and `4`, then tries each selector
combination. It waits 1 ns, compares `out` with the expected input, and prints
`PASS` or `FAIL` for each case. The expected outputs are `1`, `2`, `3`, and `4`
in the mapping above.
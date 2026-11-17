# SystemC Clock

## 1. What is a clock?

A clock is a repeating signal that alternates between low and high levels. In
SystemC, `sc_clock` generates this signal automatically, making it useful for
modeling and testing clocked hardware.

## 2. `sc_clock` syntax

```cpp
sc_clock(
  const char* name_,             // clock name
  double period_v_,              // clock period
  sc_time_unit period_unit_,     // period unit
  double duty_cycle_,            // fraction of period spent high
  double start_time_v_,          // time of the first edge
  sc_time_unit start_time_unit_, // start-time unit
  bool posedge_first_ = true     // start with a rising edge
);
```

The arguments specify the clock name, period, period time unit, duty cycle,
start time, start-time unit, and whether the first edge is rising. Here, the
clock starts at 0 ns, begins with a rising edge, and stays high and low for
5 ns each.

This example creates a clock with a 10 ns period and a 50% duty cycle:

```cpp
sc_clock clk("Clock", 10, SC_NS, 0.5, 0, SC_NS, true);
```

## 3. Why clocks matter

Clocks provide a shared timing reference for sequential hardware. A clocked
module can use its edges to decide when to sample inputs or update state.
Changing the period changes the clock frequency; changing the duty cycle
changes how long the signal remains high versus low.

In this example, the monitor reports whether the signal is high or low and the
simulation time whenever the clock changes. The simulation runs for 100 ns,
which covers ten complete clock periods.

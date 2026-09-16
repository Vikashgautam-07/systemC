// uart_skeleton.h
//
// GOAL: Serialize a byte into a UART frame (start bit, 8 data bits, optional
// parity, stop bit) at a configurable baud rate, and deserialize it back.
//
// Do this AFTER the FIFO — a real UART TX module typically pulls bytes from
// a FIFO exactly like the one you just built, so the two projects chain
// together naturally (TX pulls from your CustomFifo when not empty).
//
// UART frame format (8N1, the most common config — start simple, add parity later):
//   idle=1, start=0, [8 data bits, LSB first], stop=1
//
// Baud rate handling: SystemC doesn't know about "baud" — you model it as
// "hold each bit for N clock cycles" where N = clk_freq / baud_rate.
// e.g. 50MHz clock, 115200 baud -> N ≈ 434 clock cycles per bit.

#include <systemc.h>

SC_MODULE(UartTx) {
    sc_in<bool>   clk;
    sc_in<bool>   rst;

    sc_in<bool>          tx_start;
    sc_in<sc_uint<8>>    tx_data;
    sc_out<bool>         tx_busy;
    sc_out<bool>         tx_line;

    // TODO: pick clock frequency + baud rate, compute CYCLES_PER_BIT.
    // Make this a constructor parameter, not a hardcoded #define — you'll want
    // to instantiate TX and RX at different rates in testing to prove your
    // state machine handles arbitrary N correctly, not just one magic number.
    static const unsigned int CYCLES_PER_BIT = /* TODO */ 4;

    SC_CTOR(UartTx) {
        SC_METHOD(fsm);
        sensitive << clk.pos();
    }

private:
    enum class State { IDLE, START_BIT, DATA_BITS, STOP_BIT /* , PARITY_BIT if you add it */ };

    State state = State::IDLE;
    sc_uint<8> shift_reg = 0;   // holds the byte being shifted out
    unsigned int bit_index = 0; // which of the 8 data bits we're currently on
    unsigned int cycle_count = 0; // counts up to CYCLES_PER_BIT, then advances state

    void fsm()
    {
        if (rst.read())
        {
            state = State::IDLE;
            tx_busy.write(0);
            tx_line.write(1);
            bit_index = 0;
            cycle_count = 0;

        }

        switch (state)
        {
            case State::IDLE:
                tx_line.write(1);
                tx_busy.write(0);
                if (tx_start.read())
                {
                    shift_reg = tx_data.read();
                    state = State::START_BIT;
                    cycle_count = 0;
                }
                break;
            
            case State::START_BIT:
                tx_line.write(0);
                tx_busy.write(1);
                if (++cycle_count >= CYCLES_PER_BIT)
                {
                    cycle_count = 0;
                    bit_index = 0;
                    state = State::DATA_BITS;
                }
                break;
            case State::DATA_BITS:
                tx_line.write(shift_reg[bit_index]);
                if (++cycle_count >= CYCLES_PER_BIT)
                {
                    cycle_count = 0;
                    bit_index++;
                    
                    if (bit_index == 8)
                    {
                        state = State::STOP_BIT;
                    }
                }
                break;

            case State::STOP_BIT:
                tx_line.write(1);
                if (++cycle_count >= CYCLES_PER_BIT)
                {
                    cycle_count = 0;
                    state = State::IDLE;
                }
                break;
        }
    }
};

SC_MODULE(UartRx) {
    sc_in<bool>   clk;
    sc_in<bool>   rst;
    sc_in<bool>   rx_line;

    sc_out<bool>       rx_valid;
    sc_out<sc_uint<8>> rx_data;
    sc_out<bool>       rx_error;

    static const unsigned int CYCLES_PER_BIT = /* TODO, must match TX */ 4;

    SC_CTOR(UartRx) {
        SC_METHOD(fsm);
        sensitive << clk.pos();
    }

private:
    enum class State { IDLE, START_BIT, DATA_BITS, STOP_BIT };
    State state = State::IDLE;
    sc_uint<8> shift_reg = 0;
    unsigned int bit_index = 0;
    unsigned int cycle_count = 0;

    // TODO implement. Key difference from TX: you need to SAMPLE rx_line at the
    // MIDDLE of each bit period, not the edge — this is the classic UART RX
    // design point. Sampling at the edge risks catching a transition; sampling
    // at CYCLES_PER_BIT/2 gives you the most stable, centered sample.
    //
    // IDLE:
    //   watch for rx_line falling to 0 (start bit detected) -> state = START_BIT,
    //   reset cycle_count (this is your "edge detected" moment, not a full bit period yet)
    //
    // START_BIT:
    //   wait until cycle_count == CYCLES_PER_BIT/2, confirm rx_line is still 0
    //   (glitch rejection — if it's back to 1, it was noise, go back to IDLE)
    //   then wait the rest of the bit period, move to DATA_BITS
    //
    // DATA_BITS / STOP_BIT: sample at bit-center, same idea as above, shift
    // bits into shift_reg (LSB first, matching TX), pulse rx_valid when done.
    
};

// Suggested testbench structure:
//   - wire UartTx's tx_line directly to UartRx's rx_line (loopback)
//   - drive tx_start with a few test bytes (e.g. 0x00, 0xFF, 0x55, 0xAA — good
//     bit-pattern coverage: all-zero, all-one, alternating)
//   - assert (in the C++ sense, or just print + eyeball) that rx_data matches
//     what you sent, and rx_valid pulses at the right time
//   - THEN try feeding rx_line garbage/noise mid-frame and see if your glitch
//     rejection in START_BIT actually works — this is a great "I thought about
//     edge cases" talking point in an interview
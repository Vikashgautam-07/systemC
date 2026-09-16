#ifndef UART_H
#define UART_H

#include <systemc.h>

SC_MODULE(UartTx) {
public:
    sc_in<bool> clk;
    sc_in<bool> rst;

    sc_in<bool> tx_start;
    sc_in<sc_uint<8>> tx_data;
    sc_out<bool> tx_busy;
    sc_out<bool> tx_line;

    const unsigned int CYCLES_PER_BIT;

    UartTx(sc_module_name name, unsigned int cycles_per_bit);

private:
    enum class State { IDLE, START_BIT, DATA_BITS, STOP_BIT };

    State state = State::IDLE;
    sc_uint<8> shift_reg = 0;
    unsigned int bit_index = 0;
    unsigned int cycle_count = 0;

    void fsm();
};

SC_MODULE(UartRx) {
public:
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<bool> rx_line;

    sc_out<bool> rx_valid;
    sc_out<sc_uint<8>> rx_data;
    sc_out<bool> rx_error;

    const unsigned int CYCLES_PER_BIT;

    UartRx(sc_module_name name, unsigned int cycles_per_bit);

private:
    enum class State { IDLE, START_BIT, DATA_BITS, STOP_BIT };

    State state = State::IDLE;
    sc_uint<8> shift_reg = 0;
    unsigned int bit_index = 0;
    unsigned int cycle_count = 0;

    void fsm();
};

#endif


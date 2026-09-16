#include <systemc.h>
#include <vector>
#include "uart.h"

SC_MODULE(Testbench)
{
    sc_clock clk{"clk", 1, SC_NS};
    sc_signal<bool> rst;

    sc_signal<bool> tx_start;
    sc_signal<sc_uint<8>> tx_data;
    sc_signal<bool> tx_busy;
    sc_signal<bool> tx_line;

    sc_signal<bool> rx_valid;
    sc_signal<sc_uint<8>> rx_data;
    sc_signal<bool> rx_error;

    static const unsigned int CYCLES_PER_BIT = 8;

    UartTx tx;
    UartRx rx;

    SC_CTOR(Testbench)
        : tx("tx", CYCLES_PER_BIT), rx("rx", CYCLES_PER_BIT)
    {
        tx.clk(clk);
        tx.rst(rst);
        tx.tx_start(tx_start);
        tx.tx_data(tx_data);
        tx.tx_busy(tx_busy);
        tx.tx_line(tx_line);

        rx.clk(clk);
        rx.rst(rst);
        rx.rx_line(tx_line);
        rx.rx_valid(rx_valid);
        rx.rx_data(rx_data);
        rx.rx_error(rx_error);

        SC_THREAD(run);
        SC_THREAD(monitor);
    }

    void monitor()
    {
        while (true)
        {
            wait(clk.posedge_event());

            if (tx_start.read())
            {
                std::cout << sc_time_stamp() << " [TX] start, data = 0x"
                          << std::hex << std::uppercase << tx_data.read().to_uint()
                          << std::dec << std::nouppercase << std::endl;
            }

            if (rx_valid.read())
            {
                std::cout << sc_time_stamp() << " [RX] valid, data = 0x"
                          << std::hex << std::uppercase << rx_data.read().to_uint()
                          << std::dec << std::nouppercase << std::endl;
            }

            if (rx_error.read())
            {
                std::cout << sc_time_stamp() << " [RX] error detected" << std::endl;
            }
        }
    }

    void run()
    {
        wait(2, SC_NS);
        rst.write(true);
        wait(clk.posedge_event());
        rst.write(false);

        std::vector<sc_uint<8>> bytes = {0x00, 0xFF, 0x55, 0xAA, 0x3C};
        unsigned int pass_count = 0;

        for (auto value : bytes)
        {
            while (tx_busy.read())
            {
                wait(clk.posedge_event());
            }

            std::cout << sc_time_stamp() << " [TB] sending 0x"
                      << std::hex << std::uppercase << value.to_uint()
                      << std::dec << std::nouppercase << std::endl;

            tx_data.write(value);
            tx_start.write(true);
            wait(clk.posedge_event());
            tx_start.write(false);

            while (!rx_valid.read())
            {
                wait(clk.posedge_event());
            }

            const auto received = rx_data.read();
            const auto expected = value;
            std::cout << sc_time_stamp() << " [TB] received 0x"
                      << std::hex << std::uppercase << received.to_uint()
                      << std::dec << std::nouppercase;

            if (received == expected)
            {
                ++pass_count;
                std::cout << "  PASS" << std::endl;
            }
            else
            {
                std::cout << "  FAIL (expected 0x"
                          << std::hex << std::uppercase << expected.to_uint()
                          << std::dec << std::nouppercase << ")" << std::endl;
            }

            wait(clk.posedge_event());
            wait(clk.posedge_event());
        }

        std::cout << sc_time_stamp() << " [TB] summary: " << pass_count << "/" << bytes.size() << " frames passed" << std::endl;

        sc_stop();
    }
};

int sc_main(int, char* [])
{
    Testbench tb("tb");
    sc_start(2000, SC_NS);
    return 0;
}

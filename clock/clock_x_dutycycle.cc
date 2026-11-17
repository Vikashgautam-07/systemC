#include<systemc.h>

using namespace std;

SC_MODULE(clock_monitor)
{
    sc_in<bool> clk;

    SC_CTOR(clock_monitor)
    {
        SC_METHOD(monitor);
        sensitive << clk;
        dont_initialize();
    }

    void monitor()
    {
        cout << "Clock signal is " << (clk.read() ? "high" : "low")
             << " at time: " << sc_time_stamp() << endl;
    }
};

int sc_main(int argc, char * argv[])
{
    sc_clock clk("Clock", 10, SC_NS, 0.5, 0, SC_NS, true);

    clock_monitor monitor("Clock Monitor");
    monitor.clk(clk);

    cout << "Clock signal with 50%' duty cycle" << endl;
    sc_start(100, SC_NS);
    return 0;
}
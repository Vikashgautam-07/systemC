#include<systemc.h>

using namespace std;

SC_MODULE(ClockedThreadModule)
{
    sc_in<bool> clk;

    SC_CTOR(ClockedThreadModule)
    {
        SC_THREAD(thread_process);
    };

    void thread_process()
    {
        while (true)
        {
            wait(clk.posedge_event());

            cout << "[THREAD] at time: " << sc_time_stamp() << endl;

            wait(1000, SC_NS);
        }
    }
};

SC_MODULE(ClockedMethodModule)
{
    sc_in<bool> clk;
    sc_time next_allowed_time;
    bool is_first_run;

    SC_CTOR(ClockedMethodModule) : next_allowed_time(SC_ZERO_TIME), is_first_run(true)
    {
        SC_METHOD(method_process);
    };

    void method_process()
    {
        if (is_first_run)
        {
            next_allowed_time = sc_time_stamp();
            is_first_run = false;
        }

        if (sc_time_stamp() >= next_allowed_time)
        {
            cout << "[METHOD] at time: " << sc_time_stamp() << endl;
            next_allowed_time = sc_time_stamp() + sc_time(1000, SC_NS);
        }

        next_trigger(clk.posedge_event());
    }

};

int sc_main(int argc, char * argv[])
{
    sc_clock test_clk("clk", 100, SC_NS, 0.5, 0, SC_NS, true);

    ClockedThreadModule thread_module("Thread_Module");
    thread_module.clk(test_clk);

    ClockedMethodModule method_module("Method_Module");
    method_module.clk(test_clk);

    cout << "---Starting the Time Validation Simulation of Clocked thread and method example" << endl;
    sc_start(10000, SC_NS);

    cout << "---Simulation finished" << endl;

    return 0;
}

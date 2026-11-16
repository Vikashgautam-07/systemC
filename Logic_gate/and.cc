#include<systemc.h>

using namespace std;

SC_MODULE(and_logic)
{
    sc_in<bool> a, b;
    sc_out<bool> out;

    SC_CTOR(and_logic)
    {
        SC_METHOD(do_and);
        sensitive << a << b;
    }

    void do_and()
    {
        out.write(a.read() && b.read());
    }
};

SC_MODULE(testbench)
{
    sc_out<bool> a, b;
    sc_in<bool> out;

    SC_CTOR(testbench)
    {
        SC_THREAD(run);
    }
    void run()
    {
        cout << "Test the simulation of AND Logic gate" << endl;

        struct testcases {
                bool a;
                bool b;
                bool expected;
            };

        testcases cases[] = {
                {false, false, false},
                {false, true, false},
                {true, false, false},
                {true, true, true}
            };

        int pass = 0, fail = 0;

        for (auto& tc: cases)
        {
            a.write(tc.a);
            b.write(tc.b);

            wait(1, SC_NS);

            bool accepted = out.read();
            bool ok = (accepted == tc.expected);
            cout << "a = " << tc.a << " b = " << tc.b
                 << " expected = " << tc.expected << " actual = " << accepted
                 << " -> " << (ok ? "PASS" : "FAIL") << endl;
            ok ? pass++ : fail++;
        }
        cout << "Test Results: Passed = " << pass << ", Failed = " << fail << endl;
    }
};

int sc_main(int argc, char* argv[])
{
    sc_signal<bool> a_sig, b_sig, out_sig;

    and_logic and_inst("and_Logic");
    and_inst.a(a_sig);
    and_inst.b(b_sig);
    and_inst.out(out_sig);

    testbench tb("testbench");
    tb.a(a_sig);
    tb.b(b_sig);
    tb.out(out_sig);

    cout << "START : Test the simulation of AND logic gate" << endl;
    sc_start(10, SC_NS);
    cout << "END : Test the simulation of AND logic gate" << endl;
    return 0;
}
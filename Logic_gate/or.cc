#include<systemc.h>

using namespace std;

SC_MODULE(or_logic)
{
    sc_in<bool> a, b;
    sc_out<bool> out;

    SC_CTOR(or_logic)
    {
        SC_METHOD(do_or);
        sensitive << a << b;
    }

    void do_or()
    {
        out.write(a.read() || b.read());
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
        cout << "Test the simulation of the OR logic gate" << endl;

        struct testcases {
            bool a;
            bool b;
            bool expected;
        };

        testcases cases[] = {
            {false, false, false},
            {false, true, true},
            {true, false, true},
            {true, true, true}
        };

        int pass = 0, fail = 0;
        for (auto& tc : cases)
        {
            a.write(tc.a);
            b.write(tc.b);

            wait(1, SC_NS);

            bool accepted = out.read();
            bool ok = (accepted == tc.expected);
            cout << "a = " << tc.a << " b = " << tc.b
                 << " expected = " << tc.expected << " actual = " << accepted
                 << (ok ? " PASS" : " FAIL") << endl;

            if (ok) pass++;
            else fail++;
        }
        cout << "Tests passed: " << pass << ", Tests failed: " << fail << endl;
    }
};

int sc_main(int argc, char* argv[])
{
    sc_signal<bool> a_sig, b_sig, out_sig;

    or_logic or_inst("OR Logic");
    or_inst.a(a_sig);
    or_inst.b(b_sig);
    or_inst.out(out_sig);

    testbench tb("Testbench");
    tb.a(a_sig);
    tb.b(b_sig);
    tb.out(out_sig);

    cout << "START : Simulation of OR Logic gate" << endl;

    sc_start(10, SC_NS);

    cout << "END : Simulation of OR Logic gate" << endl;

    return 0;
}
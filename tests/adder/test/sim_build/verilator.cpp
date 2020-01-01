#include "Vfulladd.h"
#include "verilated.h"
#include "verilated_vpi.h"

#if VM_TRACE
# include <verilated_vcd_c.h>
#endif

vluint64_t main_time = 0;       // Current simulation time

double sc_time_stamp () {       // Called by $time in Verilog
    return main_time;           // converts to double, to match
    // what SystemC does
}

extern "C" {
void vlog_startup_routines_bootstrap(void);

vpiHandle vpi_register_systf(p_vpi_systf_data)
{
	return 0;
}
}

int main(int argc, char** argv, char** env) {
    Verilated::commandArgs(argc, argv);
    Vfulladd* top = new Vfulladd("BYPASSTOP_fulladd");

    vlog_startup_routines_bootstrap();
    VerilatedVpi::callCbs(cbStartOfSimulation);

#if VM_TRACE
    Verilated::traceEverOn(true);

    VerilatedVcdC *tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("dump.vcd");
#endif

    while (!Verilated::gotFinish()) {
        top->eval();
        VerilatedVpi::callValueCbs();
        VerilatedVpi::callTimedCbs();
        VerilatedVpi::callCbs(cbReadOnlySynch);
        VerilatedVpi::callCbs(cbReadWriteSynch);
        VerilatedVpi::callValueCbs();
#if VM_TRACE
        tfp->dump(main_time);
#endif
        main_time++;
    }

    VerilatedVpi::callCbs(cbEndOfSimulation);

#if VM_TRACE
    tfp->close();
#endif

#if VM_COVERAGE
    VerilatedCov::write("coverage.dat");
#endif

    delete top;
    return 0;
}
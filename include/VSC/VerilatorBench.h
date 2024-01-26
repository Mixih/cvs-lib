/** @file
 * This header contains public definitions for the Verilator TestBench class.
 *
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */
#ifndef VSC_VERILATOR_BENCH_H_
#define VSC_VERILATOR_BENCH_H_

#include "VSC/util/Concept.h"

namespace vsc {

/**
 * A generic testbench driver class for wrapping verilated testbenches
 *
 * This should only be used when one design (testbench) is serving as the toplevel, and
 * said design should be the module wrapped by this class.
 *
 * IMPORTANT: The wrapped testbench toplevel module must contain the following inputs to
 * successfully interface with this wrapper:
 *   - clk (scalar) - Global clock input
 *   - rst (scalar) - Global reset input
 * This implies that the simulation testbench must be written in a cycle-driven style
 * instead of a timing-driven style.
 * @tparam TopModule The Model created by verilating the rtl and testbench files for the
 *                   given design
 */
template <VerilatedToplevel TopModule> class VerilatorBench {
private:
    unsigned long cycles;

public:
    /**
     * Used to as an alias to the no-op function for various edge handler callbacks
     */
    static auto noOpHandler = [](TopModule*) {};
    /**
     * Direct access to the verilator topmodule
     */
    TopModule* topmodule;

    VerilatorBench();
    ~VerilatorBench();
    /**
     * Get the number of cycles since the last reset event.
     */
    unsigned long getCycles() { return cycles; }
    /**
     * Reset the simulation model.
     */
    virtual void reset();
    /**
     * Step the simulation model forward by one cycle.
     * @param handleClkRising Callback to execute after the clk rising edge has been
     *                        evaluated.
     * @param handleClkFalling Callback to execute after the clk falling edge has been
     *                         evaluated.
     */
    template <ClkEdgeHandler<TopModule> RiseEdgeHandler,
              ClkEdgeHandler<TopModule> FallEdgeHandler>
    void advanceCycle(RiseEdgeHandler handleClkRising = noOpHandler,
                      FallEdgeHandler handleClkFalling = noOpHandler);

    // disable copying
    VerilatorBench(const VerilatorBench<TopModule>& other) = delete;
    VerilatorBench& operator=(const VerilatorBench<TopModule>& other) = delete;
};

///////////////////////////////////////////////////////////////////////////////
// Template Method Implementations
///////////////////////////////////////////////////////////////////////////////
template <VerilatedToplevel TopModule>
VerilatorBench<TopModule>::VerilatorBench() : cycles{0}, topmodule(new TopModule) {
    // start everything off in a known state
    topmodule->clk = 0;
    topmodule->rst = 0;
}

template <VerilatedToplevel TopModule> VerilatorBench<TopModule>::~VerilatorBench() {
    delete topmodule;
}

template <VerilatedToplevel TopModule> void VerilatorBench<TopModule>::reset() {
    topmodule->rst = 1;
    this->advanceCycle();
    topmodule->rst = 0;
    cycles = 0; // zeroth cycle after reset
}

template <VerilatedToplevel TopModule>
template <ClkEdgeHandler<TopModule> RiseEdgeHandler,
          ClkEdgeHandler<TopModule> FallEdgeHandler>
void VerilatorBench<TopModule>::advanceCycle(RiseEdgeHandler handleClkRising,
                                             FallEdgeHandler handleClkFalling) {
    ++cycles;

    // settle combinatorial logic from changed inputs
    topmodule->clk = 0;
    topmodule->eval_step();
    topmodule->eval_end_step();

    // rising edge of clock
    topmodule->clk = 1;
    topmodule->eval_step();
    topmodule->eval_end_step();
    handleClkRising(topmodule);

    // falling edge of clock
    topmodule->clk = 0;
    topmodule->eval_step();
    topmodule->eval_end_step();
    handleClkFalling(topmodule);
}

} // namespace vsc

#endif /* VERILATOR_BENCH_H_ */

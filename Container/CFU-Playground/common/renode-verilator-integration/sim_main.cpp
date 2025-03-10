//
// Copyright (c) 2021 Antmicro
// Copyright 2021 The CFU-Playground Authors
//
//  This file is licensed under the MIT License.
//  Full license text is available in 'LICENSE' file.
//
#include <verilated.h>
#include "Vcfu.h"
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#if VM_TRACE_VCD
# include <verilated_vcd_c.h>
# define VERILATED_DUMP VerilatedVcdC
# define DEF_TRACE_FILEPATH "simx.vcd"
#elif VM_TRACE_FST
# include <verilated_fst_c.h>
# define VERILATED_DUMP VerilatedFstC
# define DEF_TRACE_FILEPATH "simx.fst"
#endif

#include "src/renode_cfu.h"
#include "src/buses/cfu.h"

RenodeAgent *cfu;
Vcfu *top = new Vcfu;
vluint64_t main_time = 0;

#if VM_TRACE
VERILATED_DUMP *tfp;
#endif

void eval() {
#if VM_TRACE
    main_time++;
    tfp->dump(main_time);
    tfp->flush();
#endif
    top->eval();
}

RenodeAgent *Init() {
    Cfu* bus = new Cfu();

    //=================================================
    // Init CFU signals
    //=================================================
    bus->req_valid = &top->i_cfu_valid;
    bus->rsp_ready = &top->o_cfu_valid;
    bus->req_func_id = (uint16_t *)&top->i_cfu_op;
    bus->req_data0 = (uint32_t *)&top->i_cfu_rs1;
    bus->req_data1 = (uint32_t *)&top->i_cfu_rs2;
    bus->resp_data = (uint32_t *)&top->o_cfu_rd;
    bus->rst = &top->i_rst;
    bus->clk = &top->i_clk;

    //=================================================
    // Init eval function
    //=================================================
    bus->evaluateModel = &eval;

    //=================================================
    // Init peripheral
    //=================================================
    cfu = new RenodeAgent(bus);

#if VM_TRACE
    Verilated::traceEverOn(true);
    tfp = new VERILATED_DUMP;
    top->trace(tfp, 99);
  #ifndef TRACE_FILEPATH
    tfp->open(DEF_TRACE_FILEPATH);
  #else
    tfp->open(TRACE_FILEPATH);
  #endif
#endif

    return cfu;
}


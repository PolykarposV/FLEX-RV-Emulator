// Copyright 2021 The CFU-Playground Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.



module cfu
#(
  parameter WIDTH = 32
)(
  input  wire             i_clk,
  input  wire             i_rst,
  input      [WIDTH-1:0]   i_cfu_rs1,
  input      [WIDTH-1:0]   i_cfu_rs2,
  input      [2:0]    i_cfu_op,
  input               i_cfu_valid,
  input               i_ibus_ack,
  input               i_rf_rreq,
  input [31:0]         i_instruction,
  output              o_cfu_ready,
  output     [WIDTH-1:0]   o_cfu_rd
);

  wire valid  = i_cfu_valid;

  // Responsible for picking cfu[2] or cfu[3]
  wire [31:0] mux_out;
  wire [31:0] quant_out;

  wire signed [31:0] bigs;

  reg enable;
  reg done;

  reg signed [31:0] bigsum; // First instruction cfu[0]
  reg signed [31:0] smallsum; // Second instruction cfu[1]
  reg signed [31:0] clamped; // Third-Fourth instructions cfu[2-3]
  reg signed [31:0] my_o;
  reg [31:0] mycounter; // Fifth instruction cfu[4]
  reg [31:0] total_instructions; // Sixth instruction cfu[5]
  reg [31:0] cpu_instructions; // Seventh instruction cfu[6]
  reg [31:0] r_w_instructions; // Eigth instruction cfu[7]

  //Cycles counter + delay for r/w
  always @(posedge i_clk) begin
    if(i_rst) begin
       mycounter <= 0;
    end
    //else if (i_ibus_ack && i_rf_rreq) begin
    //   if (i_instruction[6:2] == 5'b00000 || i_instruction[6:2] == 5'b01000)
    //          mycounter <= mycounter + 40;
    //end
    else begin
       mycounter <= 200;
    end
  end


  // Total instructions counter
  always @(posedge i_clk) begin
    if(i_rst) begin
       total_instructions <= 0;
    end
    else if (i_ibus_ack && i_rf_rreq) begin
          total_instructions <= total_instructions + 1;
    end
  end

  // Cpu instructions counter
  always @(posedge i_clk) begin
    if(i_rst) begin
       cpu_instructions <= 0;
    end
    else if (i_ibus_ack && i_rf_rreq) begin
       if (!(i_instruction[6:2] == 5'b01100 && i_instruction[31:25] == 7'b0000001))
          cpu_instructions <= cpu_instructions + 1;
    end
  end

  // R/W instructions counter
  always @(posedge i_clk) begin
    if(i_rst) begin
       r_w_instructions <= 0;
    end
    else if (i_ibus_ack && i_rf_rreq) begin
       if (i_instruction[6:2] == 5'b00000 || i_instruction[6:2] == 5'b01000)
          r_w_instructions <= r_w_instructions + 1;
    end
  end

  wire signed [31:0] addbias = $signed(i_cfu_rs1) + $signed(i_cfu_rs2);

  assign mux_out = (($signed(addbias) < 0 && i_cfu_op[1:0] == 2'b11)? 0 : addbias);
  assign quant_out = {{5{mux_out[31]}},mux_out[31:5]};
  assign bigs = $signed(i_cfu_rs1[23:16]) * $signed(i_cfu_rs2[15:12]) + $signed(i_cfu_rs1[15:8]) * $signed(i_cfu_rs2[11:8]);

  always @(posedge i_clk) begin
    if(i_rst) begin
      enable <= 0;
      bigsum <= 0;
      smallsum <= 0;
      clamped <= 0;
    end
    else if (valid) begin
      bigsum <= bigs;
      smallsum <= $signed(bigs[8:0]) + $signed(i_cfu_rs1[7:4]) * $signed(i_cfu_rs2[7:4]) + $signed(i_cfu_rs1[3:0]) * $signed(i_cfu_rs2[3:0]);
      clamped <= ($signed(quant_out) > 7)? 7 : ($signed(quant_out) < -8)? -8 : quant_out; //Clamping to [-8,7]
      enable <= 1'b1;
    end else begin
      enable <= 0;
      bigsum <= 0;
      smallsum <= 0;
      clamped <= 0;
    end
  end

  always @(posedge i_clk) begin
    if(!i_rst & valid & enable) begin
      my_o <= (i_cfu_op[2:0] ==3'b000)? bigsum : (i_cfu_op[2:0] ==3'b001)? smallsum : (i_cfu_op[2:0] == 3'b010 || i_cfu_op[2:0] == 3'b011)? clamped :(i_cfu_op[2:0] ==3'b100)? mycounter :(i_cfu_op[2:0] ==3'b101)? total_instructions : (i_cfu_op[2:0] ==3'b110)? cpu_instructions : r_w_instructions;
    end
    else begin
      my_o <= 0;
    end
    done <= enable;
  end

  assign o_cfu_rd = my_o;
  assign o_cfu_ready = done & valid;


endmodule

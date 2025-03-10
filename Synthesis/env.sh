#set library path in the system
libsPath="~/Pragmatic1.0V"

#set the library path
export ENV_LIBRARY_PATH="${libsPath}/db"
#set the library name
export ENV_LIBRARY_DB="${ENV_LIBRARY_PATH}/Prag_PDK_stdCells_tt_1_27.db"
#set library verilog path
export ENV_LIBRARY_VERILOG_PATH="${libsPath}/verilog"

#set the top design name
export ENV_TOP_DESIGN="serv_rf_top"
#set the desired delay
export ENV_CLK_PERIOD="16666.67"

#clock name and reset (if applicable)
export ENV_CLK_PORT="clk"
# export ENV_CLK_PORT="i_clk"
export ENV_RST_PORT="i_rst"
#set ENV_VIRTUAL_CLOCK to false if the design contains a clock or to true if not
export ENV_VIRTUAL_CLOCK="true"

#set tb name
export ENV_TB_NAME="top_tb"
#vcd name
export ENV_DUMPFILE="$(pwd)/sim/${ENV_TOP_DESIGN}.fsdb"
export ENV_DUT_NAME="DUT"


if [ -z "$SNPS_SYN" ]; then
	echo "set SNPS_SYN"
	exit 1
fi

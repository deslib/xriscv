vlib work
vlog -l v.log +define+SIM +define+DBG +define+FAST_UART +define+UART -f compile.f 
vsim -c -l sim.log -novopt tb_soc -default_radix decimal -do vsim_noprobe.do

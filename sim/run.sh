vlib work
vlog -l v.log +define+SIM +define+DBG -f compile.f 
vsim -l sim.log -novopt tb_soc -default_radix decimal -do vsim.do

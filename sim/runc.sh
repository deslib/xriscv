vlib work
vlog -l v.log +define+SIM -f compile.f 
vsim -c -l sim.log -novopt tb_soc -default_radix decimal -do vsim_noprobe.do

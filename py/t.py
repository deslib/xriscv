import sys

ROM_BASE_ADDR = 0x0
ROM_SIZE      = 0x0
RAM_BASE_ADDR = 0x0
RAM_SIZE      = 0x4000

def bin2rxm(in_fn,rom_fn,ram_fn,coe_fn,ram_bin_fn):
    with open(in_fn,'rb') as f:
        bin_bytes = f.read()
    dws = []
    for i in range(len(bin_bytes)):
        if (i%4) == 0:
            dws.append(bin_bytes[i])
        else:
            dws[-1] = dws[-1] | (bin_bytes[i] << (8*(i%4)))

    #generate the bin file for uart software upgrade
    f_ram_bin = open(ram_bin_fn,'wb')
    ram_bin = bytearray()
    for i in range(len(bin_bytes)):
        if i >= RAM_BASE_ADDR and i < RAM_BASE_ADDR + RAM_SIZE:
            ram_bin.append(bin_bytes[i])
    f_ram_bin.write(bytes(ram_bin))
    f_ram_bin.close()

    f_rom = open(rom_fn,'w')
    f_ram = open(ram_fn,'w')
    f_coe = open(coe_fn,'w')
    f_coe.write("memory_initialization_radix=16;\n");
    f_coe.write("memory_initialization_vector=\n");
    coe_s = []
    for i in range(len(dws)):
        if i*4 >= ROM_BASE_ADDR and i*4 < ROM_BASE_ADDR + ROM_SIZE:
            f_rom.write("%08x\n"%dws[i])
            coe_s.append("%08x"%dws[i])
        elif i*4 >= RAM_BASE_ADDR and i*4 < RAM_BASE_ADDR + RAM_SIZE:
            f_ram.write("%08x\n"%dws[i])

    f_coe.write(",\n".join(coe_s[0:256]))
    f_coe.write(";")
    f_rom.close()
    f_ram.close()
    f_coe.close()

if __name__ == '__main__':
    if sys.argv[1] == 'b2r':
        bin2rxm(sys.argv[2],sys.argv[3],sys.argv[4],sys.argv[5],sys.argv[6])




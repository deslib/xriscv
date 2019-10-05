import sys

ROM_BASE_ADDR = 0x0
ROM_SIZE      = 0x800
RAM_BASE_ADDR = 0x4000
RAM_SIZE      = 0x4000

def bin2rxm(in_fn,rom_fn,ram_fn,mif_fn):
    with open(in_fn,'rb') as f:
        bytes = f.read()
    dws = []
    for i in range(len(bytes)):
        if (i%4) == 0:
            dws.append(bytes[i])
        else:
            dws[-1] = dws[-1] | (bytes[i] << (8*(i%4)))

    f_rom = open(rom_fn,'w')
    f_ram = open(ram_fn,'w')
    f_mif = open(mif_fn,'w')
    f_mif.write("DEPTH=%d;\n"%(ROM_SIZE/4));
    f_mif.write("WIDTH=32;\n");
    f_mif.write("ADDRESS_RADIX=HEX;\n");
    f_mif.write("DATA_RADIX=HEX;\n");
    f_mif.write("CONTENT BEGIN\n");
    for i in range(len(dws)):
        if i*4 >= ROM_BASE_ADDR and i*4 < ROM_BASE_ADDR + ROM_SIZE:
            f_rom.write("%08x\n"%dws[i])
            f_mif.write("%03x:%08x;\n"%(i,dws[i]))
        elif i*4 >= RAM_BASE_ADDR and i*4 < RAM_BASE_ADDR + RAM_SIZE:
            f_ram.write("%08x\n"%dws[i])
    f_mif.write("END;\n");
    f_rom.close()
    f_ram.close()
    f_mif.close()

if __name__ == '__main__':
    if sys.argv[1] == 'b2r':
        bin2rxm(sys.argv[2],sys.argv[3],sys.argv[4],sys.argv[5])




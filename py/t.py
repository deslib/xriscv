import sys

ROM_BASE_ADDR = 0x0
ROM_SIZE      = 0x800
RAM_BASE_ADDR = 0x1000
RAM_SIZE      = 0x1000

def bin2rxm(in_fn,rom_fn,ram_fn):
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
    for i in range(len(dws)):
        if i*4 >= ROM_BASE_ADDR and i*4 < ROM_BASE_ADDR + ROM_SIZE:
            f_rom.write("%08x\n"%dws[i])
        elif i*4 >= RAM_BASE_ADDR and i*4 < RAM_BASE_ADDR + RAM_SIZE:
            f_ram.write("%08x\n"%dws[i])
    f_rom.close()
    f_ram.close()

if __name__ == '__main__':
    if sys.argv[1] == 'b2r':
        bin2rxm(sys.argv[2],sys.argv[3],sys.argv[4])




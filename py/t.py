import sys

def bin2ram(in_fn,out_fn):
    with open(in_fn,'rb') as f:
        bytes = f.read()
    dws = []
    for i in range(len(bytes)):
        if (i%4) == 0:
            dws.append(bytes[i])
        else:
            dws[-1] = dws[-1] | (bytes[i] << (8*(i%4)))

    with open(out_fn,'w') as f:
        for dw in dws:
            f.write("%08x\n"%dw)

if __name__ == '__main__':
    if sys.argv[1] == 'b2r':
        bin2ram(sys.argv[2],sys.argv[3])




# -*- coding: utf-8 -*-
"""
Created on Sun Sep 29 20:04:24 2019

@author: qili
"""

import json
import os


def get_object_from_json(json_fn):
    with open(json_fn) as f:
        s = f.read()
    return json.loads(s)

def reg_decode(regs, file):
    out = ''
    default = '#define %s_reset(csr) \\\n'%file.split('/')[-1][:-2]
    for reg in regs:
        out += 'typedef union {\n'
        out += '  struct {\n'
        if ('type' in reg.keys()):
            t = reg['type']
        else:
            t = 'u32'
        for f in reg['fields']:
            out += '    %-2s %-18s: '%(t,f['name'])
            if isinstance(f['bits'],int):
                out += ' 1;'
            elif isinstance(f['bits'],list):
                out += '%2d;'%(f['bits'][0]-f['bits'][1]+1)
            else:
                print('[ERROR]: invalid bits in %s'%reg['name']) 
            if(len(f['desc']) != 0):
                out += '     // %s'%f['desc']
            out += '\n'
        out += '  } bits;\n'
        out += '  %-2s data;\n'%(t)
        out += '} %s_reg;\n'%reg['name']
        if ('offset' in reg.keys()):
            off_name = '%s_offset'%reg['name']
            out += '#define %-28s%s\n'%(off_name, reg['offset'])
        if ('default' in reg.keys()):
            def_name = '%s_default'%reg['name']
            out += '#define %-28s%s\n'%(def_name, reg['default'])
        if ('offset' in reg.keys()) and ('default' in reg.keys()):
            default += '          csr[%s] =  %s; \\\n'%(off_name, def_name)
        out += '\n'
    out += '\n'
    default += '\n'
    o = open(file, 'w')
    o.write(out)
    o.write(default)
    o.close()
                

def main():
    jfile = "C:/work/riscv/xriscv/emulator/csr_reg_m.json"
    hfile = jfile[0:-4]+'h'
    try:
        regs = get_object_from_json(jfile)
    except:
        print('decode ', jfile, ' failed')
    if (os.path.exists(hfile)):
        os.remove(hfile)
    reg_decode(regs, hfile)


if __name__ == "__main__":
    main()

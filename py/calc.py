def div(dividend, divisor):
    remainder = dividend << 1
    quotient = 0
    for i in range(32):
        print("%16x %8x %8x %8x"%(remainder,remainder>>32,divisor,quotient))
        if (remainder>>32) >= divisor:
            quotient = (quotient << 1) + 1
            remainder = (((remainder>>32) - divisor)<<32) + (remainder&0xffffffff) 
        else:
            quotient = (quotient << 1) + 0
        remainder = remainder << 1
    return quotient,remainder >> 33

def num2comp(num,length=32):
    return num&0xffffffff

def comp2num(comp,length=32):
    if comp >> length - 1:
        return 0 - (~comp&((1<<length)-1)) - 1
    else:
        return comp

def mult(a,b,optype):
    al = a&0xffff
    ah = (a>>16)&0x7fff
    bl = b&0xffff
    bh = (b>>16)&0x7fff
    sign = (a>>31)^(b>>31)

    al_m_bl = al * bl
    al_m_bh = al * bh << 16
    ah_m_bl = ah * bl << 16
    ah_m_bh = ah * bh << 32

    print(sign,al,ah,bl,bh,ah_m_bh, ah_m_bl, al_m_bh, al_m_bl)

    #if sign != 0:
    #    al_m_bl += 0x7FFFFFFF00000000
    #    al_m_bh += 0x7FFF000000000000
    #    ah_m_bl += 0x7FFF000000000000
    #    ah_m_bh += 0x1000000000000000

    num = ah_m_bh + ah_m_bl + al_m_bh + al_m_bl

    print(sign,al,ah,bl,bh,ah_m_bh, ah_m_bl, al_m_bh, al_m_bl, comp2num(num&0xffffffff))
    return comp2num(num+(1<<62),63)






    

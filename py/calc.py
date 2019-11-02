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

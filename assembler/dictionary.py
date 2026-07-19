opcodes = {                                
    "ldr": "000", #load to register
    "mov": "001", #load to memory
    "adc": "010", #add with carry
    "sbc": "010", #subtract with carry
    "and": "010", #bitwise and
    "orr": "010", #bitwise or
    "xor": "010", #bitwise ex-or
    "ars": "010", #arithmetic right shift
    "lrs": "010", #logical right shift
    "lls": "010", #logical left shift
    "psh": "011", #push to stack
    "pop": "011", #pop from stack
    "jmi": "100", #jump to immediate
    "jmr": "101", #jump to register val
    "bzc": "101", #branch if zero flag clear
    "bzs": "101", #branch if zero flag set
    "bcc": "101", #branch if carry flag clear
    "bcs": "101", #branch if carry flag set
    "sec": "110", #set carry flag
    "clc": "110", #clear carry flag
    "sez": "110", #set zero flag
    "clz": "110"  #clear zero flag
}

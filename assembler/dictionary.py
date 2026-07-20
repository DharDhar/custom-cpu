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
    "jmp": "100", #jump to immediate or register
    "bzc": "101", #branch if zero flag clear
    "bzs": "101", #branch if zero flag set
    "bcc": "101", #branch if carry flag clear
    "bcs": "101", #branch if carry flag set
    "sec": "110", #set carry flag
    "clc": "110", #clear carry flag
    "sez": "110", #set zero flag
    "clz": "110"  #clear zero flag
}

alu_instrs = {
    "adc": "000",
    "sbc": "001",
    "and": "010",
    "orr": "011",
    "xor": "100",
    "ars": "101",
    "lrs": "110",
    "lls": "111"
}

jump_instrs = {
    "jmp": "000",
    "bzc": "001",
    "bzs": "010",
    "bcc": "011",
    "bcs": "100" 
}

flag_instrs = {
    "sec": "00",
    "clc": "01",
    "sez": "10",
    "clz": "11"
}


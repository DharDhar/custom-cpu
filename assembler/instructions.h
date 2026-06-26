//Contains the assembly instructions and a short description for each

const char *instr_str[] = {"ldr", //load to register
						   "ldm", //load to memory
						   "adc", //add with carry
						   "sbc", //subtract with carry
						   "and", //bitwise and of registers
						   "orr", //bitwise or of registers
						   "xor", //bitwise ex-or of registers
						   "ars", //arithmetic right shift a register
						   "lrs", //logical right shift a register
						   "lls", //logical left shift a register
						   "psh", //push to stack
						   "pop", //pop from stack
						   "jmr", //jump to register val
						   "jmi", //jump to immediate
						   "bzc", //branch if zero flag clear
						   "bzs", //branch if zero flag set
						   "bcc", //branch if carry flag clear
						   "bcs"};//branch if carry flag set
								

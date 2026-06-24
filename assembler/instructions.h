//Contains the assembly instructions and a short description for each
#define INSTRUCTION_COUNT 22

typedef struct {
	char *string;
	int opcode;
} Instruction;

const Instruction instr_str[] = {{"ldr", 0b000}, //load to register
		 					     {"ldm", 0b001}, //load to memory
							     {"adc", 0b010}, //add with carry
						   		 {"sbc", 0b010}, //subtract with carry
						   		 {"and", 0b010}, //bitwise and
						   		 {"orr", 0b010}, //bitwise or
						   		 {"xor", 0b010}, //bitwise ex-or
						   		 {"ars", 0b010}, //arithmetic right shift
						   		 {"lrs", 0b010}, //logical right shift
						   		 {"lls", 0b010}, //logical left shift
						   		 {"psh", 0b011}, //push to stack
						   		 {"pop", 0b011}, //pop from stack
						   		 {"jmi", 0b100}, //jump to immediate
						   		 {"jmr", 0b101}, //jump to register val
						   		 {"bzc", 0b101}, //branch if zero flag clear
						   		 {"bzs", 0b101}, //branch if zero flag set
						   		 {"bcc", 0b101}, //branch if carry flag clear
						   		 {"bcs", 0b101}, //branch if carry flag set
								 {"sec", 0b110}, //set carry flag
								 {"clc", 0b110}, //clear carry flag
								 {"sez", 0b110}, //set zero flag
								 {"clz", 0b110}};//clear zero flag
		


from dictionary import opcodes, alu_instrs, jump_instrs, flag_instrs
from io import StringIO
from fnmatch import fnmatchcase
import sys

labels_line_num = {}
labels_instr_num = {}

def main():
    label_scraper()
    with open("program.txt", "r", encoding="utf-8") as file:
        for line_num, line in enumerate(file):
            instr_buf = StringIO()
            fields = (line.strip()).split(" ")
            
            if not fields[0]: #skip if line empty
                continue
    
            if line_num+1 not in labels_line_num.values():
                if (fields[0] not in opcodes.keys()):
                    throw_error("ILLEGAL INSTRUCTION", line_num)
                opcode = int(opcodes.get(fields[0]), 2)
            
                if (fields[0] == "ldr"): #ldr imm rd
                    if (int(fields[1]) > 1023):
                        throw_error("IMMEDIATE TOO BIG", line_num)
                    if (fields[2][0] != 'r'):
                        throw_error("ILLEGAL INSTRUCTION", line_num)

                    imm = int(fields[1])
                    rd = int(fields[2][1])

                    print(f"{imm:010b}{rd:03b}{opcode:03b}", file=instr_buf, end="")
                    print(instr_buf.getvalue())
                    instr_buf.close()

                elif (fields[0] == "mov"): #mov dest source
                    for i in range(1,2):
                        if (fields[i][0] != 'r' and fields[i] != "@MP"):
                            throw_error("ILLEGAL INSTRUCTION", line_num)

                    if (fields[1] == "@MP"):
                        dest = 8 #0b1000
                    else:
                        dest = int(fields[1][1:])

                    if (fields[2] == "@MP"):
                        src = 8 #0b1000
                    else:
                        src = int(fields[2][1:])

                    print(f"00000{dest:04b}{src:04b}{opcode:03b}", file=instr_buf, end="")
                    print(instr_buf.getvalue())
                    instr_buf.close()

                elif (fields[0] in alu_instrs.keys()): #func rs1 rs2 rd
                    if (len(fields) != 4):
                        throw_error("ILLEGAL INSTRUCTION", line_num)
                    for i in range (1,3):
                        if (fields[i][0] != 'r'):
                            throw_error("ILLEGAL INSTRUCTION", line_num)

                    func = int(alu_instrs.get(fields[0]), 2)
                    rs1 = int(fields[1][1:])
                    rs2 = int(fields[2][1:])
                    rd = int(fields[3][1:])

                    print(f"0{func:03b}{rd:03b}{rs2:03b}{rs1:03b}{opcode:03b}", file=instr_buf, end="")
                    print(instr_buf.getvalue())
                    instr_buf.close()

                elif (fields[0] == "psh" or fields[0] == "pop"): #psh/pop rx/PC
                    if (fields[1][0] != 'r' and fields[1] != "PC" and fields[1] != "MP"):
                        throw_error("ILLEGAL INSTRUCTION", line_num)

                    if (fields[0] == "psh"):
                        op = 0
                    else:
                        op = 1
                    
                    if (fields[1] == "PC"):
                        print(f"00000000{op:01b}1000{opcode:03b}", file=instr_buf, end="")
                    else:
                        r = int(fields[1][1:])
                        print(f"00000000{op:01b}0{r:03b}{opcode:03b}", file=instr_buf, end="")
                    print(instr_buf.getvalue())
                    instr_buf.close()

                elif (fields[0] in jump_instrs.keys()): #func rx/imm
                    if (fields[1] in labels_instr_num.keys()):
                        imm = labels_instr_num.get(fields[1])
                        func = int(jump_instrs.get(fields[0]), 2)
                        opcode = 5 #0b0101

                        print(f"{imm:010b}{func:03b}{opcode:03b}", file=instr_buf, end="")
                    elif (fnmatchcase(fields[1], "r?")):
                        r = int(fields[1][1:])
                        func = int(jump_instrs.get(fields[0]), 2)
                        opcode = 4 #0b0100
                        print(f"0000000{r:03b}{func:03b}{opcode:03b}", file=instr_buf, end="")
                    else:
                        throw_error("UNDEFINED LABEL", line_num);
                    print(instr_buf.getvalue())
                    instr_buf.close()

                elif (fields[0] in flag_instrs.keys()): #func
                    func = int(flag_instrs.get(fields[0]), 2)

                    print(f"00000000000{func:02b}{opcode:03b}", file=instr_buf, end="")
                    print(instr_buf.getvalue())
                    instr_buf.close()

                        
 
def throw_error(error_str, line_num):
    if (error_str == "SPACE IN LABEL"):
        sys.exit(f"At line {line_num+1}: Labels cannot contain space")
    elif (error_str == "DUPLICATE LABEL"):
        sys.exit(f"At line {line_num+1}: Duplicate label definition")
    elif (error_str == "IMMEDIATE TOO BIG"):
        sys.exit(f"At line {line_num+1}: Immediate cannot exceed 1023 for ldr")
    elif (error_str == "ILLEGAL INSTRUCTION"):
        sys.exit(f"At line {line_num+1}: Illegal instruction")
    elif (error_str == "LABEL SAME AS REGISTER"):
        sys.exit(f"At line {line_num+1}: Labels cannot be named after registers")
    elif (error_str == "UNDEFINED LABEL"):
        sys.exit(f"At line {line_num+1}: Undefined label")

def label_scraper():
    instr_num = 0

    with open("program.txt", "r", encoding="utf-8") as file:
        for line_num, line in enumerate(file):
            fields = (line.strip()).split(" ")
            stripped_line = line.strip()

            if ":" in stripped_line:
                label = (fields[0])[:-1]  #strip trailing ':'
                if (fnmatchcase(label, "r?")):
                    throw_error("LABEL SAME AS REGISTER", line_num)
                if " " in stripped_line:
                    throw_error("SPACE IN LABEL", line_num)
                if label in labels_line_num:
                    throw_error("DUPLICATE LABEL", line_num)
                labels_line_num[label] = (line_num+1)
                labels_instr_num[label] = instr_num
            else:
                instr_num+=1


if __name__ == "__main__":
    main()


from dictionary import opcodes
import sys

labels_line_num = {}
labels_instr_num = {}

instr_num = 0

with open("program.txt", "r", encoding="utf-8") as file:
    for line_num, line in enumerate(file):
        fields = (line.strip()).split(" ")
        stripped_line = line.strip()

        if ":" in stripped_line:
            label = (fields[0])[:-1]  #strip trailing ':'

            if " " in stripped_line:
                sys.exit(f"At line {line_num+1}: Labels cannot contain space")
            if label in labels_line_num:
                print(f"At line {line_num+1}: Duplicate label definition '{label}'")
                sys.exit(f"Previously defined at line {labels_line_num[label]}")
            labels_line_num[label] = (line_num+1)
            labels_instr_num[label] = instr_num
        else:
            instr_num+=1

#print(labels_line_num)
#print(labels_instr_num)
        

with open("program.txt", "r", encoding="utf-8") as file:
    for line_num, line in enumerate(file):
        fields = (line.strip()).split(" ")
        if line_num+1 not in labels_line_num.values():
            print(opcodes.get(fields[0]))

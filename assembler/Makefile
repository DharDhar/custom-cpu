CXX = gcc
TARGET = assembler.bin
P ?= program.txt
O ?= program.bin

all: $(TARGET)

$(TARGET): assembler.c
	$(CXX) -o $(TARGET) $<

.PHONY: clean run
clean: $(TARGET)
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET) $(P) $(O)


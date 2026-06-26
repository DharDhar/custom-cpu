CXX = gcc
TARGET = assembler.bin

all: $(TARGET)

$(TARGET): assembler.c
	$(CXX) -o $(TARGET) $<

.PHONY: clean run
clean: $(TARGET)
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

